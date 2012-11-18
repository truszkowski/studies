#ifndef _KLASY_H_
#define _KLASY_H_

#include "klasy_polaczenia.h"
#include "klasy_pytania.h"
#include "klasy_odpowiedzi.h"
#include "klasy_udostepniane.h"
#include "klasy_pobieranie.h"
#include "klasy_fragmenty.h"

#include "klasy_watki.h"

class Wszystko 
{
	public:
		polaczenia * pol;
		pytania * pyt;
		odpowiedzi * odp;
		pobieranie * pob;
		fragmenty * frg;
		udostepniane * uds;
		watki * wtk;
		
		Wszystko() {}
		~Wszystko() 
		{
//			~*pol;
//			~*pyt;
//			~*odp;
//			~*pob;
//			~*frg;
//			~*uds;
		}
		void ustaw(polaczenia * pl, pytania * pt, odpowiedzi * od, pobieranie * po, fragmenty * fr, udostepniane * ud, watki * wt)
		{
			pol = pl; pyt = pt; odp = od; pob = po; frg = fr; uds = ud; wtk = wt;
		}
		void koniec()
		{
				wtk->koniec();
				pol->koniec();
//			pyt->koniec();
//			odp->koniec();
//			pob->koniec();
//			frg->koniec();
//			uds->koniec();
		}
};


#endif 
