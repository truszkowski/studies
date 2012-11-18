//#include <netinet/in.h>
//#include <netdb.h>
//#include <arpa/inet.h>

#include "klasy.h"
#include "operatory.h"

void * watek_polacz_pob(void * dane)
{
	int i, w, id;
	slowo argument;
	polaczenia * desk;
	przesylka_pol_pob * p;
	p = (przesylka_pol_pob *) dane;
	w = p->nr_watku;
	id = p->id_pob;
	strcpy(argument, p->adres);
	desk = p->Globalne->pol;
	int gniazdo_nowe;
	struct hostent * htmp;
	struct sockaddr_in klient;
	in_addr adres;
	inf_pakiet i_pakiet; 
	socket_pakiet typ_pak;
	kmn_pakiet kmn_pak;
	it_inf it;
	
	if (desk->zbior_pob.size() == MAX_POB) 
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
			if (connect(gniazdo_nowe, (struct sockaddr *) &klient, sizeof(klient)) < 0) {//true){//
				printf("> Nie połaczono z %s\n", argument);
				printf("> Wysłanie prośby połączenia.\n");
				
				ustaw(&kmn_pak, &p->Globalne->pob->plik[id], p->Globalne->pol->ip, klient.sin_addr); //!
//				p->Globalne->pob->plik[id]
				typ_pak.typ = SOCKET_KMN;
				for (it = p->Globalne->pol->iterator_inf(); it != p->Globalne->pol->it_inf_koniec(); it++)
					if (p->Globalne->pol->kanaly[*it].adr == klient.sin_addr) {
						if (write(p->Globalne->pol->kanaly[*it].dsk, &typ_pak, R_SOCKET_PAKIET) <= 0) 
							printf("> Błąd w write!\n");
						else if (write(p->Globalne->pol->kanaly[*it].dsk, &kmn_pak, R_KMN_PAKIET) <= 0) 
							printf("> Błąd w write!\n");
						break;
					}	
				
			} else {
				inet_aton(argument, &adres); 
				int tmp = desk->ustaw_kanal_pob(gniazdo_nowe, adres, KANAL_POB);
				p->Globalne->pob->dodaj_kanal_pob(id, adres, p->Globalne->pol->kanaly[tmp].dsk);
				
				i_pakiet.inf = KANAL_POB; strcpy(i_pakiet.nazwa_pliku, p->Globalne->pob->plik[id].nazwa_pliku);
				if (write(gniazdo_nowe, &i_pakiet, R_INF_PAKIET) <= 0) {
					printf("> Bład w write!");
					desk->reset_kanal(gniazdo_nowe);
					if (close(gniazdo_nowe) == -1) 
						printf("> Błąd w close!");
				}	
				pob_pakiet pob_pak;
				p->Globalne->pob->plik[id].daj_fragmenty(&pob_pak, id);
				if (write(gniazdo_nowe, &pob_pak, R_POB_PAKIET) <= 0) {
					printf("> Bład w write!");
					desk->reset_kanal(gniazdo_nowe);
				}	
				
			}
		}
	}
	
	p->Globalne->wtk->oddaj_watek_pol_pob(w);
	return dane;
}
