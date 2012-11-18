/* Piotr Truszkowski */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "err.h"
#include "sem.h"

#define SEMK ((key_t) 1235)
#define SHMK ((key_t) 5431)
#define MAX 20


int main(int argc, char * argv[])
{

	int sem, shm, N, R, blad, i, j, k, tmp, nrMacierzy, nrM1, nrM2, iluZagonic;
	SemBin dostep;
	SemUgl praca;
	FILE *strumien_dane, *strumien_wynik;
	char odpowiedz, linia[MAX];

	/* ,,Zmienne dzielone'' */
	int *wykladnik;     /* sizeof(int) */
	int *rozmiar;       /* sizeof(int) */
	int *minimum;       /* sizeof(int) */
	int *maxMinimum;    /* sizeof(int) */
	int *tabMinimum;    /* sizeof(int)*WYKL */
	int ***tabMacierzy; /* sizeof(int**)*WYKL */
	int *czynnik;          /* sizeof(int)*WYKL */
	/* razem potrzeba: */
	char *dane;         /* suma jw. + sizeof(int*)*WYKL*ROZM + sizeof(int)*WYKL*ROZM*ROZM  */
	
	if (argc != 5)
		fatal("> Użycie: %s <plik_z_danymi> <plik_z_wynikiem> <wykładnik> <robotnicy>", argv[0]);
		
	/* Wczytanie... */
	N = atoi(argv[3]);
	R = atoi(argv[4]);

	if (N <= 1)
		fatal("> Dla takich danych niemam czego liczyc!");
	if (R <= 0)
		fatal("> Niema kto pracować...");

	/* Otworzenie plików... */
	
	/* Odczyt z pliku... */
	if ((strumien_dane = fopen(argv[1], "r")) == NULL) {
		if (errno == EACCES)
			fatal("> Brak dostępu do pliku z danymi!");
		else 
			syserr("> Błąd w fopen!");
	}
	
	/* Zapis do pliku... */
	if ((blad = open(argv[2], O_CREAT | O_EXCL | O_WRONLY, 00600)) == -1) {
		if (errno == EEXIST) {
			printf("> Podany plik wynikowy już istnieje! (Nadpisz,Dopisz,Przerwij)/(N,D,P)");
			scanf("%c", &odpowiedz);
			switch (odpowiedz)
			{
				case 'N':
					if ((strumien_wynik = fopen(argv[2], "w")) == NULL)
						syserr("> Błąd w fopen!");
					break;

				case 'D':
					if ((strumien_wynik = fopen(argv[2], "a")) == NULL)
						syserr("> Błąd w fopen!");
					break;
				
				default:
					printf("> Kończe działanie programu...\n");
					if (fclose(strumien_dane))
						syserr("> Błąd w close!");
					return 0;
			}
		} else if (errno == EACCES)
			fatal("> Brak dostępu do pliku wynikowego!");
		else
			syserr("> Błąd w open!");	
	} else {
		if (close(blad) == -1)
			syserr("> Błąd w close!");
		if ((strumien_wynik = fopen(argv[2], "w")) == NULL)
			syserr("> Błąd w fopen!");
	}
	/* Wczytanie z pliku zadania */
	fscanf(strumien_dane, "%s", linia); /* <start */
	fscanf(strumien_dane, "%s", linia); /* of     */
	fscanf(strumien_dane, "%s", linia); /* file>  */
	fscanf(strumien_dane, "%d", &tmp); 
	/* c.d.n...*/

		/* Inicjowanie pamięci dzielonej */
		if ((shm = shmget(SHMK, (sizeof(int)*(N*tmp*tmp + 2*N + 4) + sizeof(int*)*N*tmp + sizeof(int**)*N), 0645 | IPC_CREAT | IPC_EXCL)) < 0)
			syserr("> Błąd w shmget!");
		if ((dane = (char *) shmat(shm, (char *) 0, 0)) == (char *) -1)
			syserr("> Błąd w shmat!");

	/* c.d... */	
	minimum = (int *) (dane);
	maxMinimum = (int *) (dane + sizeof(int));
	wykladnik = (int *) (dane + sizeof(int)*2);
	rozmiar = (int *) (dane + sizeof(int)*3);
	czynnik = (int *) (dane + sizeof(int)*4);
	tabMinimum = (int *) (dane + sizeof(int)*(4 + N*1));
	tabMacierzy = (int ***) (dane + sizeof(int)*(4 + N*2));
	for (i = 0; i < N; i++)
		tabMacierzy[i] = (int **) (dane + sizeof(int)*(4 + N*2) + sizeof(int**)*N + sizeof(int*)*tmp*i);
	for (i = 0; i < N; i++)
		for(j = 0; j < tmp; j++)
			tabMacierzy[i][j] = (int *) (dane + sizeof(int)*(4 + N*2) + sizeof(int**)*N + sizeof(int*)*N*tmp + sizeof(int)*(i*tmp*tmp + j*tmp));
	
	*minimum = 1;
	*maxMinimum = 1;
	*wykladnik = N;
	*rozmiar = tmp;
	czynnik[0] = 0;
	tabMinimum[0] = 0;
	czynnik[1] = 0;
	tabMinimum[1] = 1;
	for (i = 2; i < *wykladnik; i++) {
		czynnik[i] = -1;
		tabMinimum[i] = -1;
	}

	for (i = 0; i < *rozmiar; i++) 
		for (j = 0; j < *rozmiar; j++) 
			fscanf(strumien_dane, "%d", &tabMacierzy[0][i][j]); 

	for (i = 1; i < *wykladnik; i++)
		for (j = 0; j < *rozmiar; j++)
			for (k = 0; k < *rozmiar; k++) 
				tabMacierzy[i][j][k] = 0;
	
	if (fclose(strumien_dane) == -1)
		syserr("> Bład w fclose!");

	/* Inicjowanie semaforów */
	if ((sem = semget(SEMK, 2, IPC_CREAT | 0600)) == -1)
		syserr("> Błąd w semget!");
	dostep.id_sem = sem;
	dostep.nr_sem = 0;
	praca.id_sem = sem;
	praca.nr_sem = 1;

	tmp = -1;

/* Schodza sie robotnicy... */

	for (i = 0; i < R; i++)
		switch (fork())
		{
			case -1:
				syserr("> Błąd w fork!");
				break;
			
			case 0: /* Robotnik */
				
				/* Cieżka praca... */
				while (1)
				{
				
					iluZagonic = 0;					
					
					printf("%d> Zaczynam pracę...\n", getpid());
					
					PU(&praca, 1);
					printf("%d> Mam zlecenie...\n", getpid());
					PB(&dostep);

					printf("%d> Jestem w sekcji krytycznej!\n", getpid());

					
					/* czy kończyć... */
					if (((*minimum) + 1) > (*wykladnik)) {
						VU(&praca, 1); /* 1 -- aby inni też skończyli ... */
						VB(&dostep); /* ... i mieli dostęp */
						printf("%d> Opuściłem sekcję krytyczną.\n%d> Kończe pracę, niema co liczyć...\n", getpid(), getpid());
						break; /* wyjście z pętli while */
					}
					
					/* Abyśmy potem nie korzystali ze zmiennych które mogą być w między czasie modyfikowane */
					
					tmp = *minimum;
					nrMacierzy = tabMinimum[*minimum];
					nrM1 = czynnik[nrMacierzy];
					nrM2 = nrMacierzy - czynnik[nrMacierzy] - 1; 
					
					*minimum = *minimum + 1;

					/* Jeśli wykonujemy ostatnie mnożenie to zostawiamy sobie ,,furtkę'' na potem */
					if (*minimum >= *wykladnik) {
						printf("%d> Otwieram furtke!\n", getpid());
						VU(&praca, 1);
					} else
						printf("%d> pozycja: %d\n", getpid(), *minimum);
					VB(&dostep);
					
					printf("%d> Opuściłem sekcję krytyczną.\n", getpid());
					printf("%d> Mnożę macierze: A^%d * A^%d = A^%d\n", getpid(), nrM1 + 1, nrM2 + 1, nrMacierzy + 1);
					
					for (i = 0; i < *rozmiar; i++) 
						for (j = 0; j < *rozmiar; j++) 
							for (k = 0; k < *rozmiar; k++) 
								tabMacierzy[nrMacierzy][i][j] += tabMacierzy[nrM1][i][k] * tabMacierzy[nrM2][k][j];
					
					printf("%d> Zlecenie wykonane!\n", getpid());
					
					PB(&dostep);
				
					/* nowe minima... */
					for (i = 0; i <= tmp; i++) 
							if ((czynnik[tabMinimum[tmp] + tabMinimum[i] + 1]) == -1) {
								czynnik[tabMinimum[tmp] + tabMinimum[i] + 1] = tabMinimum[tmp];
								*maxMinimum = *maxMinimum + 1;
								tabMinimum[*maxMinimum] = tabMinimum[i] + tabMinimum[tmp] + 1;
								iluZagonic++;
								printf("%d> Można będzie liczyć macierz o wykładniku: %d!\n", getpid(), tabMinimum[*maxMinimum] + 1);
							}
					
					printf("%d> Liczba nowych macierzy które będzie można liczyć: %d\n", getpid(), iluZagonic);
					
					VB(&dostep);					
					VU(&praca, iluZagonic); 
					
				}
				
				printf("%d> Idę do domu...\n", getpid());
				
				return 0; /* Koniec pracy robotnika */
			
			default: /* Proces macierzysty */
				printf("> Utworzono proces robotnik!\n");
		}
	
	printf("> Dajemy zlecenia...\n");
	
	VB(&dostep);
	VU(&praca, 1);

	/* Oczekiwanie na zakończenie pracy robotników */
	for (i = 0; i < R; i++)
		if (wait(0) == -1)
			syserr("> Błąd w wait!");

	/* Zapis wyniku do pliku */
	fprintf(strumien_wynik, "<start of file>\n");
	for (i = 0; i < *wykladnik; i++) {
		fprintf(strumien_wynik, "WYKLADNIK: %d\n", i + 1);
		for (j = 0; j < *rozmiar; j++) {
			for (k = 0; k < *rozmiar; k++)
				fprintf(strumien_wynik, "%d ", tabMacierzy[i][j][k]);
			fprintf(strumien_wynik, "\n");
		}
	}
	fprintf(strumien_wynik, "<end of file>");
		
	/* Czyszczenie... */
	if (semctl(sem, 0, IPC_RMID) == -1)
		syserr("> Błąd w shmctl!");
	if (shmdt(dane) < 0)
		syserr("> Błąd w shmdt!");
	if (shmctl(shm, IPC_RMID, 0) < 0)
		syserr("> Błąd w shmctl!");
	
	return 0;
}
