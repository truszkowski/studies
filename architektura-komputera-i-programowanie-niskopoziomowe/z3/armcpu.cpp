#include "armcpu.h"
#include "todo.h"

using namespace std;

// sprawdzenie czy x moze byc podany "#x" czy musi byc zaladowany do pamieci
inline bool const_validator(int x) {
	int len = 0;		
// jak sa pod rzad 24 zera to ok
	for (int i = 0; i < 64; ++i)
		if (x & (1 << (i % 32))) len = 0;
		else if (++len >= 24) return true;
	return false;
}
	
ARMCPU::ARMCPU(void) {
	stacklvl = 0; // tutaj startujemy
// i w tym miejscu odkladamy rejestry ktorych wartosci niechcemy zgubic
	printf("\tstmfd sp!, { r4, r5, r6, r7, r8, r9, r10, r11, r12, lr }\n");
	variables.resize(VARIABLES_LIMIT); // potem stacklvl nam powie oile bedzie trzeba sie cofnac
	stacklvl = STACK_LIMIT; // jeszcze ustawienie rozmiarow tablic(moznabyloby tez dynamicznie)
	stacksize = 0;
}

ARMCPU::~ARMCPU(void) {
// powrot do miejsca na stosie gdzie bylismy na poczatku
	if ((STACK_LIMIT - stacklvl) != 0) {// po co przesuwac sp o zero
		if (const_validator((STACK_LIMIT - stacklvl) * NUMOFBYTES)) {
			printf("\tadd sp, sp, #%d\n", (STACK_LIMIT - stacklvl) * NUMOFBYTES);
		} else {
			printf("\tldr r12, =%d\n", (STACK_LIMIT - stacklvl) * NUMOFBYTES);
			printf("\tadd sp, sp, r12\n");
		} 
	}
// i zdjecie zachowanych starych rejestrow
	printf("\tldmfd sp!, { r4, r5, r6, r7, r8, r9, r10, r11, r12, pc }\n");
// pa pa
}


inline string get_op(string & s) {
	string r = "Ojej!";
	if (s == "+") r = "add";
	else if (s == "*") r = "mul"; 
	else if (s == "-") r = "sub"; 
	else if (s == "&") r = "and"; 
	else if (s == "|") r = "orr"; 
	else if (s == "^") r = "eor"; 
	else if (s == "=") r = "mov"; 
	else if (s == "<<") r = "lsl";
	else if (s == ">>") r = "lsr";
	return r;
}

// Dla zbioru td instrukcji w instrukcji nr: tdi robimy:
void ARMCPU::todo_line(TODO & td, int tdi) {
	if (!td.todo[tdi].ignored) {
		
		string op = get_op(td.todo[tdi].op), typearg[3]; 
		int areg[3];

		// dla zmiennych
		for (int i = 0; i < 3; ++i) 
			if (td.todo[tdi].reg[i]) {
				typearg[i] = "r";
				
				bool save = false; // trszeba uwzglednic mozliwosc wystapienia zmiennej wiele razy
				if (i == 0) // jest to istotne tylko gdy zmienna jest 1 i 2/3
					save = !(
						td.todo[tdi].arg[0] == td.todo[tdi].arg[1] ||
						td.todo[tdi].arg[0] == td.todo[tdi].arg[2]
					);
				
				areg[i] = get_free_registry(td, tdi, td.todo[tdi].arg[i], save) + 1; // + 1 bo nie chcemy od 0
			}
		
		variables[td.todo[tdi].arg[0]].modify = true; // zmodyfikowany moze byc tylko 1 (1 musi byc rejestrem)
		
		// dla stalych 
		for (int i = 0; i < 3; ++i) 
			if (!td.todo[tdi].reg[i]) {
				if (!const_validator(td.todo[tdi].arg[i])) {
					printf("\tldr r%d, =%d\n", CONST[i], td.todo[tdi].arg[i]);
					areg[i] = CONST[i];
					typearg[i] = "r";
				} else {
					typearg[i] = "#";
					areg[i] = td.todo[tdi].arg[i];
				}
			}
		
//		check_for_parallel(); // TODO
			
		// wypis	
		if (op == "lsl" || op == "lsr")
			printf("\tmov r%d, %s%d, %s %s%d\n", 
				areg[0], typearg[1].c_str(), areg[1], op.c_str(), typearg[2].c_str(), areg[2]);
		else if (op == "mov")
			printf("\t%s r%d, %s%d\n", 
				op.c_str(), areg[0], typearg[1].c_str(), areg[1]);
		else 
			printf("\t%s r%d, %s%d, %s%d\n", 
				op.c_str(), areg[0], typearg[1].c_str(), areg[1], typearg[2].c_str(), areg[2]);
			
	}
}

