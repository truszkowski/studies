#ifndef _PROC_SERWER_H
#define _PROC_SERWER_H

extern void pytanie(pyt_pakiet *, pytania *, odpowiedzi *, udostepniane *, polaczenia *);
extern void odpowiedz(odp_pakiet *, pytania *, odpowiedzi *, polaczenia *);
extern void pobierz(pob_pakiet *, udostepniane *, polaczenia *);
extern void fragment(frg_pakiet *, pobieranie *, polaczenia *);
extern void komunikat(kmn_pakiet *, Wszystko *);
extern void sasiedzi(sas_pyt_pakiet *, int, polaczenia *);
extern void sasiedzi(sas_odp_pakiet *);

#endif
