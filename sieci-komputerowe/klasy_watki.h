#ifndef _KLASY_WATKI_H_
#define _KLASY_WATKI_H_

/* STL */
#include <stack>
#include <set>

/* C */
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "klasy.h"
#include "watki.h"


class watki
{
	public:
		pthread_t watek_pol_inf[MAX_WTK_POL_INF];
		pthread_attr_t watek_attr_pol_inf[MAX_WTK_POL_INF];
		pthread_t watek_pol_pob[MAX_WTK_POL_POB];
		pthread_attr_t watek_attr_pol_pob[MAX_WTK_POL_POB];
		pthread_t watek_pob[MAX_WTK_POB];
		pthread_attr_t watek_attr_pob[MAX_WTK_POB];
		pthread_t watek_frg[MAX_WTK_FRG];
		pthread_attr_t watek_attr_frg[MAX_WTK_FRG];
		przesylka_pol_inf * p_pol_inf[MAX_WTK_POL_INF];
		przesylka_pol_pob * p_pol_pob[MAX_WTK_POL_POB];
		przesylka_pob * p_pob[MAX_WTK_POB];
		przesylka_frg * p_frg[MAX_WTK_FRG];
		set<int> zbior_pol_inf;
		set<int> zbior_pol_pob;
		set<int> zbior_pob;
		set<int> zbior_frg;
		stack<int> wolne_pol_inf;
		stack<int> wolne_pol_pob;
		stack<int> wolne_pob;
		stack<int> wolne_frg;
		pthread_mutex_t semafor_pol_inf;
		pthread_mutex_t semafor_pol_pob;
		pthread_mutex_t semafor_pob;
		pthread_mutex_t semafor_frg;
		
		watki() 
		{
			int i;
			pthread_mutex_init(&semafor_pol_inf, NULL);
			pthread_mutex_init(&semafor_pol_pob, NULL);
			pthread_mutex_init(&semafor_pob, NULL);
			pthread_mutex_init(&semafor_frg, NULL);
			for (i = 0; i < MAX_WTK_POL_INF; i++) 
			{
				pthread_attr_init(&watek_attr_pol_inf[i]);
				pthread_attr_setdetachstate(&watek_attr_pol_inf[i], PTHREAD_CREATE_JOINABLE); // DETACHED
				wolne_pol_inf.push(i);
				p_pol_inf[i] = (przesylka_pol_inf *) malloc(sizeof(przesylka_pol_inf));
			}
			for (i = 0; i < MAX_WTK_POL_POB; i++) 
			{
				pthread_attr_init(&watek_attr_pol_pob[i]);
				pthread_attr_setdetachstate(&watek_attr_pol_pob[i], PTHREAD_CREATE_JOINABLE); // DETACHED
				wolne_pol_pob.push(i);
				p_pol_pob[i] = (przesylka_pol_pob *) malloc(sizeof(przesylka_pol_pob));
			}
			for (i = 0; i < MAX_WTK_POB; i++) 
			{
				pthread_attr_init(&watek_attr_pob[i]);
				pthread_attr_setdetachstate(&watek_attr_pob[i], PTHREAD_CREATE_DETACHED);
				wolne_pob.push(i);
				p_pob[i] = (przesylka_pob *) malloc(sizeof(przesylka_pob));
			}
			for (i = 0; i < MAX_WTK_FRG; i++) 
			{
				pthread_attr_init(&watek_attr_frg[i]);
				pthread_attr_setdetachstate(&watek_attr_frg[i], PTHREAD_CREATE_DETACHED);
				wolne_frg.push(i);
				p_frg[i] = (przesylka_frg *) malloc(sizeof(przesylka_frg));
			}
		}
		~watki() 
		{
			int i;
			for (i = 0; i < MAX_WTK_POL_INF; i++) 
			{
				free(p_pol_inf[i]);
			}
			for (i = 0; i < MAX_WTK_POL_POB; i++) 
			{
				free(p_pol_pob[i]);
			}
			for (i = 0; i < MAX_WTK_POB; i++) 
			{
				free(p_pob[i]);
			}
			for (i = 0; i < MAX_WTK_FRG; i++) 
			{
				free(p_frg[i]);
			}
		}
		
