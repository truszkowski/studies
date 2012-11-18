#ifndef _KLASY_POBIERANIE_H_
#define _KLASY_POBIERANIE_H_

/* STL */
#include <stack>

/* C */
#include <sys/stat.h>
#include <fcntl.h>
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
#include <pthread.h>

#include "definicje.h"
#include "operatory.h"
#include "klasy.h"
#include "proc_pakiety.h"

using namespace std;


/* Klasy dotyczące pobierania plików */

class pobierany // pojedynczy plik
{
	public:
		int deskryptor_pliku;
		slowo nazwa_pliku;                 /* Nazwa pobieranego pliku                         */
		int id_pliku;                      /* Id pobieranego pliku                            */
		int liczba_zrodel;                 /* Liczba dostepnych zrodel                        */
		bool czy_zapisano;                 /* Oczekiwana ilosc fragmentow                     */
		off_t rozmiar_pliku; 
		long licznik_pliku_start;          /* Oczekiwana ilosc fragmentow                     */
		long licznik_pliku_koniec;         /* Oczekiwana ilosc fragmentow                     */
		fd_set zrodla_dsk;                 /* Deskryptory gniazd posiadaczy plików            */
		int zrodlo_dsk[MAX_NRZ];           /* Deskryptor gniazda do posiadacza pliku          */
		bool czy_gotowe;                   /* Aktywność połączenia                            */
		bool czy_aktywne[MAX_NRZ];         /* Aktywność połączenia                            */
		in_addr zrodla_pliku[MAX_NRZ];     /* Adresy zrodel pliku                             */
		long licznik_fragmentu[MAX_NRZ];   /* Do ktorego fragmentu pobieranie z danego zrodla */
		
		stack<pair<long,long> > stos_fragmentow; 
		long liczba_fragmentow[MAX_NRZ];
		long fragmenty_start[MAX_NRZ][MAX_NRZ];
		long fragmenty_koniec[MAX_NRZ][MAX_NRZ];
		
		in_addr sciezka_do_zrodla[MAX_NRZ][MAX_GLB];
		
		pthread_mutex_t semafor; 
		pthread_mutex_t semafor_zapis;
		
		pobierany() 
		{
			int i, j;
			pthread_mutex_init(&semafor, NULL);
			pthread_mutex_init(&semafor_zapis, NULL);
			strcpy(nazwa_pliku, "brak");
			deskryptor_pliku = -1;
			id_pliku = -1;
			liczba_zrodel = 0;
			czy_zapisano = false;
			rozmiar_pliku = 0;
			licznik_pliku_start = 0;
			licznik_pliku_koniec = 0;
			for (i = 0; i < MAX_NRZ; i++)
			{	
				czy_aktywne[i] = false;
				zero(zrodla_pliku[i]);
				licznik_fragmentu[i] = 0;
				liczba_fragmentow[i] = 0;
				for (j = 0; j < MAX_NRZ; j++) {
					fragmenty_start[i][j] = -1;
					fragmenty_koniec[i][j] = -1;
				}
				zrodlo_dsk[i] = -1;
			}
			czy_gotowe = false;
			FD_ZERO(&zrodla_dsk);
		}
		~pobierany() {}
		
