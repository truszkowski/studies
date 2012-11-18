#ifndef _KLASY_UDOSTEPNIANE_H_
#define _KLASY_UDOSTEPNIANE_H_

#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>

#include "definicje.h"
#include "operatory.h"
#include "klasy.h"

using namespace std;


/* Klasa udostepnianych plików */

class udostepniane
{
	public:
		slowo nazwa_pliku[MAX_UDS];
		slowo nazwa_pliku_frg[MAX_UDS];
		off_t rozmiar_pliku[MAX_UDS];
		off_t rozmiar_pliku_frg[MAX_UDS];
		long fragmenty_start[MAX_UDS][MAX_NRZ];
		long fragmenty_koniec[MAX_UDS][MAX_NRZ];
		int rekord;
		int rekord_frg;
		bool czy_pobrany[MAX_UDS];
		pthread_mutex_t semafor;
		// mozna na mapie stlowej;
		
		udostepniane()
		{
			int i,j;
			DIR * katalog;
			dirent * pozycja;
			struct stat s;
			rekord = 0;
			rekord_frg = 0;
			katalog = opendir(KATALOG_UDS);
			while (((pozycja = readdir(katalog)) != NULL) && (rekord < MAX_UDS))
			{
				char ttt[2 * MAX_SLW];
				strcpy(ttt, KATALOG_UDS);
				strcpy(nazwa_pliku[rekord], pozycja->d_name);
				strcat(ttt, nazwa_pliku[rekord]);
				if (stat(ttt, &s) == -1)
					printf("> Błąd w stat(%s)!\n", pozycja->d_name);
				rozmiar_pliku[rekord++] = s.st_size;
			}
			for (i = 0; i < MAX_UDS; i++) {
				czy_pobrany[i] = false;
				for (j = 0; j < MAX_NRZ; j++)
					{ fragmenty_start[i][j] = -1; fragmenty_koniec[i][j] = -1; }
			}
			closedir(katalog);
			pthread_mutex_init(&semafor, NULL);
		}
		bool czy_udostepniam(slowo plik)
		{
			pthread_mutex_lock(&semafor);
			int i, tmp = 0;

			for (i = 0; i < rekord; i++)
				if (strcmp(plik, nazwa_pliku[i]) == 0)
					{ tmp = 1; pthread_mutex_unlock(&semafor); return true; }

			pthread_mutex_unlock(&semafor);
			return false;
		}
		bool czy_udostepniam_frg(slowo plik)
		{
			pthread_mutex_lock(&semafor);
			int i, tmp = 0;
		
			for (i = 0; i < rekord_frg; i++)
				if (strcmp(plik, nazwa_pliku_frg[i]) == 0)
					{ tmp = 1; pthread_mutex_unlock(&semafor); return true; }
		
			pthread_mutex_unlock(&semafor);
			return false;
		}
		void dodaj_fragment(slowo plik, off_t r)
		{
			pthread_mutex_lock(&semafor);
			strcpy(nazwa_pliku_frg[rekord_frg], plik);
			rozmiar_pliku_frg[rekord_frg] = r;
			rekord_frg++;
			pthread_mutex_unlock(&semafor);	
		}
		void dodaj_fragment(long nrfrg, slowo plik)
		{
			pthread_mutex_lock(&semafor);
			int i, j, id;
			
			for (i = 0; i < MAX_UDS; i++)
				if (strcmp(plik, nazwa_pliku_frg[i]) == 0)
					{ id = i; break; }
			
			for (i = 0; i < MAX_NRZ; i++)
				if (fragmenty_start[id][i] == -1) {
					fragmenty_start[id][i] = nrfrg;
					fragmenty_koniec[id][i] = nrfrg;
					break;
				} else if (fragmenty_koniec[id][i] == nrfrg + 1) {
					fragmenty_koniec[id][i]++; 
					break; 
				} else if (fragmenty_koniec[id][i] < nrfrg + 1) {
					if (i != MAX_NRZ - 1) {
						fragmenty_start[id][i + 1] = nrfrg; 
						fragmenty_koniec[id][i + 1] = nrfrg; 
					} else
						printf("> Błąd w zapisie fragmentów!\n");
					break;
				}
				
			for (i = 1; i < MAX_NRZ; i++)
				if (fragmenty_koniec[id][i - 1] == fragmenty_start[id][i] - 1) {
					fragmenty_koniec[id][i - 1] = fragmenty_koniec[id][i];
					fragmenty_start[id][i] = -1;
					fragmenty_koniec[id][i] = -1;
					if (i != MAX_NRZ - 1)
						for (j = i + 1; j < MAX_NRZ; j++) {
							if (fragmenty_start[id][j] == -1)
								break;
							fragmenty_start[id][j - 1] = fragmenty_start[id][j];
							fragmenty_start[id][j - 1] = fragmenty_start[id][j];
							fragmenty_start[id][j] = -1;
							fragmenty_start[id][j] = -1;
						}
							
					break;
				} 
			pthread_mutex_unlock(&semafor);
		}
		void pobrany(slowo argument)
		{
			pthread_mutex_lock(&semafor);
			int i;
			for (i = 0; i < MAX_UDS; i++)
				if (strcmp(argument, nazwa_pliku_frg[i]) == 0)
					czy_pobrany[i] = true;
			pthread_mutex_unlock(&semafor);		
		}	
		int znajdz_frg(slowo s)
		{
			int i;
			for (i = 0; i < MAX_UDS; i++)
				if (strcmp(s, nazwa_pliku_frg[i]) == 0)
					return i;
			return -1;
		}
		off_t rozmiar(slowo c)
		{
//			pthread_mutex_lock(&semafor);
			struct stat s;
			char ttt[2 * MAX_SLW];
			strcpy(ttt, KATALOG_UDS);
			strcat(ttt, c);
			if (stat(ttt,&s) == -1) 
				printf("> Błąd w stat(%s)!\n", c);
			return s.st_size;
//			pthread_mutex_unlock(&semafor);
		}
		off_t rozmiar_frg(slowo c)
		{
//			pthread_mutex_lock(&semafor);
			int i;
			for (i = 0; i < MAX_UDS; i++)
				if (strcmp(c, nazwa_pliku_frg[i]) == 0)
					return rozmiar_pliku_frg[i];
			return -1;
//			pthread_mutex_unlock(&semafor);
		}
		void wypisz() 
		{
			int i;
			printf("> Udostępiane pliki: \n");
			for (i = 0; i < rekord; i++)
				printf("> %s %ldB\n", nazwa_pliku[i], (long) rozmiar_pliku[i]);
		}
		void pobrane()
		{
			int i,j;
			printf("> Udostępiane pliki: \n");
			for (i = 0; i < rekord_frg; i++) {
				printf("> %s %s\n", nazwa_pliku_frg[i], (czy_pobrany[i]) ? "pobrany" : "fragment");
				if (!czy_pobrany[i]) {
					for (j = 0; j < MAX_NRZ; j++)
						if (fragmenty_koniec[i][j] != -1)
							printf(">  <%ld,%ld", fragmenty_start[i][j], fragmenty_koniec[i][j]);
						else 
							break;
					printf("> \n");
				}
			}
		}
		
};

#endif
