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
#include <errno.h>

#include "err.h"
#include "komunikaty.h"


int id, id_raport, czy_koniec = 0;
komunikat kom;
long priorytet;
	

void koniec(int sig)
{
	printf_2("> Wykonawca: SIGINT!\n");
	printf_2("> Kończę pracę, powiadamiam klientów...\n");
	
/* Mimo iż pętla jest nieskończona program wyjdzie z niej za sprawą *
 * komunikatu kończącego od nadzorcy.                               */
	while (1)
	{
/* Czy jest komunikat o właściwym priorytecie. */
		if (msgrcv(id, &kom, ROZMIAR, priorytet, 0) == -1) {
			if (errno != ENOMSG) 
				syserr("> Błąd w msgrcv!");
			else /* Niema nic dla nas, dodatkowo czekamy na niższych priorytetach */
				if (msgrcv(id, &kom, ROZMIAR, -priorytet, 0) == -1)
					syserr("Bład w msgrcv!");
		}
		switch (kom.dane.typ)
		{
			case PYTANIE: /* odprawiamy z kwitkiem klienta... */
				kom.mtype = kom.dane.klient;
				kom.dane.typ = KONIEC;
				if (msgsnd(id, &kom, ROZMIAR, 0) != 0) 
					syserr("> Błąd w msgsnd!");
				printf_2("> Klient odprawiony...\n");
				break;
				
			case KONIEC: /* idziemy do domu...*/
				printf_2("> Koniec pracy...\n");
				exit(0);
				
			default: /* Kontrola błedów */
				printf_2("> Niewiadomo co nastapilo...\n");
				
		}
	}	
}


int main(int argc, char * argv[])
{
	
	struct sigaction ctrlc;
  sigset_t blokuj;

	if (argc != 2) 
		fatal("Użycie: %s <priorytet>", argv[0]);
		
/* Obsługa Ctrl+C */
  sigemptyset (&blokuj);
  ctrlc.sa_handler = koniec;
  ctrlc.sa_mask = blokuj;
  ctrlc.sa_flags = 0;
	
  if (sigaction (SIGINT, &ctrlc, 0) == -1)
    syserr("Błąd w sigaction!");
		
	priorytet = atoi(argv[1]);
	
/* Crtl+C... */
	if (signal(SIGINT, koniec) == SIG_ERR)
		syserr("> Błąd w signal!");
	
/* Otwieramy kolejki... */
	if ((id = msgget(KLUCZ, 0666 | IPC_EXCL)) == -1)
		syserr("Bład w msgget!");
	
	if ((id_raport = msgget(KLUCZ_WYK, 0666 | IPC_EXCL)) == -1)
		syserr("Bład w msgget!");
	
	printf_2("> Nasłuchuję...\n");
	
	while (1)
	{
/* Czy jest komunikat o właściwym priorytecie. */
		if (msgrcv(id, &kom, ROZMIAR, priorytet, 0) == -1) {
			if (errno != ENOMSG) 
				syserr("> Błąd w msgrcv!");
			else /* Niema nic dla nas, dodatkowo czekamy na niższych priorytetach */
				if (msgrcv(id, &kom, ROZMIAR, -priorytet, 0) == -1)
					syserr("Bład w msgrcv!");
		}
		switch (kom.dane.typ)
		{
			case PYTANIE: /* nasz klient nasz pan, liczymy... */
				printf_2("> Obliczam...\n");
				kom.mtype = kom.dane.klient;
				kom.dane.c = kom.dane.a + kom.dane.b;
				kom.dane.typ = ODPOWIEDZ;
				printf_2("> Wysyłam odpowiedź klientowi...\n");
				if (msgsnd(id, &kom, ROZMIAR, 0) != 0) 
					syserr("> Błąd w msgsnd!");
				kom.mtype = 1;
				kom.dane.priorytet = priorytet; 
				printf_2("> Wysyłam raport do nadzorcy...\n");
				if (msgsnd(id_raport, &kom, ROZMIAR, 0) != 0) 
					syserr("> Błąd w msgsnd!");
				break;

			default: /* Kontrola błedów */
				printf_2("> Niewiadomo co nastapilo...\n");
				
		}
	}	
	
/* Tutaj nigdy się nie znajdziemy! */
		
}
