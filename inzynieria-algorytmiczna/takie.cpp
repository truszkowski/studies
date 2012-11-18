#include <cstdio>
#include <cmath>
#include <algorithm>
using namespace std;
const long MAX = 100000l;
__inline__ long long mul(long long a, long long b) { return a*b; }
__inline__ long long sq(long long a) { return a*a; }

long ch[MAX+1];
pair<long,long> v[MAX];
long chn = 0, vn = 0;
long long d = 0;

// Kierunek
long long direction(long i, long j, long k) {
	long long det = 
		mul(v[i].first, v[j].second) + mul(v[k].first, v[i].second) + mul(v[j].first, v[k].second)
		- mul(v[i].first, v[k].second) - mul(v[k].first, v[j].second) - mul(v[j].first, v[i].second);
	return det;
}

void convex_hull(void) {
	// Sortujemy po x-ach.
	sort(v,v+vn);
	
	// Poczatkowo mamy 2 pierwsze punkty z v w otoczce.
	chn = 2;
	ch[0] = 0;
	ch[1] = 1;

	// Minimalna wielkosc otoczki - aby nie wywalac zbyt duzo z niej pktow
	long guard = 1;
	
	// Dokladamy kolejne punkty do otoczki, od 0->vn-1
	for (long i = 2; i < vn; ++i) {
		while (chn > guard && direction(ch[chn-2], ch[chn-1], i) <= 0) --chn; // na prawo
		ch[chn++] = i; // na lewo
	}

	guard = chn;

	// W druga strone vn-1->0
	for (long i = vn-2; i >= 0; --i) {
		while (chn > guard && direction(ch[chn-2], ch[chn-1], i) <= 0) --chn; // na prawo 
		ch[chn++] = i; // na lewo
	}

	--chn;
}

long long dist(long i, long j) { return sq(v[i].first-v[j].first)+sq(v[i].second-v[j].second); }

double dist(long a, long b, long i) {
	double A, B, C;// Ax+By+C=0 //
	A = v[a].second - v[b].second;
	B = v[b].first - v[a].first;
	C = mul(v[a].first, v[b].second) - mul(v[b].first, v[a].second);
	return abs(A*v[i].first + B*v[i].second + C) / sqrt(A*A + B*B);
}

void find_width(void) {
	d = 0;
	long long nd1 = 0, nd2 = 0;

	// Dla kazdej krawedzi z otoczki szukamy puntu najdalej od niej.
	for (long i = 0, x = 1; i < chn; ++i) {
		double ld = 0, tmp = -1;

		// Bedziemy szli po punktach z otoczki caly czas w te sama strone az
		// dojdziemy do punktu najbardziej oddalonego.
		for (long j = x; j < chn; ++j) {
			if ((nd1 = dist(ch[i], ch[j])) > d) d = nd1;
			if ((nd2 = dist(ch[i+1], ch[j])) > d) d = nd2;
			if ((tmp = dist(ch[i], ch[i+1], ch[j])) < ld) break;

			ld = tmp;
			x = j;
		}
	}
}

int main(void) {
	scanf("%ld", &vn);
	for (long i = 0; i < vn; ++i) 
		scanf("%ld%ld", &v[i].first, &v[i].second);

	// Szukamy otoczki wypuklej, tylko punkty lezace na niej moga byc
	// najbardziej odleglymi.
	convex_hull();

	// Szukamy po otoczce najwiekszej srednicy - czyli najbardziej 
	// odleglych 2 punktow.
	find_width();

	printf("%lld\n", d);
	return 0;
}

