/* Piotr Truszkowski */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "err.h"
#include "sem.h"


/* Semafory uogólnione */

void PU(SemUgl * su, int n)
{
	int i;
	struct sembuf * tmp;
	if (n > 0) {
		tmp = malloc(sizeof(struct sembuf)*n);
		for (i = 0; i < n; i++) {
			tmp[i].sem_num = su->nr_sem;
			tmp[i].sem_op = -1;
			tmp[i].sem_flg = 0;
		}
		if (semop(su->id_sem, tmp, n) == -1)
			syserr("> Błąd w semop!");
		free(tmp);
	}
}

void VU(SemUgl * su, int n)
{
	int i;
	struct sembuf * tmp; 
	if (n > 0) {
		tmp = malloc(sizeof(struct sembuf)*n);
		for (i = 0; i < n; i++) {
			tmp[i].sem_num = su->nr_sem;
			tmp[i].sem_op = 1;
			tmp[i].sem_flg = 0;
		}
		if (semop(su->id_sem, tmp, n) == -1)
			syserr("> Błąd w semop!");
		free(tmp);
	}
}

/* Semafory binarne */

void PB(SemBin * sb)
{
	struct sembuf tmp;
	tmp.sem_num = sb->nr_sem;
	tmp.sem_op = -1;
	tmp.sem_flg = 0;
	if (semop(sb->id_sem, &tmp, 1) == -1)
		syserr("> Błąd w semop!");
}

void VB(SemBin * sb)
{
	struct sembuf tmp;
	tmp.sem_num = sb->nr_sem;
	tmp.sem_op = 1;
	tmp.sem_flg = 0;
	if (semop(sb->id_sem, &tmp, 1) == -1)
		syserr("> Błąd w semop!");
}

/* uwaga tak naprawde dwukrotne wykonanie VB i tak zwiekszy    *
 * wartosc semafora o 2. Owo wyroznienie na operacje PU(),VU() *
 * i PB(), VB() jedynie ma na celu zwrocenie uwagi iz takie    *
 * rodzaje wystarcza do pelnego opisu(oraz czytelniejszy kod   *
 * programu).                                                  */
