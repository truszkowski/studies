#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#include "err.h"
#include "konf.h"


int main (int argc, char *argv[]) {
  
	pid_t pid, gpid = getpid();
  int odczyt_dsk, zapis_dsk, pipe_dsk[2];
	int N, czy_ostatni = 0, licznik = 3;
  struct pakiet paczka;
	struct info plan;
	char tmp[10];
	
	
  if (argc != 2) fatal("Użycie: %s deskryptor_do_odczytu\n", argv[0]);
  
  odczyt_dsk = atoi(argv[1]);
	
	
  /* Oczekiwanie na parametry pierścienia: */
	printf("[%d] Czekanie na odczyt danych...\n", gpid);
  READ(odczyt_dsk, &plan, INFO_ROZ);
	printf("[%d] Odczytano: dsk_odczyt=%d licznik=%d dsk_ostatni=%d\n", gpid, odczyt_dsk, plan.licznik, plan.ostatni);
	
  N = plan.licznik;
	
  if (N == 1) { /* Koniec pierścienia */
		zapis_dsk = plan.ostatni;
		czy_ostatni = 1;
  } else {
  /* Tworzenie deskryptora do komunikacji z następnym procesem: */
		PIPE(pipe_dsk);
		printf("[%d] Utworzenie łącza nienazwanego...(o=%d, z=%d)\n", gpid, pipe_dsk[0], pipe_dsk[1]);
	
		switch (pid = fork()) {
	  	case -1:
				ERR("fork!");
		
	  	case 0: /* Proces potomny */
				CLOSE(pipe_dsk[1]);
				sprintf(tmp, "%d", pipe_dsk[0]);
				execl("./proces","proces", tmp, 0);
				ERR("execl!");
		
	  	default: /* Proces macierzysty */
	/* Zamykamy deskryptor, powierzony dla ostatniego prosesu, *
	 * ponieważ został już przekazany potomkowi.               */
				CLOSE(plan.ostatni);
				CLOSE(pipe_dsk[0]);
				printf("[%d] Utworzono proces potomny: %d\n", gpid, pid);
				plan.licznik = --N;
				WRITE(pipe_dsk[1], &plan, INFO_ROZ);
				zapis_dsk = pipe_dsk[1];
		}/* Koniec switch, wyjdzie tylko macierzysty */	
  }
	
	printf("[%d] Gotowy! Komunikacja: odczyt=%d zapis=%d\n", gpid, odczyt_dsk,zapis_dsk);
	/* Wg specyfikacji zadania deskryptory nie przepełniają się. Zwróćmy jednak  *
	 * uwagę co się wtedy dzieje. Załóżmy, ze do obiegu została wpuszczona taka  *
	 * ilość pakietów, że zapełniła wszystkie deskryptory oraz każdy proces      *
	 * probuje zapisać jakiś pakiet (czyli: (N+1)*pojemność_dsk/rozmiar_pakietu  *
	 * pakietów) - zakleszczenie. Można zauważyć, że już liczba pakietów o jeden * 
	 * mniejsza, już zapewnia żywotność. Wtedy są dwie możliwości: albo jeden z  *
	 * deskryptorów może jeszcze pomieścić jeden pakiet, albo jeden z procesów   *
	 * odczytuje pakiet z deskryptora. Oba przypadki można sprowadzić do jednego.*
	 * Zatem jeden z deskryptorów ma wolne miejsce, tam wskakuje pakiet, który   *
	 * próbował zapisać któryś deskryktor, tenże deskryptor po zapisie odczytuje *
	 * dane z wcześniejszego itd, postępując cyklicznie, zapewniając iż każdy    *
	 * pakiet przejdze pierścien K razy.                                         */
  while (1) {
  /* Odczyt w pętli z deskryptora:                               *
	 * Jest nie możliwe aby znalazł się tu pakiet konfiguracyjny,  *
	 * Za to może się znależć pakiet kończący - jest obługiwany    *
	 * tak samo jak zwykły pakiet z wyjątkiem wypisania na         *
	 * standardowe wyjście.                                        */
		READ(odczyt_dsk, &paczka, PAKIET_ROZ);
		if ((paczka.licznik == 0) && (strlen(paczka.tresc) != 0)) {
			printf("[%d] -> %s\n", gpid, paczka.tresc);
		} else {
			--paczka.licznik;
			srand((unsigned) (gpid + time(NULL) + licznik)); 
			licznik = (licznik + 17)%65535; /* zwiększa losowość */
			if ((strlen(paczka.tresc) != 0) && rand()%3 && (strlen(paczka.tresc) != (MAX - 1))) 
				strncat(paczka.tresc, "x", 1); /* niedopisujemy gdy brak miejsca */
			WRITE(zapis_dsk, &paczka, PAKIET_ROZ);
		}
	/* Jesli nadejdzie wiadomość o zakończeniu programu, niech okrąży ona        *
	 * pierścień K razy, przez ten czas wszystkie pakiety zostaną wypisane,      *
	 * gdyż pakiet kończący nie może wyprzedzić wcześniej umieszczonych.         *
	 * Gdyż jesli dogoni jakiś pakiet to i tak zostanie w deskryptorze zapisany  *
	 * na dalszej pozycji. Po K okrążeniach licznik będzie niedodatni. Procesy   *
	 * będą wtedy po kolei czekać na zakończenie działania potomków.             */
/*		printf("[%d] dlugość=%d licznik_obrotów=%d\n",gpid,strlen(paczka.tresc),paczka.licznik);*/
		if ((strlen(paczka.tresc) == 0)	&& (paczka.licznik < 0)) break; 
  }
	/* Ostatni nie czeka: */
	if (!czy_ostatni) {
		printf("[%d] Oczekiwanie na zamknęcie procesów potomnych...\n", gpid);
		WAIT(0); 
	}
  CLOSE(zapis_dsk);
  CLOSE(odczyt_dsk);
	printf("[%d] Zamykany proces...\n", gpid);
  exit (0);
}
