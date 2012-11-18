#include "klasy.h"
#include "operatory.h"


void * watek_pobierz(void * dane)
{
	
	int i, w;
	slowo argument;
	przesylka_pob * p;
	Wszystko * Glb;
	zapytanie dane_pliku;
	int id_pob;
	fd_set zbior_dsk;	
	frg_pakiet frg_pak;
	timeval czas;

	p = (przesylka_pob *) dane;
	w = p->nr_watku;
	strcpy(argument, p->plik);
	Glb = p->Globalne;
	
	if (p->Globalne->pyt->znajdz(argument)) {
		dane_pliku = p->Globalne->pyt->eksport(argument);
		id_pob = p->Globalne->pob->import(&dane_pliku); // Od teraz posługuje sie tylko id_pob - nie dane_pliku;
		if (p->Globalne->pob->plik[id_pob].liczba_zrodel > 0) {
			for (i = 0; i < p->Globalne->pob->plik[id_pob].liczba_zrodel; i++)
				p->Globalne->wtk->daj_watek_pol_pob(p->Globalne, inet_ntoa(p->Globalne->pob->plik[id_pob].zrodla_pliku[i]), id_pob);
			
			p->Globalne->uds->dodaj_fragment(argument, p->Globalne->pob->plik[id_pob].rozmiar_pliku);
			while (!p->Globalne->pob->plik[id_pob].czy_gotowe) 
				sleep(1);
			int l = 0, licznik_przerwan = 0;
			bool ktos_pobiera;
			while (!p->Globalne->pob->plik[id_pob].czy_zapisano)
			{
				czas.tv_sec = 2;
				czas.tv_usec = 0;
				
				pthread_mutex_lock(&p->Globalne->pob->plik[id_pob].semafor);
				zbior_dsk = p->Globalne->pob->plik[id_pob].zrodla_dsk;
				pthread_mutex_unlock(&p->Globalne->pob->plik[id_pob].semafor);
				
				ktos_pobiera = false;
				
				select(FD_SETSIZE, &zbior_dsk, NULL, NULL, &czas); 
				
				for (i = 0; i < MAX_NRZ; i++)
					if (p->Globalne->pob->plik[id_pob].czy_aktywne[i]) {
						if (FD_ISSET(p->Globalne->pob->plik[id_pob].zrodlo_dsk[i], &zbior_dsk)) {
							switch (read_while(p->Globalne->pob->plik[id_pob].zrodlo_dsk[i], &frg_pak, R_FRG_PAKIET))
							{
								case -1:
									printf("> Błąd w read_while!\n");
									p->Globalne->pol->reset_kanal(p->Globalne->pol->dsk2rek[p->Globalne->pob->plik[id_pob].zrodlo_dsk[i]]);
									p->Globalne->pob->plik[id_pob].reset(i);
									break;
									
								case 0:
									printf("> Koniec pobierania...\n");
									p->Globalne->pol->reset_kanal(p->Globalne->pol->dsk2rek[p->Globalne->pob->plik[id_pob].zrodlo_dsk[i]]);
									p->Globalne->pob->plik[id_pob].reset(i);
									break;
								
								default:
									printf("> (%ld) (%ld) (%ld)\n", frg_pak.nr_frg, frg_pak.rozmiar, p->Globalne->pob->plik[id_pob].licznik_pliku_koniec);
									p->Globalne->pob->plik[id_pob].zapisz(&frg_pak, i);
									p->Globalne->uds->dodaj_fragment(frg_pak.nr_frg, argument);
							}
						}
						ktos_pobiera = true;
						licznik_przerwan = 0;
					}
				/* Jesli wszystkie polaczenia sa zerwane a nie sciagnelismy pliku to laczymy sie jeszcze raz... */
				if (!ktos_pobiera) {
					licznik_przerwan++;
					if (p->Globalne->pob->plik[id_pob].liczba_zrodel > 0) 
						for (i = 0; i < p->Globalne->pob->plik[id_pob].liczba_zrodel; i++)
							p->Globalne->wtk->daj_watek_pol_pob(p->Globalne, inet_ntoa(p->Globalne->pob->plik[id_pob].zrodla_pliku[i]), id_pob);
					if (licznik_przerwan == MAX_PRZERWAN) {
						printf("> Przekroczono limit prób połączeń, niepobrano pliku:(\n");
						break;
					}
				}
			}
			if (licznik_przerwan < MAX_PRZERWAN) {
				printf("> Plik ściągnięty :)\n");
				for (i = 0; i < MAX_NRZ; i++)
					if (p->Globalne->pob->plik[id_pob].czy_aktywne[i]) 
						p->Globalne->pol->reset_kanal(p->Globalne->pol->dsk2rek[p->Globalne->pob->plik[id_pob].zrodlo_dsk[i]]);
				p->Globalne->uds->pobrany(argument);
			}
			p->Globalne->pob->reset(id_pob);
		} else 
			printf("> Brak źródeł pliku %s\n", argument);
	} else 
		printf("> Brak danych o %s\n", argument);

	

	p->Globalne->wtk->oddaj_watek_pob(w);
	return dane;
}

