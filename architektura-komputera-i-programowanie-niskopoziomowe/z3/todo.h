#ifndef __ARM_TO_DO_H__
#define __ARM_TO_DO_H__

/*
 * W tym pliku znajduje sie parser instrukcji z stdin. 
 * zapisuje on do tablicy kolejno instrukcja po instrukcji
 * jaka ma byc wykonana, nastepnie wszystany zbior instrukcji 
 * przesyla do "wykonania". Czyli optymalizacji. Obiekt
 * klasy ARMCPU jest to wirtualny model procesora ARM, ktory na
 * podstawie sobie znanej heurystyki optymalizuje wczystany kod
 * przerabiajac go jednoczesnie na kod ARM. I to dostaniemy na 
 * stdout.
 * Wiecej o "czarnej skrzynce" - ARMCPU w pliku armcpu.h
 * 
 */


#include <vector>
#include <string>

namespace std {
		
/* pojedyncza instrukcja do wykonania (co, jakie zmienne lub stale) */
	class TODOEnt {
	public:
		string op, line; int arg[3]; bool reg[3], ignored;
		TODOEnt(void) { arg[0]=arg[1]=arg[2]=0; reg[0]=reg[1]=reg[2]=true; ignored = false; }
		~TODOEnt(void) {}
		bool check(int x) { // czy zmienna wystepuje tutaj
			for (int i = 0; i < 3; ++i) if (reg[i] && arg[i] == x) return true; 
			return false;
		}
	};

/* zbior instrukcji do wykonania */
	class TODO {
	public:
		unsigned int in, out;
		vector<TODOEnt> todo;
		TODO(void) { in = 0; out = 0;}
		~TODO(void) { clear(); }
		void push(TODOEnt e) { todo.push_back(e); } // dodaj instrukcje
		void clear(void) { todo.clear(); }          // czysc zbior
/* odzczytanie instrukcji z stdin */
		void load_from_stdin(void);
/* wczytanie kolejnej linni */
		void load_next_line(void);
/* wykonaj ciag instrukcji */
		void do_todo(void);
	};


	
}

#endif
