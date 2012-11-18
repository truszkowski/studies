#ifndef __ARM_CPU_H__
#define __ARM_CPU_H__

/*
 * Ten plik zawiera wirtualny model procesora ARM. 
 * W klasie ARMCPU tablice sa odpowiedzialne za zbior
 * zmiennych, stos(jego "esp"/"sp") a takze rejestry 
 * (od 1 do REGISTR). ARMCPU na dziendobry odklada na stos 
 * odpowiednie rejestry, ktore maja na sam koniec byc 
 * odtworzone poprzez ich zdjecie. 
 *
 * Czasem rejestry moga byc za male do pomieszczenia 
 * wszystkich zmiennych. Wtedy to potrzeba zastosowac 
 * jakas heurystyke w przydziale zmiennym rejestrow.
 * Do tego celu wybralem metode usuwania z rejestru tej
 * zmienej, ktora wystapi najpozniej od miejsca gdzie 
 * jestesmy. W szczegolnosci moze sie zdarzyc ze zmienna 
 * juz w ogole nie wystapi i tym bardziej spokojnie 
 * mozemy ja zdjac a co wiecej zapisac do pamieci, jesli 
 * miejsci sie w tablicy "wyjsciowej".
 * Dzieki temu w rejestrze przechowujemy zmienne, ktore beda 
 * najdluzej uzywane. Natomiast stale, jesli nie moga byc argumentem 
 * natychmiastowym, przepisuje na rejestry CONST[1]
 * i CONST[2]. Te rejestry pelnia tez role rejestrow pomocniczych.
 * Aby zmienna wrzucic znow do rejestru trzeba ja "sciagnac"
 * ze stosu, o ile jej wartosc jest nam potrzebna. Podobnie
 * zmienna wrzucamy na stos jak jest taka potrzeba.
 *
 * Zapis Zmiennych do tablicy ""wyjsciowej" jak juz wspomnialem 
 * moze odbywac sie w trakcje wywolywania instrukcji. W ten sposob 
 * gdy juz wykonamy wszystkie instrukcje, wiemy ze pozostaly do
 * zapisania tylko te zmienne, ktore sa w rejestrze(o ile i je 
 * trzeba zapisywac).
 *
TODO- * Kazda zmienna wkladana do rejestru, zna swoje nastepne
TODO- * wystapienie, wiec moznatu dokonywac operacji zrownoleglajacych 
TODO- * funkcja: check_for_parallel(); +
 *
 */


#include <vector>
#include <string>
#include "todo.h"

const int NOCHECK = -1;        // dajemy znac ze chcemy szukać (musi byc < 0)
const int REGISTR = 10;        // ile mamy rejestrow do dyspozycji dla zmiennych
const int GOTOBED = 1000000;   // nie ma juz nastepnego wystapienia
const int NOTUSED = -3;        // aktualnie nieuzywana zmienna
const int NUMOFBYTES = 4;      // ilu bitowy(bajtowy) procesor
const int VARIABLES_LIMIT = 5000; // max dla zmiennych
const int STACK_LIMIT = 5000; // max dla stosu
// rejestry dla stalych pierwszy arg nie mozebyc stala wiec daje -1...
const int CONST[3] = { -1, 11, 12 }; 

namespace std {

	class Registry { // jaka zmienna siedzi w rejestrze
	public:
		int var;
		Registry(void) { var = 0; }
		~Registry(void) {}
	};
	
	class Variable { // zmienna, wie czy jest w rejestrze, na stosie i kiedy znow nastapi.
	public:
		int reg, stack, next, modify;
		Variable(void) { modify = false; reg = stack = NOTUSED; }
		~Variable(void) {}
	};

	class ARMCPU { // model ARM
	private:
		vector<Variable> variables;
		vector<Registry> registry;
		int stacklvl, stacksize;
	public:
		ARMCPU(void);
		~ARMCPU(void);
		void todo(TODO &);
	private:
		int find_last(TODO &, int);
		int find_next(TODO & td, int tdi, int x);
		void pop_stack(int);
		void push_stack(TODO &, int);
		void clear_registry(TODO &);
		int get_free_registry(TODO &, int, int, bool);
		void todo_line(TODO &, int);
	};

}	

#endif