void ARMCPU::todo(TODO & td) {

	for (int tdi = 0; tdi < td.todo.size(); ++tdi) 
		todo_line(td, tdi);	// dla tdi-tej instrukcji robimy tak:
	
	clear_registry(td);
}

// kombajn do zarzadzania rejestrami
int ARMCPU::get_free_registry(TODO & td, int tdi, int var, bool fst) {
	int regout;
	
	if (variables[var].reg != NOTUSED) {//uzywana
		regout = variables[var].reg;
		variables[var].next = find_next(td, tdi, var);
	} else {//nieuzywana
		if (registry.size() < REGISTR) {// jest wolny rejestr
			variables[var].reg = registry.size();
			regout = variables[var].reg;
			variables[var].next = find_next(td, tdi, var);
			Registry newr; newr.var = var;
			registry.push_back(newr);
			if (var <= td.in && !fst) {// sciagamy z pamieci
				if (const_validator((var - 1)*NUMOFBYTES)) {
					printf("\tldr r%d, [ r0, #%d ]\n", regout + 1, (var - 1)*NUMOFBYTES);
				} else {
					printf("\tldr r%d, =%d\n", CONST[1], (var - 1)*NUMOFBYTES);
					printf("\tldr r%d, [ r0, r%d ]\n", regout + 1, CONST[1]);
				}
			}
		} else {// trzeba sprowadzic zmienna!
			regout = find_last(td, tdi); // robimy miejsce
			push_stack(td, registry[regout].var);
			
			variables[var].reg = regout;
			variables[var].next = find_next(td, tdi, var);
			
			if (variables[var].stack != NOTUSED) // ostatnia wartosc zmiennej jest na stosie
				pop_stack(var);
			else if (var <= td.in && !fst) {// sciagamy z pamieci
				if (const_validator((var - 1)*NUMOFBYTES)) {
					printf("\tldr r%d, [ r0, #%d ]\n", regout + 1, (var - 1)*NUMOFBYTES);
				} else {
					printf("\tldr r%d, =%d\n", CONST[1], (var - 1)*NUMOFBYTES);
					printf("\tldr r%d, [ r0, r%d ]\n", regout + 1, CONST[1]);
				}			
			}
			// potrzeba nam tylko rejestru nic wiecej nie musimy robic
		}
	}
	
	registry[regout].var = var;
	return regout;	
}

void ARMCPU::pop_stack(int v) { // wywolujac, jest wiadome, ze zmienna jest na stosie !
//printf("POP\n");
	if ((variables[v].stack - stacklvl + 1)*NUMOFBYTES > 0) {
		if (const_validator((variables[v].stack - stacklvl + 1)*NUMOFBYTES)) {	
			printf("\tadd sp, sp, #%d\n", (variables[v].stack - stacklvl + 1)*NUMOFBYTES);
			printf("\tldmfd sp!, { r%d }\n", variables[v].reg + 1);		
		} else {
			printf("\tldr r%d, =%d\n", CONST[1], (variables[v].stack - stacklvl + 1)*NUMOFBYTES);
			printf("\tadd sp, sp, r%d\n", CONST[1]);
			printf("\tldmfd sp!, { r%d }\n", variables[v].reg + 1);
		}
		
	} else {
		
		if (const_validator((- variables[v].stack + stacklvl - 1)*NUMOFBYTES)) {
			printf("\tsub sp, sp, #%d\n", (- variables[v].stack + stacklvl - 1)*NUMOFBYTES);	
			printf("\tldmfd sp!, { r%d }\n", variables[v].reg + 1);
		} else {
			printf("\tldr r%d, =%d\n", CONST[1], (- variables[v].stack + stacklvl - 1)*NUMOFBYTES);
			printf("\tsub sp, sp, r%d\n", CONST[1]);
			printf("\tldmfd sp!, { r%d }\n", variables[v].reg + 1);	
		}
	}
	
	stacklvl = stacklvl + variables[v].stack - stacklvl + 1;
}


