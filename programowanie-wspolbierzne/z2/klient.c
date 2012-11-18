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
#include <time.h>

#include "err.h"
#include "komunikaty.h"


int main(int argc, char * argv[])
{	
	int id, id_raport;
	long numer, priorytet;
	long losowanie = 0;
	komunikat kom;
	
	if (argc != 3) 
		fatal("> Użycie: %s <numer> <priorytet>", argv[0]);
		
/* To nie klienci kończą program lecz wykonawcy, stąd ignoruję Ctrl+C. */
	if (signal(SIGINT, SIG_IGN) == SIG_ERR)
		syserr("> Błąd w signal!");
	
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
		syserr("> Błąd w signal!");
		
/* Otwieranie kolejek... */		
	if ((id = msgget(KLUCZ, 0666 | IPC_EXCL)) == -1)
		fatal("> Bład w msgget, prawdopodobnie nie jest uruchomiony żaden wykonawca!");
		
	if ((id_raport = msgget(KLUCZ_KLI, 0666 | IPC_EXCL)) == -1)
		syserr("> Bład w msgget!");
		
	numer = atol(argv[1]);      /* Numer klienta         */
	priorytet = atol(argv[2]);  /* Posiadany priorytet   */
	
	printf_2("> Gotowy...\n");

	while (1)
	{
/* Trzeba wylosować liczby a i b... */
		kom.mtype = priorytet;
		kom.dane.klient = numer;
		kom.dane.priorytet = priorytet;
		kom.dane.typ = PYTANIE;
		srand((unsigned) (getpid() + time(NULL) + losowanie++));
		kom.dane.a = rand()%10;
		srand((unsigned) (getpid() + time(NULL) + losowanie++));
		kom.dane.b = rand()%10;
	
/* Do wykonawców aby policzyli... */
		printf_2("> Wysyłam dane do wykonawcy...\n");
		if (msgsnd(id, &kom, ROZMIAR, 0) == -1) 
			syserr("> Błąd w msgsnd!");
			
/* Do nadzorcy aby wiedział... */
		kom.mtype = 1;
		printf_2("> Wysyłam raport do nadzorcy...\n");
		if (msgsnd(id_raport, &kom, ROZMIAR, 0) == -1) 
			syserr("> Błąd w msgsnd!");
			
/* Oczekiwanie na odpowiedź wykonawcy... */		
		printf_2("> Czekam na odpowiedź...\n");
		if ((msgrcv(id, &kom, ROZMIAR, numer, 0)) <= 0)
			syserr("> Błąd w msgrcv!");
			
/* Sprawdzenie tego co wczytano. */
		switch (kom.dane.typ)
		{
			case ODPOWIEDZ: /* Raport dla nadzorcy. */
				printf_2("> Jest odpowiedz!\n");
				kom.mtype = 1;
				kom.dane.priorytet = priorytet;
				kom.dane.klient = numer;
				printf_2("> Wysyłam raport dla nadzorcy...\n");
				if (msgsnd(id_raport, &kom, ROZMIAR, 0) == -1) 
					syserr("> Błąd w msgsnd!");
/* Czy klient ,,zasypia''(gdy reszta z dzielenia przez 100 jest równa 4) */
				srand((unsigned) (getpid() + time(NULL) + losowanie++));
				if ((rand() % 100) == 4) {
					printf_2("> Zasypiam...\n");
					sleep(1);
				}
				break;

			case KONIEC: /* Potwierdzamy nadejście końca do nadzorcy. */
				printf_2("> Wysyłąm raport dla nadzorcy o zakończeniu pracy...\n");
				kom.mtype = 1;
				kom.dane.typ = KONIEC;
				kom.dane.klient = numer;
				kom.dane.priorytet = priorytet;
				if (msgsnd(id_raport, &kom, ROZMIAR, 0) == -1) 
					syserr("> Błąd w msgsnd!");
				exit(0);
				/* break; zbędne... */
				
			default: /* Kontrola błedów */
				printf_2("> Niewiadomo co nastapilo...)\n");
		}
	}		

/* Tutaj nigdy się nie znajdziemy! */

}
