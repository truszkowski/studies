#include <cstdio>
#include <set>
#include <algorithm>
using namespace std;
const long MAX = 100000l;
__inline__ long long sq(const long long &a) { return a*a; }

long n, lastX;
pair<long,long> v[MAX];
long long d;

struct ycmp { // Porzadek dla Ystruktury.
	bool operator()(const long &i, const long &j) const { 
		return (v[i].second == v[j].second) ? v[i].first < v[j].first : v[i].second < v[j].second;
	} 
};

set<long, ycmp> sy; // Ystruktura dla miotly
typedef set<long, ycmp>::iterator yiter;

void init(void) { // startujemy z 2 pierwszymi punktami.
	sort(v,v+n); // tutaj przygotowujemy sobie Xstrukture sortem.
	d = sq(v[1].first - v[0].first) + sq(v[1].second - v[0].second);
	sy.insert(0);
	sy.insert(1);
	lastX = 0; // odznaczamy poczatek Xstruktury w v[]
}

void add(long i) { // dodajemy kolejny punkt.
	// Wywalamy odlegle punkty z Xstrutury = el v[]: (lastX, i-1)
	while (sq(v[i].first-v[lastX].first) > d) sy.erase(lastX++);

	// Poszukujemy w Ystrukturze okolic punktu v[i].
	yiter yb = sy.begin(), yi = sy.lower_bound(i), yj = yi;
	if (yb!=sy.end()) --yj;

	// sprawdzamy okolice wzwyz...
	while (yi != sy.end() && sq(v[*yi].second-v[i].second) <= d) {
		long long nd = sq(v[*yi].first-v[i].first) + sq(v[*yi].second-v[i].second);
		if (nd < d) d = nd;
		++yi;
	}
	
	// i sprawdzamy w dol...
	while (yj != sy.end() && sq(v[*yj].second-v[i].second) <= d) {
		long long nd = sq(v[*yj].first-v[i].first) + sq(v[*yj].second-v[i].second);
		if (nd < d) d = nd;
		if (yb == yj) break;
		--yj;
	}

	// i dodajemy do Ystruktury rozpatrzony juz punkt.
	sy.insert(i);
}

int main(void) {
	scanf("%ld", &n);
	for (long i = 0; i < n; ++i) scanf("%ld%ld", &v[i].first, &v[i].second);

	// i zamiatamy. 
	//  Xstruktura : v[lastX],...,v[i]
	//  Ystruktura : sy.
	init();
	for (long i = 2; i < n; ++i) add(i);

	printf("%lld\n", d);
	return 0;
}

