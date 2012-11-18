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


int id, id_raport, id_raport_wyk;
long i, K, M;
long * tablica_priorytety_pyt;
long * tablica_klienci_pyt;
long * tablica_priorytety_odp;
long * tablica_klienci_odp;   


/* Procedura kończąca */
void koniec()
{
/* Jeśli tu jesteśmy oznacza to, że klienci są juz zamknięci */
	komunikat kom;
	kom.mtype = 1;
	kom.dane.typ = KONIEC;
	long suma_pyt = 0, suma_odp = 0;
	
/* Informujemy nadzorcę wykonawców, że klienci skończyli już pracę */
	if (msgsnd(id_raport_wyk, &kom, ROZMIAR, 0) == -1) 
			syserr("> Błąd w msgsnd!");

	printf("> Informacje zbiorcze:\n");
	for (i = 0; i < M; i++) {
		printf("> %ld klient wysłał %ld pytań i otrzymał %ld odpowiedzi, odrzuconych %ld.\n", 
		i + 1, tablica_klienci_pyt[i], tablica_klienci_odp[i], tablica_klienci_pyt[i] - tablica_klienci_odp[i]);
		suma_pyt += tablica_klienci_pyt[i];
		suma_odp += tablica_klienci_odp[i];
	}
	printf("> Ogółem. pytań: %ld, odpowiedzi: %ld, odrzuconych: %ld\n", suma_pyt, suma_odp, suma_pyt - suma_odp);
	suma_pyt = 0; suma_odp = 0;
	for (i = 0; i < K; i++) {
		printf("> %ld priorytet wysłał %ld pytań i otrzymał %ld odpowiedzi, odrzuconych %ld.\n", 
		i + 1, tablica_priorytety_pyt[i], tablica_priorytety_odp[i], tablica_priorytety_pyt[i] - tablica_priorytety_odp[i]);
		suma_pyt += tablica_priorytety_pyt[i];
		suma_odp += tablica_priorytety_odp[i];
	}
	printf("> Ogółem. pytań: %ld, odpowiedzi: %ld, odrzuconych: %ld\n", suma_pyt, suma_odp, suma_pyt - suma_odp);
	
	printf_1("> Zamykanie kolejki raportów...\n");
	if (msgctl(id_raport, IPC_RMID, 0) == -1)
    syserr("> Błąd w msgctl RMID!");
		
/* Zwalnianie pamięci... */
	free(tablica_priorytety_pyt);
	free(tablica_klienci_pyt);
	free(tablica_priorytety_odp);
	free(tablica_klienci_odp);   
	
	printf_1("> Koniec!\n");
	exit(0);
}


int main(int argc, char * argv[])
{
	char tmp1[10], tmp2[10];
	komunikat kom;
	long licznik = 0;
	int typ;

	if (argc != 4)
		fatal("Użycie: %s <priorytety> <klienci> <typ losowo|modulo>", argv[0]);
		
/* To nie klienci kończą program lecz wykonawcy, stąd ignoruję Ctrl+C. */
	if (signal(SIGINT, SIG_IGN) == SIG_ERR)
		syserr("> Błąd w signal!");
	
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
		syserr("> Błąd w signal!");
	
/* Otwieranie kolejek... */		
	if ((id = msgget(KLUCZ, 0666 | IPC_EXCL)) == -1)
		fatal("> Bład w msgget, prawdopodobnie nie jest uruchomiony żaden wykonawca!");	
		
	if ((id_raport_wyk = msgget(KLUCZ_WYK, 0666 | IPC_EXCL)) == -1)
		syserr("> Bład w msgget!");
		
	if ((id_raport = msgget(KLUCZ_KLI, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
		syserr("> Bład w msgget!");

	K = atol(argv[1]);
	M = atol(argv[2]);

/* Inicjalizowanie tablic... */
	tablica_priorytety_pyt = malloc(K * sizeof(long));
	tablica_klienci_pyt = malloc(M * sizeof(long));
	tablica_priorytety_odp = malloc(K * sizeof(long));
	tablica_klienci_odp = malloc(M * sizeof(long));
	for (i = 0; i < K; i++) 
	{
		tablica_priorytety_pyt[i] = 0;
		tablica_priorytety_odp[i] = 0;
	}
	for (i = 0; i < M; i++) 
	{
		tablica_klienci_pyt[i] = 0;
		tablica_klienci_odp[i] = 0;		
	}
	
/* Wybór algorytmu przydzielania priorytetów */	
	if (strcmp(argv[3], "losowo") == 0)
		typ = LOSOWO;
	else if (strcmp(argv[3], "modulo") == 0)
		typ = MODULO;
	else 
		fatal("> Użycie: %s <priorytety> <klienci> <typ losowo|modulo>", argv[0]);

	printf_1("> Uruchamianie klientów");
	for (i = 1; i <= M; i++)
		switch (fork())
		{
			case -1:
				syserr("> Błąd w fork!");
			
			case 0:
				srand((unsigned) i);
				sprintf(tmp1, "%ld", i + K + 1);
				sprintf(tmp2, "%ld", ((typ == LOSOWO) ? (rand() % K) : (i % K)) + 1);
				execl("./klient", "klient", tmp1, tmp2, 0);
				syserr("> Błąd w execl!");
			
/* Bez default, proces macierzysty nic tu nie musi robić poza fork(). */
			
		}
	
	printf_1("> Rozpoczęcie nasłuchiwania...\n");
	
	while (1)
	{		
/* Odczytujemy raporty... */
		if (msgrcv(id_raport, &kom, ROZMIAR, 0, 0) == -1)
			syserr("> Błąd w msgrcv!");

/* Wyświetlenie informacji od klientów */
		switch (kom.dane.typ)
		{
			case PYTANIE: /* Klient informuje o zadaniu pytania */
				printf_1("> Klient: %ld zapytał: %d + %d = ?\n", kom.dane.klient, kom.dane.a, kom.dane.b); 
				tablica_priorytety_pyt[kom.dane.priorytet - 1]++;
				tablica_klienci_pyt[kom.dane.klient - K - 2]++;
				break;
				
			case ODPOWIEDZ: /* Klient informuje o udzielonej odpowiedzi */
				printf_1("> Klient: %ld dostał odpowiedź: %d + %d = %d\n", kom.dane.klient, kom.dane.a, kom.dane.b, kom.dane.c); 
				tablica_priorytety_odp[kom.dane.priorytet - 1]++;
				tablica_klienci_odp[kom.dane.klient - K - 2]++;
				break;
				
			case KONIEC: /* Klient informuje o kończeniu pracy przez wykonawców */
				printf_1("> Klient: %ld zakończył pracę (zakończonych razem: %ld)\n", kom.dane.klient, licznik);
				if (++licznik == M) 
					koniec(); /* Kończymy pracę gdy każdy z klientów powiadomi o końcu */
				break;
			
			default: /* Kontrola błedów */
				printf_1("> Niewiadomo co nastapilo...\n");
		}
	}
	
/* Tutaj nigdy się nie znajdziemy! */
	
}
