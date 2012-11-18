#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "definicje.h"
#include "operatory.h"
#include "klasy.h"
#include "proc_pakiety.h"
#include "proc_serwer.h"

using namespace std;

//extern Wszystko Globalne_Dane; //...

/* Obsługa pakietu SOCKET_PYT */
void pytanie(pyt_pakiet * pyt_pak, pytania * pyt, odpowiedzi * odp, udostepniane * uds, polaczenia * desk) 
{
	int i, tmp = 1, kanal;
	socket_pakiet typ_pak;
	odp_pakiet odp_pak;
	it_inf iter_inf;
	bool czy_odpowiadalem;

	debuger("> Zadano pytanie!(%s)\n", pyt_pak->nazwa_pliku);

	/* sprawdzenie na liście pośredników */
	for (i = 0; i < pyt_pak->glebokosc; i++)
		if (desk->ip == pyt_pak->posrednicy[i])
			{ tmp = 0; break; }

	/* sprawdzenie czy plik nie byl juz szukany przez tego samego uzytkownika (z tym samym id) */
	if (czy_odpowiadalem = odp->znajdz(pyt_pak->nazwa_pliku, pyt_pak->nadawca, pyt_pak->id)) 
		tmp = 0;
	
	nastepny(pyt_pak, desk->ip);

	/* czy mozna przesylac dalej pytanie */
	if ((pyt_pak->glebokosc < MAX_GLB - 1) && (tmp != 0) && (desk->ip != pyt_pak->nadawca)) { // glebokosc jest numerowana od 0!!
		typ_pak.typ = SOCKET_PYT;
		/* przesyłanie dalej pytania*/
		for (iter_inf = desk->iterator_inf(); iter_inf != desk->it_inf_koniec(); iter_inf++) 
			if (write(desk->kanaly[*iter_inf].dsk, &typ_pak, R_SOCKET_PAKIET) <= 0) 
				printf("> Błąd w write!\n");
			else if (write(desk->kanaly[*iter_inf].dsk, pyt_pak, R_PYT_PAKIET) <= 0) 
				printf("> Błąd w write!\n");
	}	

	/* czy mam plik o ktory mnie pytaja */
	if (((uds->czy_udostepniam(pyt_pak->nazwa_pliku)) || (uds->czy_udostepniam_frg(pyt_pak->nazwa_pliku))) && (desk->ip !=  pyt_pak->nadawca) && (!czy_odpowiadalem)) { 
		debuger("> Wysyłam odpowiedz!\n");
		pyt_pakiet2odp_pakiet(pyt_pak, &odp_pak, uds, desk->ip);
		typ_pak.typ = SOCKET_ODP;
		for (iter_inf = desk->iterator_inf(); iter_inf != desk->it_inf_koniec(); iter_inf++) 
			if (desk->kanaly[*iter_inf].adr == odp_pak.posrednicy[odp_pak.glebokosc]) {		
				if (write(desk->kanaly[*iter_inf].dsk, &typ_pak, R_SOCKET_PAKIET) <= 0)
					printf("> Błąd w write!\n");
				else if (write(desk->kanaly[*iter_inf].dsk, &odp_pak, R_ODP_PAKIET) <= 0)
					printf("> Błąd w write!\n");
				break;
			}
	}
	odp->dodaj(pyt_pak->nazwa_pliku, pyt_pak->nadawca, pyt_pak->id);
}
	
