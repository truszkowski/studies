#include <cstdlib>
#include "todo.h"
#include "armcpu.h"

#define BUFLEN 10

using namespace std;

// czytamy z stdin
void TODO::load_from_stdin(void) {
	clear();
	int lines;
	
	scanf("%u %u %u",&lines,&in,&out);

	for (unsigned int i = 0; i < lines; ++i)
		load_next_line();

}

// wczytwanie kolejnej linni z stdin
void TODO::load_next_line(void) {
	TODOEnt tde;
	char buf[4][BUFLEN];
	scanf("%s%s%s%s", buf[0], buf[1], buf[2], buf[3]);
	
	tde.op = buf[0];
	
	for (unsigned int i = 0; i < 3; ++i) {
// argumenty...
		tde.reg[i] = (buf[i+1][0] == 't');
		if (tde.reg[i]) tde.arg[i] = atoi(buf[i+1] + 1);
		else            tde.arg[i] = atoi(buf[i+1]);
	}
	
	push(tde);
	
}

void TODO::do_todo(void) {
	printf("\n\t.text\n\t.global funkcja\nfunkcja:\n");
// tutaj cpu juz sobie poradzi z ciagem instrukcji...
	ARMCPU cpu; // cos zainicjuje...
	cpu.todo(*this); // wykona tak aby bylo optymalnie...
// ...i tutaj zakonczy, wykonujac czynnosci "czyszczace" (szczegoly: armcpu.*)
}
