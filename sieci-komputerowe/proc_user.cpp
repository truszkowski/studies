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
#include <pthread.h>
#include <dirent.h>

#include "definicje.h"
#include "operatory.h"
#include "klasy.h"
#include "proc_pakiety.h"
#include "proc_user.h"
//#include "watki.h"

using namespace std;

//extern Wszystko Globalne_Dane; //...

/* Parser poleceń */
int parser_polecen(slowo polecenie, slowo argument, int rozmiar) // wyodrebnia z polecenia komendy i argumenty;
{
	int i, tmp = 0, koniec_linni = 1;
	
	if (rozmiar > MAX_SLW)
		rozmiar = MAX_SLW;

	for (i = 0; i < rozmiar; i++) // pomijamy poczatkowe spacje;
		if (polecenie[i] == '\n') {
			polecenie[0] = '\0'; return 1;
		}	else if (polecenie[i] != ' ') { 
			tmp = i; break; 
		}
	
	for (i = tmp; i < rozmiar; i++) 
		if (polecenie[i] == '\n') { 
			polecenie[i] = '\0'; argument[0] = '\0'; return 0;
		} else	if (polecenie[i] == ' ') { 
			polecenie[i] = '\0'; tmp = i + 1; break; // wczytano spacje wiec wczytano cala komende;
		}
	
	for (i = tmp; i < rozmiar; i++) 
		if (polecenie[i] == '\n') { 
			argument[0] = '\0'; return 0; // komenda bezargumentowa;
		} else if (polecenie[i] != ' ') { 
			tmp = i; break; // co ciagu spacji wczytujemy argument;
		}
	
	if (polecenie[tmp] == '/') // znak "/" oznacza cytowanie(niektore pliki mogasie zaczynac od spacji);
		tmp++;
	
	if (tmp >= rozmiar) 
		return 1; // bład bo nie bylo '\n';
	
	if (polecenie[tmp] == '\n') {
		argument[0] = '\0'; return 0; // komenda bezargumentowa;
	}
	
	for (i = tmp; i < rozmiar; i++)
		if (polecenie[i] != '\n')
			argument[i - tmp] = polecenie[i]; // przepisywanie argumentu;
		else { 
			argument[i - tmp] = '\0'; koniec_linni = 0; break; // jest '\n', koniec;
		}
	
	return koniec_linni; // zwracamy czy we wczytanym poleceniu byl znak konca linni
}

/* Menu pomocy */

void pomoc() 
{
	printf("> Dostępne polecenia:\n");
	printf("> pomoc :: to menu.\n");
	printf("> polacz <ip> :: połącz się z <ip>.\n");
	printf("> rozlacz <id_polaczenia> :: rozłącz sie z <id_połączenia>.\n");
	printf("> otwarte :: lista otwartych połączeń.\n");
	printf("> sasiedzi <id_pałączenia> :: lista sąsiadów wybranego połączenia.\n");
	printf("> szukaj <nazwa_pliku> :: wyszukiwanie.");
	printf("> wyszukane :: wyniki wyszukiwań.\n");
	printf("> wyszukany <id_wyszukanego> :: wyniki wyszukiwań konretnego pliku.\n");
	printf("> szpieg :: to czego ktoś u nas szukał.\n");
	printf("> pliki :: udostepniane pliki.\n");
	printf("> pobierz <nazwa_pliku> :: pobieranie pliku.\n");
	printf("> pobrane :: wykaz pobranych plików.\n");
	printf("> koniec :: koniec pracy.\n");
	printf("> \n");
	printf("> Jeśli nazwa pliku zaczyna sie od spacji\n");
	printf("> nazwę pliku poprzedź znakiem: \"/\".\n");
}

