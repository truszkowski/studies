/* 
 * Na wejsciu trzeba podawac:
 * v1, v2, ..., v20
 * 
 * gdzie:
 * vi - reszty z dzielenia przez mi.
 * mi - podane w tablicy rprime.
 */

#include <iostream>
#include <vector>
#include <gmpxx.h>
#include "garner.hpp"
//#include "bignum.hpp"

using namespace std;
typedef mpz_class num;

int rprime[20] = { // jedynie 4 ostanie liczby nie sa pierwsze,
	10955551, 11111101, 11111117, 11111119, 12809539,
	12951131, 13330487, 13621781, 14000029, 14433059, 
	14917741, 14945339, 15309793, 15479153, 15485059, 
	15485857, 57930181, 57892019, 98372710, 76281973
};


int t;
num x;
vector<num> m, v;

int main() {
	t = 20;

	m.resize(t);
	for (int i = 0; i < t; ++i)
		m[i] = rprime[i];
	
	v.resize(t);
	for (int i = 0; i < t; ++i)
		cin >> v[i];
	
// Definiujemy...
	CRT<num> crt(m, v);
// Wyznaczamy rozwiazanie...
	cout << crt.garner() << endl;
// Sprawdzamy...
	assert(crt.check());
// Ok,

	return 0;
}