		int daj_watek_pol_inf(Wszystko * Glb, slowo adr)
		{
			pthread_mutex_lock(&semafor_pol_inf);
			if (wolne_pol_inf.empty()) {
				printf("> Limit wątków połączeń!\n");
				return -1;
			}
			int w = wolne_pol_inf.top();
			zbior_pol_inf.insert(w);
			wolne_pol_inf.pop();
			p_pol_inf[w]->nr_watku = w;
			strcpy(p_pol_inf[w]->adres, adr);
			p_pol_inf[w]->Globalne = Glb;
			pthread_mutex_unlock(&semafor_pol_inf);
			if (pthread_create(&watek_pol_inf[w], &watek_attr_pol_inf[w], watek_polacz_inf, (void *) p_pol_inf[w]) != 0)//&ww,&wa,watek_polacz,(void *)0) != 0);//
				perror("> Bład w pthread_create!");
			return 0;
		}
		int daj_watek_pol_pob(Wszystko * Glb, slowo adr, int id_pob)
		{
			pthread_mutex_lock(&semafor_pol_pob);
			if (wolne_pol_pob.empty()) {
				printf("> Limit wątków połączeń!\n");
				return -1;
			}
			int w = wolne_pol_pob.top();
			zbior_pol_pob.insert(w);
			wolne_pol_pob.pop();
			p_pol_pob[w]->nr_watku = w;
			strcpy(p_pol_pob[w]->adres, adr);
			p_pol_pob[w]->Globalne = Glb;
			p_pol_pob[w]->id_pob = id_pob;
			pthread_mutex_unlock(&semafor_pol_pob);
			if (pthread_create(&watek_pol_pob[w], &watek_attr_pol_pob[w], watek_polacz_pob, (void *) p_pol_pob[w]) != 0)//&ww,&wa,watek_polacz,(void *)0) != 0);//
				perror("> Bład w pthread_create!");
			return 0;
		}
		int daj_watek_pob(Wszystko * Glb, slowo plk)
		{
			pthread_mutex_lock(&semafor_pob);
			if (wolne_pob.empty()) {
				printf("> Limit wątków pobierania!\n");
				return -1;
			}
			int w = wolne_pob.top();
			zbior_pob.insert(w);
			wolne_pob.pop();
			p_pob[w]->nr_watku = w;
			strcpy(p_pob[w]->plik, plk);
			p_pob[w]->Globalne = Glb;
			pthread_mutex_unlock(&semafor_pob);
			pthread_create(&watek_pob[w], &watek_attr_pob[w], watek_pobierz, (void *) p_pob[w]);
			return 0;
		}
		int daj_watek_frg(Wszystko * p, int id_p, slowo plk)
		{
			pthread_mutex_lock(&semafor_frg);
			if (wolne_frg.empty()) {
				printf("> Limit wątków wysyłania!\n");
				return -1;
			}
			int w = wolne_frg.top();
			zbior_frg.insert(w);
			wolne_frg.pop();
			p_frg[w]->nr_watku = w;
			p_frg[w]->id_pol = id_p;
			strcpy(p_frg[w]->plik, plk);
			p_frg[w]->Globalne = p;
			pthread_mutex_unlock(&semafor_frg);
			pthread_create(&watek_frg[w], &watek_attr_frg[w], watek_fragmenty, (void *) p_frg[w]);
			return 0;
		}
		void oddaj_watek_pol_inf(int w)
		{
			pthread_mutex_lock(&semafor_pol_inf);
			wolne_pol_inf.push(w);
			zbior_pol_inf.erase(w);
			pthread_mutex_unlock(&semafor_pol_inf);
		}
		void oddaj_watek_pol_pob(int w)
		{
			pthread_mutex_lock(&semafor_pol_pob);
			wolne_pol_pob.push(w);
			zbior_pol_pob.erase(w);
			pthread_mutex_unlock(&semafor_pol_pob);
		}
		void oddaj_watek_pob(int w)
		{
			pthread_mutex_lock(&semafor_pob);
			wolne_pob.push(w);
			zbior_pob.erase(w);
			pthread_mutex_unlock(&semafor_pob);
		}
		void oddaj_watek_frg(int w)
		{
			pthread_mutex_lock(&semafor_frg);
			wolne_frg.push(w);
			zbior_frg.erase(w);
			pthread_mutex_unlock(&semafor_frg);
		}
		void koniec()
		{
			
		}
};

#endif
