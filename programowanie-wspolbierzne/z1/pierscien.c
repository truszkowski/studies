#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "err.h"
#include "konf.h"


int main(int argc, char *argv[]) {
  
	pid_t pid, gpid = getpid();
  int pipe_dsk[2], N, K, i;
  char tekst[MAX], tmp[10];
  struct pakiet paczka;
	struct info plan;
	
	
  if (argc != 3) fatal("Użycie: %s liczba_procesów liczba_obrotów\n",argv[0]);
	
  N = atoi(argv[1]); 
  K = atoi(argv[2]);
  
	if ((N <= 0) || K < 0) fatal("Podaj dodatnie liczby!\n");
  
  PIPE(pipe_dsk);
	printf("[%d] Konfiguracja:\n", gpid);
	printf("[%d] N= %d K= %d\n", gpid, N, K);
	printf("[%d] Utworzono łącze nienazwane...(o=%d, z=%d)\n\n", gpid, pipe_dsk[0], pipe_dsk[1]);

  /* Tworzenie struktury pierścienia: */
  switch (pid = fork()) {
    case -1:
	  	ERR("fork!");
	  
    case 0: /* Proces potomny: */
	/* Nie zamykamy deskryptora do zapisu - potrzebny jest dla ostaniego procesu, *
	 * zamykając go tutaj zamykamy go również dla procesów potomnych.             */
			sprintf(tmp, "%d", pipe_dsk[0]);
      execl("./proces", "proces", tmp, 0);
      ERR("execl!");
	  
    default: /* Proces macierzysty: */
	  	printf("[%d] Tworzenie pierscienia...\n", gpid);
			printf("[%d] Utworzono proces potomny: %d\n", gpid, pid);
	  	CLOSE(pipe_dsk[0]);
  /* Pozostalo wysłać do deskryptora informacje o parametrach pierścienia: */
	  	plan.licznik = N;
	  	plan.ostatni = pipe_dsk[1];			
	  	WRITE(pipe_dsk[1], &plan, INFO_ROZ);
  /* W pętli dodajemy kolejno słowa do przetworzenia przez pierscień procesów.     *
   * Wczytanie pustej linni powoduje wyjście z pętli. Zauważmy, iż pusta wiadomość *
   * zostanie wysłana przed wyjściem z pętli stanowi ona znak dla pierścienia, iż  *
	 * po przetworzeniu wszystkich słów krążących w nim program ma sie zakończyć.    *
	 * jeśli wyślemy zadlugi tekst, zostanie on podzielony na kilka pakietów.        */
	  	while (1) {
				fgets(tekst, MAX, stdin);
				for (i = 0; i < strlen(tekst); i++) if (tekst[i] == '\n') tekst[i] = '\0';
				tekst[MAX-1]='\0';
				strncpy(paczka.tresc, tekst, MAX);
				paczka.licznik = N*K;
				
				printf("[%d] Wysyłanie danych do pierścienia...(%d)\n", gpid, strlen(paczka.tresc));
    		WRITE(pipe_dsk[1], &paczka, sizeof(paczka));
				if (strlen(paczka.tresc) == 0) break;
	  	}
			printf("[%d] Wczytano pusty wiersz, kończenie programu...\n", gpid);
			printf("[%d] Zamykanie pierścienia...\n", gpid);
  /* Czekamy na zakonczenie wykonywania się procesu potomnego: */
	  	WAIT(0);
			printf("[%d] Pierścień zamknięty.\n", gpid);
			printf("[%d] Zamykanie deskryptora...\n", gpid);
	  	CLOSE(pipe_dsk[1]);
			printf("[%d] Koniec!\n", gpid);
    	exit(0);
  } 
}
