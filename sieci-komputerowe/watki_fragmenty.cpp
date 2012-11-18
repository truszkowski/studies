#include "klasy.h"
#include "operatory.h"


void * watek_fragmenty(void * dane)
{
	int w, dsk, id_p;
	long i;
	slowo argument;
	przesylka_frg * p;
	Wszystko * Glb;
	zapytanie dane_pliku;
	int id_pob;
	char sciezka[2 * MAX_SLW];
	struct pob_pakiet pob_pak;
	struct frg_pakiet frg_pak;

	p = (przesylka_frg *) dane;
	w = p->nr_watku;
	strcpy(argument, p->plik);
	id_p = p->id_pol;
//	Glb = p->Globalne;

	ssize_t s1, s2;
	bool koniec_wysylania = false;
	
	/* Opcjonalne łączenie się(gdy jesteśmy za firewallem lub natem)... */
	
	
	/* Wybór ścieżki */
	if ((p->Globalne->uds->czy_udostepniam_frg(argument)) && (!p->Globalne->uds->czy_udostepniam(argument)))
		strcpy(sciezka, KATALOG_POB);
	else
		strcpy(sciezka, KATALOG_UDS);
	strcat(sciezka, argument);
	
	if ((dsk = open(sciezka, O_RDONLY)) != -1) {
		while (!koniec_wysylania)
			switch (s1 = read_while(p->Globalne->pol->kanaly[id_p].dsk, &pob_pak, R_POB_PAKIET))
			{
				case -1:
					printf("> Koniec Wysyłania...(-1)\n");
					koniec_wysylania = true;
					break;
					
				case 0:
					printf("> Koniec Wysyłania...(0)\n");
					koniec_wysylania = true;
					break;
					
				default:
					if (pob_pak.frg_start < pob_pak.frg_koniec)
						printf("> Wysyłanie fragmentów: <%ld, %ld>\n", pob_pak.frg_start, pob_pak.frg_koniec);
					else {
						printf("> Wszystkie fragmenty wysłane!\n");
						koniec_wysylania = true; break;
					}	
					for (i = pob_pak.frg_start; i < pob_pak.frg_koniec; i++) {// wysyłanie odbywa sie od (start) do (koniec-1)
						
						if ((s1 = lseek(dsk, i * MAX_DANE, SEEK_SET)) == -1) {
							printf("> Błąd w lseek! Przerywam wysyłanie...\n");
							koniec_wysylania = true; p->Globalne->pol->reset_kanal(id_p); break;
						}
						if ((s2 = read(dsk, frg_pak.fragment, MAX_DANE)) <= 0) {
							printf("> Błąd w read! Przerywam wysyłanie...(%ld,%ld)\n", i, s1);
							koniec_wysylania = true; p->Globalne->pol->reset_kanal(id_p); break;
						}
						ustaw(&frg_pak, i, (long) s2, frg_pak.fragment);
						if (write(p->Globalne->pol->kanaly[id_p].dsk, &frg_pak, R_FRG_PAKIET) == -1) {
							printf("> Błąd podczas wysyłania pliku! Przerywam wysyłanie...\n");
							koniec_wysylania = true; p->Globalne->pol->reset_kanal(id_p); break;
						}
						printf("> (%ld) (%ld) (%ld)\n", frg_pak.nr_frg, s1, s2);
					}
			}
		if (close(dsk) == -1)
			printf("> Błąd w close!\n");
	} else
		printf("> Nieprawidłowy plik! Przerywam połączenie...\n");

	p->Globalne->pol->reset_kanal(id_p);

	p->Globalne->wtk->oddaj_watek_frg(w);
	return dane;
}