void ARMCPU::push_stack(TODO & td, int v) {
//printf("PUSH\n");
	if (variables[v].next == GOTOBED) { 
		if (v <= td.out && variables[v].modify) {
// zapisujemy bo juz i tak nie bedziemy uzywac a zapisac i tak musimy 
// potem bedzie latwiej zrzucic do pamieci to co jest w rejestrach niz skakac po stosie
			if (const_validator((v - 1)*NUMOFBYTES)) { // spr
				printf("\tstr r%d, [ r0, #%d ]\n", variables[v].reg + 1, (v - 1)*NUMOFBYTES);
			} else {
				printf("\tldr r%d, =%d\n", CONST[1], (v - 1)*NUMOFBYTES);
				printf("\tstr r%d, [ r0, r%d ]\n", variables[v].reg + 1, CONST[1]);
			}
		}// i juz nie ma po co skladac na stos
	} else { // a tutaj trzeba na stos
		if (variables[v].stack != NOTUSED) {// zmienna jest juz na stosie
			if ((variables[v].stack - stacklvl)*NUMOFBYTES > 0) {
				if (const_validator((variables[v].stack - stacklvl)*NUMOFBYTES)) {
					printf("\tadd sp, sp, #%d\n", (variables[v].stack - stacklvl)*NUMOFBYTES);
					printf("\tstmfd sp!, { r%d }\n", variables[v].reg + 1);
				} else {
					printf("\tldr r%d, =%d\n", CONST[1], (variables[v].stack - stacklvl)*NUMOFBYTES);
					printf("\tadd sp, sp, r%d\n", CONST[1]);
					printf("\tstmfd sp!, { r%d }\n", variables[v].reg + 1);
				}	
				stacklvl = variables[v].stack - 1;
			} else {
				if (const_validator((stacklvl - variables[v].stack)*NUMOFBYTES)) {
					printf("\tsub sp, sp, #%d\n", (stacklvl - variables[v].stack)*NUMOFBYTES);
					printf("\tstmfd sp!, { r%d }\n", variables[v].reg + 1);
				} else {
					printf("\tldr r%d, =%d\n", CONST[1], (stacklvl - variables[v].stack)*NUMOFBYTES);
					printf("\tsub sp, sp, r%d\n", CONST[1]);
					printf("\tstmfd sp!, { r%d }\n", variables[v].reg + 1);
				}	
				stacklvl = variables[v].stack - 1;
			}

		} else {// zmiennej jeszcze nie ma na stosie
			if (const_validator((STACK_LIMIT - stacksize - stacklvl)*NUMOFBYTES)) {
				printf("\tadd sp, sp, #%d\n", (STACK_LIMIT - stacksize - stacklvl)*NUMOFBYTES);
				printf("\tstmfd sp!, { r%d }\n", variables[v].reg + 1);
			} else {
				printf("\tldr r%d, =%d\n", CONST[1], (STACK_LIMIT - stacksize - stacklvl)*NUMOFBYTES);
				printf("\tadd sp, sp, r%d\n", CONST[1]);
				printf("\tstmfd sp!, { r%d }\n", variables[v].reg + 1);
			}
			
			++stacksize;
			stacklvl = STACK_LIMIT - stacksize;
			variables[v].stack = stacklvl;
		}
	}
	variables[v].reg = NOTUSED;
}

int ARMCPU::find_last(TODO & td, int tdi) {
// szukamy ktora z aktywnych zmiennych bedacych w rejestrze jest uzyta najpozniej
	int max = 0;
	for (int i = 1; i < REGISTR; ++i) 
		if ((registry[i].var != td.todo[tdi].arg[0]) && (td.todo[tdi].reg[0]) &&
		    (registry[i].var != td.todo[tdi].arg[1]) && (td.todo[tdi].reg[1]) &&
		    (registry[i].var != td.todo[tdi].arg[2]) && (td.todo[tdi].reg[2]))
			if (variables[registry[max].var].next <= variables[registry[i].var].next)
				max = i;

	return max;
}

void ARMCPU::clear_registry(TODO & td) {
	for (int i = 0; i < registry.size(); ++i)
		if (registry[i].var <= td.out && variables[registry[i].var].modify) {
			if (const_validator((registry[i].var - 1)*NUMOFBYTES)) {
				printf("\tstr r%d, [ r0, #%d ]\n", i + 1, (registry[i].var - 1)*NUMOFBYTES);
			} else {
				printf("\tldr r%d, =%d\n", CONST[1], (registry[i].var - 1)*NUMOFBYTES);
				printf("\tstr r%d, [ r0, r%d ]\n", i + 1, CONST[1]);
			}
		}
}

int ARMCPU::find_next(TODO & td, int tdi, int x) {
	for (int i = tdi + 1; i < td.todo.size(); ++i)
		if (td.todo[i].check(x)) return i;
	return GOTOBED; // tutaj staje sie jednoczesnie kandydatem to wywalenia z rejestru 
	// GOTOBED > od dowolnego numeru instrukcji
}