/* Obsługa pakietu SOCKET_ODP */
void odpowiedz(odp_pakiet * odp_pak, pytania * pyt, odpowiedzi * odp, polaczenia * desk)
{
	int i, kanal;
	socket_pakiet typ_pak;
	typ_pak.typ = SOCKET_ODP;
	it_inf iter_inf;

	printf("> Odpowiedź: %s %s\n", inet_ntoa(odp_pak->nadawca), inet_ntoa(odp_pak->posrednicy[0]));
	if (odp_pak->glebokosc != 0) {
		nastepny(odp_pak); 
		for (iter_inf = desk->iterator_inf(); iter_inf != desk->it_inf_koniec(); iter_inf++) 
			if (desk->kanaly[*iter_inf].adr == odp_pak->posrednicy[odp_pak->glebokosc]) {		
				if (write(desk->kanaly[*iter_inf].dsk, &typ_pak, R_SOCKET_PAKIET) <= 0)
					printf("> Błąd w write!\n");
				else if (write(desk->kanaly[*iter_inf].dsk, odp_pak, R_ODP_PAKIET) <= 0)
					printf("> Błąd w write!\n");
				break;
			}
	} else 
		pyt->ustaw_zrodlo(odp_pak); // pyt->ustaw_zrodlo(odp_pak->id, odp_pak->nadawca, odp_pak->rozmiar); // !
}

/* Obsługa pakietu SOCKET_POB */
void pobierz(pob_pakiet * pob_pak, udostepniane * uds, polaczenia * desk) 
{
	/* Zbędne... */
}

/* Obsługa pakietu SOCKET_FRG */
void fragment(frg_pakiet * frg_pak, pobieranie * pob, polaczenia * desk) 
{
	/* Zbędne... */
}

/* Obsługa pakietu SOCKET_KMN_PYT */
void komunikat(kmn_pakiet * kmn_pak, Wszystko * Globalne)
{
	socket_pakiet typ_pak;
	brk_pakiet brk_pak;
	it_inf it;
	bool czy_wyslane = false;
	
	if (kmn_pak->posrednicy[kmn_pak->glebokosc] != Globalne->pol->ip) {
		for (it = Globalne->pol->iterator_inf(); it != Globalne->pol->it_inf_koniec(); it++) 
			if (Globalne->pol->kanaly[*it].adr == kmn_pak->posrednicy[kmn_pak->glebokosc]) {
				nastepny(kmn_pak);
				typ_pak.typ = SOCKET_KMN;
				if (write(Globalne->pol->kanaly[*it].dsk, &typ_pak, R_SOCKET_PAKIET) <= 0) 
					printf("> Błąd w write!\n");
				else if (write(Globalne->pol->kanaly[*it].dsk, kmn_pak, R_KMN_PAKIET) <= 0) 
					printf("> Błąd w write!\n");
				else 
					czy_wyslane = true;
			}
		if (!czy_wyslane) {
			kmn_pakiet2brk_pakiet(kmn_pak, &brk_pak);
			for (it = Globalne->pol->iterator_inf(); it != Globalne->pol->it_inf_koniec(); it++) 
				if (Globalne->pol->kanaly[*it].adr == kmn_pak->posrednicy[kmn_pak->glebokosc]) {			
					typ_pak.typ = SOCKET_BRK;
					if (write(Globalne->pol->kanaly[*it].dsk, &typ_pak, R_SOCKET_PAKIET) <= 0) 
						printf("> Błąd w write!\n");
					else if (write(Globalne->pol->kanaly[*it].dsk, &brk_pak, R_BRK_PAKIET) <= 0) 
						printf("> Błąd w write!\n");
				}
		}
	} else {
		printf("> ...\n");
	}
}

/* Obsługa pakietu SOCKET_SAS_PYT */
void sasiedzi(sas_pyt_pakiet * sas_pyt, int desk, polaczenia * pol)
{
	sas_odp_pakiet sas_odp;
	socket_pakiet typ_pak;
	typ_pak.typ = SOCKET_SAS_ODP;
	ustaw(&sas_odp, pol);
	if (write(desk, &typ_pak, R_SOCKET_PAKIET) <= 0) 
		printf("> Błąd w write!\n");
	else if (write(desk, &sas_odp, R_SAS_ODP_PAKIET) <= 0) 
		printf("> Błąd w write!\n");
}

/* Obsługa pakietu SOCKET_SAS_ODP */
void sasiedzi(sas_odp_pakiet * sas_odp)
{
	int i;
	printf("> Odebrano listę sąsiedztwa:\n");
	for (i = 0; i < sas_odp->liczba; i++)
		printf("> %s\n", inet_ntoa(sas_odp->lista[i]));
}
