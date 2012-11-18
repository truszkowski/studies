#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "definicje.h"
#include "operatory.h"
#include "klasy.h"
#include "proc.h"

using namespace std;

Wszystko Globalne_Dane; 

void koniec_p2p(int s)
{
	printf("> Kończenie pracy programu...\n");
	Globalne_Dane.koniec();
	exit(0);
}

int main(int argc, char * argv[0]) 
{
	
	/* Zmienne... */
	struct sockaddr_in serwer, klient, klient_nowy;
	int gniazdo_nowe, i, kanal;
	socklen_t dlugosc;
	slowo polecenie, argument;
	ssize_t wynik_read;

	/* Zbior deskryptorów do otwartych socketów... */
	fd_set zbior_dsk; 

	/* Klasy... */
	polaczenia pol;
	pytania pyt;
	odpowiedzi odp;
	udostepniane uds;
	pobieranie pob;
	fragmenty frg;
	watki wtk;
	
	/* Iteratory */
	it_inf iter_inf;
	it_pob iter_pob;
	it_nzn iter_nzn;
	
	/* Przesyłane pakiety... */
	struct socket_pakiet socket_pak;
	struct inf_pakiet inf_pak;
	struct pyt_pakiet pyt_pak;
	struct odp_pakiet odp_pak;
	struct pob_pakiet pob_pak;
	struct frg_pakiet frg_pak;
	struct kmn_pakiet kmn_pak;
	struct brk_pakiet brk_pak;
	struct sas_psr_pakiet sas_psr_pak;
	struct sas_pyt_pakiet sas_pyt_pak;
	struct sas_odp_pakiet sas_odp_pak;
	
	/* Timeout... */
	timeval czas;
	
	/* Ustawienie globalnych danych */
	Globalne_Dane.ustaw(&pol, &pyt, &odp, &pob, &frg, &uds, &wtk);
	
	/* Postawienie serwera */
	if (argc == 2)
		pol.start(argv[1]);
	else
		pol.start("domyślny");
	
	/* Ctrl+C... */
	if (signal(SIGINT, koniec_p2p) == SIG_ERR)
    perror("> Błąd w signal!");
	
	/* Odczyt w pętli nieskończonej */
	while (true)
	{
		/* ustawienie timeoutu */
		czas.tv_sec = 2;
		czas.tv_usec = 0;
		/* wyzerowanie zbioru */
		FD_ZERO(&zbior_dsk); 
		/* uzupełnienie zbioru */
		FD_SET(pol.user_odczyt, &zbior_dsk); 
		FD_SET(pol.socket_odczyt, &zbior_dsk);
		for (iter_inf = pol.iterator_inf(); iter_inf != pol.it_inf_koniec(); iter_inf++) 
			FD_SET(pol.kanaly[*iter_inf].dsk, &zbior_dsk);
		for (iter_nzn = pol.iterator_nzn(); iter_nzn != pol.it_nzn_koniec(); iter_nzn++) 
			FD_SET(pol.kanaly[*iter_nzn].dsk, &zbior_dsk);
		
		/* wybór niepustych deskryptorów */
		if (select(FD_SETSIZE, &zbior_dsk, NULL, NULL, &czas) == -1) {
			printf("> Błąd w select!\n"); koniec_p2p(0);
		}

		/* obsługa kanałów informacyjnych */
		for (iter_inf = pol.iterator_inf(); iter_inf != pol.it_inf_koniec(); iter_inf++) 
			if (FD_ISSET(pol.kanaly[*iter_inf].dsk, &zbior_dsk)) 
				if ((wynik_read = read(pol.kanaly[*iter_inf].dsk, &socket_pak, R_SOCKET_PAKIET)) < 0) {
					printf("> Nieczytam z (%d)\n", pol.kanaly[*iter_inf].dsk);
					pol.reset_kanal(*iter_inf); // ?
				} else {
					if (wynik_read == 0) {
						pol.reset_kanal(*iter_inf);
					} else 
						switch (socket_pak.typ)
						{
							case SOCKET_PYT: 
								if ((wynik_read = read(pol.kanaly[*iter_inf].dsk, &pyt_pak, R_PYT_PAKIET)) <= 0)
									printf("> Błąd w read!\n");
								else 
									pytanie(&pyt_pak, &pyt, &odp, &uds, &pol);
								break;
								
							case SOCKET_ODP: 
								if ((wynik_read = read(pol.kanaly[*iter_inf].dsk, &odp_pak, R_ODP_PAKIET)) <= 0)
									printf("> Błąd w read!\n");
								else  
									odpowiedz(&odp_pak, &pyt, &odp, &pol);
								break;
								
							case SOCKET_SAS_PYT:
								if ((wynik_read = read(pol.kanaly[*iter_inf].dsk, &sas_pyt_pak, R_SAS_PYT_PAKIET)) <= 0)
									printf("> Błąd w read!\n");
								else
									sasiedzi(&sas_pyt_pak, pol.kanaly[*iter_inf].dsk, &pol);
								break;
								
							case SOCKET_SAS_ODP:
								if ((wynik_read = read(pol.kanaly[*iter_inf].dsk, &sas_odp_pak, R_SAS_ODP_PAKIET)) <= 0)
									printf("> Błąd w read!\n");
								else  
									sasiedzi(&sas_odp_pak);
								break;
							
							case SOCKET_SAS_PSR:
								if ((wynik_read = read(pol.kanaly[*iter_inf].dsk, &sas_psr_pak, R_SAS_PSR_PAKIET)) <= 0)
									printf("> Błąd w read!\n");
								else  
									printf("> ups....\n");
								break;
							case SOCKET_KMN:
								if ((wynik_read = read(pol.kanaly[*iter_inf].dsk, &kmn_pak, R_KMN_PAKIET)) <= 0)
									printf("> Błąd w read!\n");
								else  
									komunikat(&kmn_pak, &Globalne_Dane);
								break;
								
							case SOCKET_BRK:
								if ((wynik_read = read(pol.kanaly[*iter_inf].dsk, &brk_pak, R_BRK_PAKIET)) <= 0)
									printf("> Błąd w read!\n");
								else  
									printf("> ups....\n");
								break;
							
							default:
								printf("> Nieznany typ SOCKET(%d)!\n", socket_pak.typ);
						}
				}

		/* obsługa kanałów nieokreślonych */
		for (iter_nzn = pol.iterator_nzn(); iter_nzn != pol.it_nzn_koniec(); iter_nzn++) 
			if (FD_ISSET(pol.kanaly[*iter_nzn].dsk, &zbior_dsk)) 
				if ((wynik_read = read(pol.kanaly[*iter_nzn].dsk, &inf_pak, R_INF_PAKIET)) < 0) {
					printf("> Nieczytam z (%d)\n", pol.kanaly[*iter_nzn].dsk);
					pol.reset_kanal(*iter_nzn); // pol.kanaly[*iter_nzn].reset();
				} else {
					if (wynik_read == 0) {
						pol.reset_kanal(*iter_nzn); // pol.kanaly[*iter_nzn].reset(); // !! reset_kanal(rekord2dsk[*iter_nzn]);
					} else {
						int id_pol = pol.ustaw(*iter_nzn, inf_pak.inf); // !??!
						switch (inf_pak.inf)
						{
							case KANAL_INF: // kanał informacyjny;
								/* Nic... */
								break;
								
							case KANAL_POB: // ktoś chce pobierac ode mnie plik;
								printf("> wysyłanie pliku...\n");
								wtk.daj_watek_frg(&Globalne_Dane, id_pol, inf_pak.nazwa_pliku); 
								break;
							
							case KANAL_FRG: // bede dostawal plik;
								printf("> ustanowiono połączenie, dostaję plik...\n");
								break;
							
							case KANAL_PSR: // bede posrednikiem;
								printf("> jestem pośrednikiem...\n");
								break;
							
							default:
								printf("> Ktoś pisze do nieokreślonego kanału!\n");
						}
						break; // ;-(
					}
				}

		/* obługa użytkownika */
		if (FD_ISSET(pol.user_odczyt, &zbior_dsk)) {
			if ((wynik_read = read(pol.user_odczyt, polecenie, MAX_SLW)) <= 0) {
				printf("> Błąd w read!");
			} else 
				if (parser_polecen(polecenie, argument, wynik_read) == 0) {
					printf("> %s %s\n", polecenie, argument);
					if  (strcmp(polecenie, "polacz") == 0)
						wtk.daj_watek_pol_inf(&Globalne_Dane, argument);
					else if (strcmp(polecenie, "rozlacz") == 0)
						rozlacz(atoi(argument), &pol);
					else if (strcmp(polecenie, "otwarte") == 0)
						otwarte(&pol);
					else if (strcmp(polecenie, "sasiedzi") == 0)
						sasiedzi(argument, &pol);
					else if (strcmp(polecenie, "szukaj") == 0)
						szukaj(argument, &pol, &pyt);
					else if (strcmp(polecenie, "wyszukane") == 0)
						wyszukane(&pyt);
					else if (strcmp(polecenie, "wyszukany") == 0)
						wyszukane_szczegoly(argument, &pyt);
					else if (strcmp(polecenie, "szpieg") == 0)
						szpieg(&odp);
					else if (strcmp(polecenie, "pliki") == 0)
						udostepnione(&uds);
					else if (strcmp(polecenie, "pobierz") == 0)
						wtk.daj_watek_pob(&Globalne_Dane, argument); 
					else if (strcmp(polecenie, "pobrane") == 0)
						uds.pobrane();
					else if (strcmp(polecenie, "koniec") == 0)
						koniec_p2p(0);
					else if (strcmp(polecenie, "pomoc") == 0)
						pomoc();
					else
						printf("> Nieprawidłowe polecenie!\n");
				}
		}

		/* nasłuchiwanie nowego połaczenia */
		if (FD_ISSET(pol.socket_odczyt, &zbior_dsk)) {
			dlugosc = sizeof(klient_nowy);
			gniazdo_nowe = accept(pol.socket_odczyt, (struct sockaddr *) &klient_nowy, (socklen_t *) &dlugosc);
			if (pol.zbior_nzn.size() == MAX_NZN) {
				if (close(gniazdo_nowe) == -1) 
					printf("> Błąd w close!\n");
			} else 
				pol.ustaw_kanal(gniazdo_nowe, klient_nowy.sin_addr, KANAL_NZN);
		}
	}
}