		void ustaw(zapytanie * zapyt) //
		{
			int i, j;
			char sciezka[2 * MAX_SLW];
			pthread_mutex_lock(&semafor);
			strcpy(nazwa_pliku, zapyt->nazwa_pliku);
			id_pliku = zapyt->id_pliku;
			liczba_zrodel = zapyt->liczba_zrodel;
			rozmiar_pliku = zapyt->rozmiar_pliku;
			for (i = 0; i < MAX_NRZ; i++) {
				zrodla_pliku[i] = zapyt->zrodla_pliku[i];
				liczba_fragmentow[i] = zapyt->liczba_fragmentow[i];
				for (j = 0; j < MAX_NRZ; j++) {	
					fragmenty_start[i][j] = zapyt->fragmenty_start[i][j];
					fragmenty_koniec[i][j] = zapyt->fragmenty_koniec[i][j];
				}
				for (j = 0; j < MAX_GLB; j++) 
					sciezka_do_zrodla[i][j] = zapyt->posrednicy[i][j];
				
			}
			
			if ((zapyt->rozmiar_pliku % MAX_DANE) == 0) 
				licznik_pliku_koniec = zapyt->rozmiar_pliku / MAX_DANE;
			else 
				licznik_pliku_koniec = zapyt->rozmiar_pliku / MAX_DANE + 1;
			
			strcpy(sciezka , KATALOG_POB);
			strcat(sciezka , nazwa_pliku);
			if ((deskryptor_pliku = open(sciezka, O_CREAT | O_WRONLY, 00600)) == -1) // S_IRURS | S_IWURS
				printf("> Bład w open!\n");
			pthread_mutex_unlock(&semafor);
			
		}
		void dodaj_dsk(in_addr a, int dsk)
		{
			pthread_mutex_lock(&semafor);
			int i;
			for (i = 0; i < liczba_zrodel; i++)
				if (zrodla_pliku[i] == a) {
					czy_aktywne[i] = true;
					zrodlo_dsk[i] = dsk;
					break;
				}
			czy_gotowe = true;
			FD_SET(dsk, &zrodla_dsk);
			pthread_mutex_unlock(&semafor);
		}
		void usun_dsk()
		{
			/* usuwanie nieaktualnego polaczenia */
		}
		void daj_fragmenty(pob_pakiet * pob_pak, int id)
		{
			pthread_mutex_lock(&semafor);
			pob_pak->frg_start = licznik_pliku_start;
			if (licznik_pliku_koniec - licznik_pliku_start > MAX_PRC) {
				pob_pak->frg_koniec = (licznik_pliku_start + MAX_PRC);
				licznik_fragmentu[id] = pob_pak->frg_koniec - 1;
				licznik_pliku_start += MAX_PRC;
			} else {
				pob_pak->frg_koniec =  licznik_pliku_koniec; 
				licznik_fragmentu[id] = pob_pak->frg_koniec - 1;
				licznik_pliku_start = licznik_pliku_koniec; 
			}
			printf("> frg:%ld, %ld, %ld, %ld, %ld\n", pob_pak->frg_start, pob_pak->frg_koniec, licznik_fragmentu[id], licznik_pliku_start, licznik_pliku_koniec);
			pthread_mutex_unlock(&semafor);
		}
		void zapisz(frg_pakiet * frg_pak, int id_zrodla)
		{
			pthread_mutex_lock(&semafor_zapis);
			struct pob_pakiet pob_pak;
			if (lseek(deskryptor_pliku, frg_pak->nr_frg*MAX_DANE, SEEK_SET) == -1)
				printf("> Błąd w lseek!(%ld,%ld)\n", frg_pak->nr_frg, frg_pak->rozmiar );
			if (write(deskryptor_pliku, frg_pak->fragment, frg_pak->rozmiar) <= 0)
				printf("> Błąd w write!(%ld,%ld)\n", frg_pak->nr_frg, frg_pak->rozmiar);
			else
				if (frg_pak->nr_frg == licznik_pliku_koniec - 1)
					czy_zapisano = true;
			
			if (frg_pak->nr_frg == licznik_fragmentu[id_zrodla]) {
				daj_fragmenty(&pob_pak, id_zrodla);
				debuger("> Chcę: (%ld)(%ld)\n", pob_pak.frg_start, pob_pak.frg_koniec);
				if (write(zrodlo_dsk[id_zrodla], &pob_pak, R_POB_PAKIET) == -1)
					printf("> Błąd w write!\n");
			} 
			pthread_mutex_unlock(&semafor_zapis);			
		}
		void reset(int numer)
		{
			pthread_mutex_lock(&semafor);
			FD_CLR(zrodlo_dsk[numer], &zrodla_dsk);
			zrodlo_dsk[numer] = -1;
			czy_aktywne[numer] = false;
			pthread_mutex_unlock(&semafor);
		}
		void reset()
		{
			pthread_mutex_lock(&semafor);
			int i;
			strcpy(nazwa_pliku, "brak");
			deskryptor_pliku = -1;
			id_pliku = -1;
			liczba_zrodel = 0;
			czy_zapisano = false;
			licznik_pliku_start = 0;
			licznik_pliku_koniec = 0;
			for (i = 0; i < MAX_NRZ; i++)
			{	
				czy_aktywne[i] = false;
				zero(zrodla_pliku[i]);
				licznik_fragmentu[i] = 0;
				zrodlo_dsk[i] = -1;
			}
			czy_gotowe = false;
			FD_ZERO(&zrodla_dsk);
			pthread_mutex_unlock(&semafor);
		}
	
};



class pobieranie // caly zbiór plikow;
{
	public:
//		int pozycja[MAX_POB];
		pobierany plik[MAX_POB];
		stack<int> wolne;
		set<int> zajete;
		pthread_mutex_t semafor;
		
		pobieranie() 
		{
			int i;
			pthread_mutex_init(&semafor, NULL);
			for (i = 0; i < MAX_POB; i++)
				wolne.push(i);
		}
		~pobieranie() {}
		
		int import(zapytanie * zapyt)
		{
			pthread_mutex_lock(&semafor);
			if (wolne.empty())
				return -1;
			
			int tmp = wolne.top();
			wolne.pop();
			zajete.insert(tmp);
			plik[tmp].ustaw(zapyt);
			pthread_mutex_unlock(&semafor);
			return tmp;
		}
		
		void dodaj_kanal_pob(int id, in_addr a, int dsk)
		{
			pthread_mutex_lock(&semafor);
			plik[id].dodaj_dsk(a,dsk);
			pthread_mutex_unlock(&semafor);
		}
		void reset(int id)
		{
			pthread_mutex_lock(&semafor);
			plik[id].reset();
			zajete.erase(id);
			wolne.push(id);
			pthread_mutex_unlock(&semafor);
		}
//		void usun(int id)
//		{
//			/* usuwanie nieaktualnego polaczenia */
//		}
		/*
		void dodaj(slowo pl, int id) 
		{
			int i;
			pobierany p(pl, id);
			for (i = 0; i < MAX_POB; i++)
				if (pozycja[i] = -1) {
					pozycja[i] = id;
					plik[i] = &p;
					break;
				}		
		}
		void czysc(int id)
		{
			int i;
			for (i = 0; i < MAX_POB; i++)
				if (i == id) 
					{ pozycja[i] = -1; break; }
		}
		void dodaj_zrodlo(int id, in_addr zrodlo)
		{
			int i;
			for (i = 0; i < MAX_POB; i++)
				if (pozycja[i] == id) {
					plik[i]->dodaj_zrodlo(zrodlo);
					break;
				}
		}	*/
};

#endif