/* Łączenie się z innym klientem */
void polacz(slowo argument, polaczenia * desk) //
{

		int gniazdo_nowe;
	struct hostent * htmp;
	struct sockaddr_in klient;
	in_addr adres;
	if (desk->zbior_inf.size() == MAX_INF) 
		printf("> Osiągnąłeś limit połączeń!\n");
	else {
		printf("> Łaczę się z %s\n", argument);
		gniazdo_nowe = socket(PF_INET, SOCK_STREAM, 0);
		if ((htmp = gethostbyname(argument)) == NULL) 
			printf("> Błąd w gethostbyname!\n");
		else {
			klient.sin_family = AF_INET;
			memcpy((char *) &klient.sin_addr.s_addr, (char * ) htmp->h_addr, htmp->h_length);
			klient.sin_port = PORT;
			if (connect(gniazdo_nowe, (struct sockaddr *) &klient, sizeof(klient)) < 0)
				printf("> Nie połaczono z %s\n", argument);
			else {
				inet_aton(argument, &adres); // !!! if == 0 źle
				int tmp = desk->ustaw_kanal_inf(gniazdo_nowe, adres);
				inf_pakiet i_pakiet; i_pakiet.inf = KANAL_INF;
				if (write(gniazdo_nowe, &i_pakiet, R_INF_PAKIET) <= 0) {
					printf("> Bład w write!");
					desk->reset_kanal(gniazdo_nowe);
					if (close(gniazdo_nowe) == -1) 
						printf("> Błąd w close!");
				}
			}
		}
	}
}

/* Rozłączanie z innym klientem */
void rozlacz(int nr_desk, polaczenia * desk) //
{
	desk->reset_kanal(nr_desk);
}

/* Prośba o listę sąsiadów */
void sasiedzi(slowo argument, polaczenia * desk)
{
	it_inf it;
	int tmp = atoi(argument);
	socket_pakiet typ_pak;
	sas_pyt_pakiet sas_pyt_pak;
	
	if (0 <= tmp < MAX_KANAL) {
		printf("> Prośba o listę sąsiadów %s...\n", inet_ntoa(desk->kanaly[tmp].adr));
		typ_pak.typ = SOCKET_SAS_PYT;
		ustaw(&sas_pyt_pak);
		if (write(desk->kanaly[tmp].dsk, &typ_pak, R_SOCKET_PAKIET) <= 0) 
			printf("> Błąd w write!\n");
		else if (write(desk->kanaly[tmp].dsk, &sas_pyt_pak, R_SAS_PYT_PAKIET) <= 0) 
			printf("> Błąd w write!\n");
	} else
		printf("> Brak podanego sąsiada!\n");
}

/* Wypisanie połączeń */
void otwarte(polaczenia * desk) 
{
	desk->wypisz();
}

/* Szukanie pliku */
void szukaj(slowo argument, polaczenia * desk, pytania * pyt) 
{
	struct socket_pakiet typ_pak;
	struct pyt_pakiet pyt_pak;
	int i, kanal;
	it_inf iter_inf;
	typ_pak.typ = SOCKET_PYT;
	ustaw(&pyt_pak, desk->ip, argument, pyt->dodaj(argument));
	
	for (iter_inf = desk->iterator_inf(); iter_inf != desk->it_inf_koniec(); iter_inf++) {
		printf("> Przeszukiwanie...\n");
		if (write(desk->kanaly[*iter_inf].dsk, &typ_pak, R_SOCKET_PAKIET) <= 0) 
			printf("> Błąd w write!\n");
		else if (write(desk->kanaly[*iter_inf].dsk, &pyt_pak, R_PYT_PAKIET) <= 0) 
			printf("> Błąd w write!\n");
	}
}

/* Wypisanie wyszukiwań */
void wyszukane(pytania * pyt) 
{
	pyt->wypisz();
}

/* Wypisanie wyszukiwań - szczegóły */
void wyszukane_szczegoly(slowo argument, pytania * pyt) 
{
	pyt->wypisz(atoi(argument));
}

void szpieg(odpowiedzi * odp)
{
	odp->wypisz();
}


/* Wypisanie listy plików udostępnianych */
void udostepnione(udostepniane * uds) 
{
	uds->wypisz();
}

/* Pobieranie pliku */
void pobierz(slowo argument, pytania * pyt, pobieranie * pob, polaczenia * desk) 
{
	
}
