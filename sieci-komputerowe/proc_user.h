#ifndef _PROC_USER_H_
#define _PROC_USER_H_

extern int parser_polecen(slowo, slowo, int);
extern void pomoc();

extern void polacz(slowo, polaczenia *);
extern void rozlacz(int, polaczenia *);
extern void sasiedzi(slowo, polaczenia *);
extern void otwarte(polaczenia *);
extern void szukaj(slowo, polaczenia *, pytania *);
extern void wyszukane(pytania *);
extern void wyszukane_szczegoly(slowo, pytania *);
extern void szpieg(odpowiedzi *);
extern void udostepnione(udostepniane *);
extern void pobierz(slowo, pytania *, pobieranie *, polaczenia *);

#endif
