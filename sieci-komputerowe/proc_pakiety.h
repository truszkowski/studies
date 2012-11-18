#ifndef _PROC_PAKIETY_H_
#define _PROC_PAKIETY_H_

#include "definicje.h"
#include "operatory.h"
#include "klasy.h"

extern void ustaw(pyt_pakiet *, in_addr, slowo, int);
extern void nastepny(pyt_pakiet *, in_addr);
extern void nastepny(odp_pakiet *);
extern void pyt_pakiet2odp_pakiet(pyt_pakiet *, odp_pakiet *, udostepniane *, in_addr); //void *
extern void ustaw(pob_pakiet *, in_addr, slowo, long , long);
extern void ustaw(frg_pakiet *, long, long, dane);
extern void ustaw(kmn_pakiet *, void *, in_addr, in_addr);
extern void nastepny(kmn_pakiet *);
extern void kmn_pakiet2brk_pakiet(kmn_pakiet *, brk_pakiet *);
extern void nastepny(brk_pakiet *);
extern void ustaw(sas_pyt_pakiet *);
extern void ustaw(sas_odp_pakiet *, polaczenia *);

#endif
