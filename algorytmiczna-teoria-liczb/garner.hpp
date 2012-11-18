/*
 * Rozwiazywanie układu kongruencji
 * z chinskiego twierdzenia o resztach(CRT)
 * za pomoca algorytmu Garnera.
 */

#ifndef __GARNER_H__
#define __GARNER_H__

#include <vector>
#include <cassert>
#include "gcde.hpp"

template <class __Type = int>
class CRT {
private:
	__Type __x;
	std::vector<__Type> __m, __v;
public:
	CRT(std::vector<__Type> & mn, std::vector<__Type> & vn) {
		set(mn, vn);
	}
	CRT(const CRT & c) 
	: __x(c.__x), __m(c.__m), __v(c.__v) { }
	~CRT(void) { }

	CRT & set(std::vector<__Type> & mn, std::vector<__Type> & vn) {
		assert(mn.size() == vn.size());
		__m = mn; __v = vn; 
		return *this;
	}
	const __Type & x(void) const { return __x; }

	__Type garner(void) {
// Algorytm Garnera,
		__Type u, M;
		int t = __m.size();
		std::vector<__Type> c(t);
		gcde<__Type> g;
	
		for (int i = 1; i < t; ++i) {
			c[i] = 1;
			for (int j = 0; j < i; ++j) {
// wyznaczenie m_j^(-1) mod m_i,
				u = g.set(__m[j], __m[i]).go().inverse();
				c[i] = ( u * c[i] ) % __m[i];
			}
		} // mamy wspolczynniki c,

		u = __v[0]; __x = u;
		
		M = 1;
		for (int i = 1; i < t; ++i) {
			M *= __m[i-1];
			u = ( (__v[i] - __x)*c[i] ) % __m[i];
			__x = __x + u*M;
		}
			
		M *= __m[t-1];
// trzeba uwazac, gdy iloczyn dzielnikow 
// wychodzi poza zakres,
		__x = ( __x > 0 ? __x : M + __x );
// i zawsze zwracamy dodatnia wartosc,
		return __x;
	}

	bool check(void) {
		return check(__x);
	}
	
	bool check(__Type xn) {
		int t = __m.size();
		for (int i = 0; i < t; ++i)
			if (xn % __m[i] != __v[i] % __m[i])
				return false;
		return true;
	}
};

#endif
