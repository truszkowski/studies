#ifndef _WATKI_H_
#define _WATKI_H_

#include "definicje.h"
#include "operatory.h"
#include "klasy.h"

struct przesylka
{
	int nr_watku;
	struct Wszystko * Pierscien;
};

struct przesylka_pol_inf
{
	int nr_watku;
	slowo adres;
	struct Wszystko * Globalne;
};

struct przesylka_pol_pob
{
	int nr_watku;
	int id_pob;
	slowo adres;
	struct Wszystko * Globalne;
};

struct przesylka_pob
{
	int nr_watku;
	slowo plik;
	struct Wszystko * Globalne;
};

struct przesylka_frg
{
	int nr_watku;
	int id_pol;
	slowo plik;
	struct Wszystko * Globalne;
};

extern void * watek_polacz_inf(void *);
extern void * watek_polacz_pob(void *);
extern void * watek_pobierz(void *);
extern void * watek_fragmenty(void *);

#endif
