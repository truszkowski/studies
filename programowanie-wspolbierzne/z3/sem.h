/* Piotr Truszkowski */

#ifndef _SEM_H_
#define _SEM_H_

typedef struct {
	int id_sem;
	short int nr_sem;
} Sem;

/* Semafory uogólnione */
typedef Sem SemUgl;
extern void PU(SemUgl*, int);
extern void VU(SemUgl*, int);

/* Semafory binarne */
typedef Sem SemBin;
extern void PB(SemBin*);
extern void VB(SemBin*);

#endif
