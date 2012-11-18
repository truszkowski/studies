/* Piotr Truszkoski --- pt209223 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include "err.h"
#include "komunikaty.h"


int id, id_raport;
long i, K; 
long * tablica_priorytety;


/* Procedura kończąca */
void koniec(int sig)
{
	komunikat kom;
	kom.mtype = 1;
	kom.dane.typ = KONIEC;
	
	printf_1("> Nadzorca: SIGINT!\n");
/* Wykonawcy przechwytują u siebie SIGINT i wysyłaja komunikaty końca do klientów. */
	
/* Wykonawcy o niczym już nie informują. Czekamy aby nadzorca *
 * klientów dał znak, że klieci się zakończyli.               */
	if (msgrcv(id_raport, &kom, ROZMIAR, 0, 0) == -1)
		syserr("> Błąd w msgrcv!");

/* Wysyłamy do wykonawców informacje o definitywnym końcu pracy */
	kom.mtype = 1;
	kom.dane.typ = KONIEC;
	for (i = 1; i <= K; i++) 
	{
		if (msgsnd(id, &kom, ROZMIAR, 0) == -1) 
			syserr("> Błąd w msgsnd!");
		kom.mtype++;
	}

	printf_1("> Oczekiwanie na zamknięcie wykonawców...\n");
	for (i = 0; i < K; i++) 
		if (wait(0) == -1)
			syserr("> Błąd w wait!");

	printf("> Informacje zbiorcze:\n");
	for (i = 0; i < K; i++)
		printf("> Wykonawca o priorytetcie %ld dokonał obliczeń %ld razy\n", i + 1, tablica_priorytety[i]);

	printf_1("> Zamykanie kolejek...\n");
	if (msgctl(id, IPC_RMID, 0) == -1)
    syserr("> Błąd w msgctl RMID!");
		
	if (msgctl(id_raport, IPC_RMID, 0) == -1)
    syserr("> Błąd w msgctl RMID!");
		
/* Zwalnianie pamięci... */
	free(tablica_priorytety);
				
	printf_1("> Koniec!\n");
  exit(0);
}


int main(int argc, char * argv[])
{
	char tmp[10];
	komunikat kom;
	struct sigaction ctrlc;
  sigset_t blokuj;


	if (argc != 2)
		fatal("> Użycie: %s <priorytety>", argv[0]);
		
/* Obsługa Ctrl+C */
  sigemptyset (&blokuj);
  ctrlc.sa_handler = koniec;
  ctrlc.sa_mask = blokuj;
  ctrlc.sa_flags = 0;
	
  if (sigaction (SIGINT, &ctrlc, 0) == -1)
    syserr("Błąd w sigaction!");
	
	if ((id = msgget(KLUCZ, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
    syserr("> Błąd w msgget!");
		
	if ((id_raport = msgget(KLUCZ_WYK, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
    syserr("> Błąd w msgget!");
	
	K = atoi(argv[1]);
	
/* Inicjalizowanie tablic... */
	tablica_priorytety = malloc(K*sizeof(long));
	for (i = 0; i < K; i++)
		tablica_priorytety[i] = 0;

	printf_1("> Uruchamianie wykonawców...\n");
	for (i = 1; i <= K; i++)
		switch (fork())
		{
			case -1:
				syserr("> Błąd w fork!");
				
			case 0:
/* Każdy z wykonawców zna swój priorytet(przez ktory jest rozpoznawany) */
				sprintf(tmp, "%ld", i);
				execl("./wykonawca", "wykonawca", tmp, 0);
				syserr("> Błąd w execl!");

/* Bez default, proces macierzysty nic tu nie musi robić poza fork(). */
			
		}

	printf_1("> Rozpoczęcie nasłuchiwania...\n");	
	
	while (1)
	{	
/* Nadzorca czeka na informacje od wykonawców. */	
		if (msgrcv(id_raport, &kom, ROZMIAR, 0, 0) == -1)
			syserr("> Błąd w msgrcv!");
	
/* Nadzorca zbiera informacje od wykonawców */
		printf_1("> Obliczono: %d + %d = %d (%ld,%ld)\n", kom.dane.a, kom.dane.b, kom.dane.c, kom.dane.klient, kom.dane.priorytet); 	
		tablica_priorytety[kom.dane.priorytet - 1]++;
	}
	
	/* Tutaj nigdy się nie znajdziemy! */
	
}
