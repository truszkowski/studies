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
#include "proc_pakiety.h"

using namespace std;

/* SOCKET_PYT */

void ustaw(pyt_pakiet * pyt_pak, in_addr n, slowo plik, int idp) 
{
	strcpy(pyt_pak->nazwa_pliku, plik);
	pyt_pak->id = idp;
	memcpy(&pyt_pak->nadawca, &n, sizeof(in_addr));
	memcpy(&pyt_pak->posrednicy[0], &n, sizeof(in_addr));
	pyt_pak->glebokosc = 0;
}

void nastepny(pyt_pakiet * pyt_pak, in_addr adres) 
{
	pyt_pak->glebokosc++; 
	memcpy(&pyt_pak->posrednicy[pyt_pak->glebokosc], &adres, sizeof(in_addr));
}

/* SOCKET_ODP */

void nastepny(odp_pakiet * odp_pak) 
{
	odp_pak->glebokosc--;
}

void pyt_pakiet2odp_pakiet(pyt_pakiet * pyt_pak, odp_pakiet * odp_pak, udostepniane * uds, in_addr adres) //void * v
{
	int i;
	odp_pak->glebokosc = pyt_pak->glebokosc - 1;
	odp_pak->id = pyt_pak->id;
	strcpy(odp_pak->nazwa_pliku, pyt_pak->nazwa_pliku);
	odp_pak->nadawca = adres;
	odp_pak->liczba_frg = 0;
	for (i = 0; i < MAX_GLB; i++)
		memcpy(&odp_pak->posrednicy[i], &pyt_pak->posrednicy[i], sizeof(in_addr));
	if ((uds->czy_udostepniam_frg(pyt_pak->nazwa_pliku)) && (!uds->czy_udostepniam(pyt_pak->nazwa_pliku))) {
		printf(">> ");
		int f = uds->znajdz_frg(pyt_pak->nazwa_pliku);
		for (i = 0; i < MAX_NRZ; i++) {
			printf(" %ld - %ld; ", uds->fragmenty_start[f][i], uds->fragmenty_koniec[f][i]);
			odp_pak->frg_start[i] = uds->fragmenty_start[f][i];
			odp_pak->frg_koniec[i] = uds->fragmenty_start[f][i];
			if (odp_pak->frg_start[i] != -1)
				odp_pak->liczba_frg++;
		}
		printf("\n");
		odp_pak->rozmiar = uds->rozmiar_pliku_frg[f];
	} else 
		odp_pak->rozmiar = uds->rozmiar(pyt_pak->nazwa_pliku);
} 

/* SOCKET_POB */

void ustaw(pob_pakiet * pob_pak, in_addr adres, slowo plik, int idp, long start, long koniec) 
{
	
}

/* SOCKET_FRG */

void ustaw(frg_pakiet * frg_pak, long nrfrg, long r, dane f) 
{
	frg_pak->nr_frg = nrfrg;
	frg_pak->rozmiar = r;
}

/* SOCKET_KMN */

void ustaw(kmn_pakiet * kmn_pak, void * v, in_addr nip, in_addr aip) 
{
	int i, nr_zdl;
	pobierany * plik = (pobierany *) v;
	kmn_pak->nadawca = nip;
	kmn_pak->adresat = aip;
	strcpy(kmn_pak->nazwa_pliku, plik->nazwa_pliku);
	kmn_pak->id_pliku = plik->id_pliku;
	kmn_pak->glebokosc = 1; //
	for (i = 0; i < MAX_NRZ; i++)
		if (plik->zrodla_pliku[i] == aip)
			nr_zdl = i;
	for (i = 0; i < MAX_GLB; i++)
		kmn_pak->posrednicy[i] = plik->sciezka_do_zrodla[nr_zdl][i];
}

void nastepny(kmn_pakiet * kmn_pak)
{
	kmn_pak->glebokosc++;
}

void kmn_pakiet2brk_pakiet(kmn_pakiet * kmn_pak, brk_pakiet * brk_pak)
{
	int i;
	brk_pak->nadawca = kmn_pak->nadawca;
	brk_pak->adresat = kmn_pak->adresat;
	strcpy(brk_pak->nazwa_pliku, kmn_pak->nazwa_pliku);
	brk_pak->id_pliku = kmn_pak->id_pliku;
	brk_pak->glebokosc = kmn_pak->glebokosc - 1;
	for (i = 0; i < MAX_GLB; i++)
		brk_pak->posrednicy[i] = kmn_pak->posrednicy[i];
}

/* SOCKET_BRK */

void nastepny(brk_pakiet * brk_pak)
{
	brk_pak->glebokosc--;
}

/* SOCKET_SAS_PYT */

void ustaw(sas_pyt_pakiet * sas_pyt)
{
	/* Nic... */
}

/* SOCKET_SAS_ODP */

void ustaw(sas_odp_pakiet * sas_odp, polaczenia * pol)
{
	it_inf it;
	int i = 0;
	sas_odp->liczba = pol->zbior_inf.size();
	for (it = pol->iterator_inf(); it != pol->it_inf_koniec(); it++, i++)
		sas_odp->lista[i] = pol->kanaly[*it].adr;
}
