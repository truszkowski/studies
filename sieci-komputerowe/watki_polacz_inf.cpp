#include "klasy.h"
#include "operatory.h"

// Wszystko Globalne_Dane; //

void * watek_polacz_inf(void * dane)
{
	int w;
	slowo argument;
	polaczenia * desk;
	przesylka_pol_inf * p;
	p = (przesylka_pol_inf *) dane;
	w = p->nr_watku;
	strcpy(argument, p->adres);
	desk = p->Globalne->pol;
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
	p->Globalne->wtk->oddaj_watek_pol_inf(w);
	return dane;
}
