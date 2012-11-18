#ifndef _KLASY_PYTANIA_H_
#define _KLASY_PYTANIA_H_

/* STL */
#include <set>
#include <vector>

/* C */
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
#include <pthread.h>

#include "definicje.h"
#include "operatory.h"
#include "klasy.h"

using namespace std;


class zapytanie
{
	public:
		slowo nazwa_pliku;

		int id_pliku;
		in_addr zrodla_pliku[MAX_ZDL];
		off_t rozmiar_pliku;
		int liczba_zrodel;
		long liczba_fragmentow[MAX_ZDL];
		long fragmenty_start[MAX_ZDL][MAX_NRZ];
		long fragmenty_koniec[MAX_ZDL][MAX_NRZ];
		in_addr posrednicy[MAX_ZDL][MAX_GLB];
		
		zapytanie() 
		{
			int i, j;
			id_pliku = -1;
			strcpy(nazwa_pliku, "brak");
			liczba_zrodel = 0;
			for (i = 0; i < MAX_ZDL; i++) {
				liczba_fragmentow[i] = 0;
				for (j = 0; j < MAX_NRZ; j++) {
					fragmenty_start[i][j]	= -1;
					fragmenty_koniec[i][j]	= -1;
				}
			}
		}
		zapytanie(slowo s, int d)
		{
			ustaw(s,d);
		}
		~zapytanie() {}

		zapytanie & operator=(const zapytanie & z)
		{
			int i;
			strcpy(nazwa_pliku, z.nazwa_pliku);
			id_pliku = z.id_pliku;
			rozmiar_pliku = z.rozmiar_pliku;
			liczba_zrodel = z.liczba_zrodel;
			for (i = 0; i < MAX_ZDL; i++)
				zrodla_pliku[i] = z.zrodla_pliku[i];
			return *this;
		}
		bool operator==(const zapytanie & z)
		{
			return (strcmp(nazwa_pliku, z.nazwa_pliku) == 0);
		}
		bool operator!=(const zapytanie & z)
		{
			return (strcmp(nazwa_pliku, z.nazwa_pliku) != 0);
		}
		void ustaw(slowo np, int id) 
		{
			strcpy(nazwa_pliku, np);
			id_pliku = id;
			liczba_zrodel = 0;
		}
		void ustaw_zrodlo(in_addr a, off_t s)
		{
			zrodla_pliku[liczba_zrodel++] = a;
			rozmiar_pliku = s;
		}
		void ustaw_zrodlo(odp_pakiet * odp)
		{
			int i;
			zrodla_pliku[liczba_zrodel] = odp->nadawca;
			rozmiar_pliku = odp->rozmiar;
			liczba_fragmentow[liczba_zrodel] = odp->liczba_frg;
			for (i = 0; i < odp->liczba_frg; i++) {
				fragmenty_start[liczba_zrodel][i] = odp->frg_start[i];
				fragmenty_start[liczba_zrodel][i] = odp->frg_koniec[i];
			}
			for (i = 0; i < MAX_GLB; i++)
				posrednicy[liczba_zrodel][i] = odp->posrednicy[i];
			liczba_zrodel++;
		}
		void wypisz()
		{
			printf("> %d %s %d %d(%s)\n", id_pliku, nazwa_pliku, rozmiar_pliku, liczba_zrodel, (liczba_fragmentow == 0) ? "cały" : "fragment");
		}
		void wypisz_szczegoly()
		{
			int i, j;
			printf("> Wyniki dla %s\n", nazwa_pliku);
			for (i = 0; i < liczba_zrodel; i++) {
				printf("> %d: %s(%ld)\n", i, inet_ntoa(zrodla_pliku[i]), liczba_fragmentow[i]);
				if (liczba_fragmentow[i] > 0) {
					printf("> fragmenty:");
					for (j = 0; j < liczba_fragmentow[i]; j++)
						printf(" <%ld,%ld", fragmenty_start[i][j], fragmenty_koniec[i][j]);
					printf(">\n");
				}
			}
		}
};

inline bool operator<(const zapytanie & z1, const zapytanie & z2)
{
	return (strcmp(z1.nazwa_pliku, z2.nazwa_pliku) < 0);
}


class pytania 
{
	private:
		set<zapytanie> zbior_pyt;
		slowo tablica[MAX_PYT];
		int wolne_id;
		pthread_mutex_t semafor;
		
	public:	
		pytania() 
		{ 
			pthread_mutex_init(&semafor, NULL);
			wolne_id = 0;
		}
		~pytania() {}
		
		bool znajdz(int id)
		{
			return (id < wolne_id);
		}
		
		int dodaj(slowo np) // zwraca id !!
		{
			int tmp;
			pthread_mutex_lock(&semafor);
			set<zapytanie>::iterator it;
			zapytanie z(np, wolne_id);
			if ((it = zbior_pyt.find(z)) != zbior_pyt.end()) {
				zbior_pyt.erase(it);
			}
			zbior_pyt.insert(z);
			if (wolne_id == MAX_PYT - 1)
				tmp = wolne_id = 0;
			else
				tmp = wolne_id++;
			strcpy(tablica[tmp], np); // ?
			pthread_mutex_unlock(&semafor);
			return tmp;
		} 
		bool znajdz(slowo np)
		{
			zapytanie z(np, -1);
			return (zbior_pyt.find(z) != zbior_pyt.end());
		}
		int znajdz_id(slowo np)
		{
			set<zapytanie>::iterator it;
			zapytanie z(np, -1);
			if ((it = zbior_pyt.find(z)) == zbior_pyt.end())
				return -1;
			return it->id_pliku;
		}
		void wypisz()
		{
			set<zapytanie>::iterator it;
			printf("> Wyniki wyszukiwań:\n");
			for (it = zbior_pyt.begin(); it != zbior_pyt.end(); it++) {
				printf("> %d %s %d %d\n", it->id_pliku, it->nazwa_pliku, it->rozmiar_pliku, it->liczba_zrodel); // it->wypisz();
			}
		}
		void wypisz(int d) 
		{
			zapytanie z;
			zapytanie tmp(tablica[d], -1);
			z = *zbior_pyt.find(tmp);
			z.wypisz_szczegoly();
		}
		void ustaw_zrodlo(int d, in_addr z, off_t s)
		{
			pthread_mutex_lock(&semafor);
			zapytanie zz;
			zapytanie tmp(tablica[d], d);
			zz = *zbior_pyt.find(tmp);
			zbior_pyt.erase(tmp);
			zz.ustaw_zrodlo(z, s);
			zbior_pyt.insert(zz);
			pthread_mutex_unlock(&semafor);
		}
		void ustaw_zrodlo(odp_pakiet * odp)
		{
			pthread_mutex_lock(&semafor);
			zapytanie zz, tmp(odp->nazwa_pliku, odp->id);
			zz = *zbior_pyt.find(tmp);
			zbior_pyt.erase(tmp);
			zz.ustaw_zrodlo(odp);
			zbior_pyt.insert(zz);
			pthread_mutex_unlock(&semafor);
		}
		zapytanie eksport(slowo w)
		{
			zapytanie z(w, -1);
			return *zbior_pyt.find(z);
		}
};

#endif
