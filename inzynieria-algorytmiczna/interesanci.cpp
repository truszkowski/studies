#include <cstdio>
#include <algorithm>

const unsigned max = 1000000;

struct P {
	unsigned long long a, b;
	unsigned i;

	bool operator<(const P &p) const {
		if (p.a * b < a * p.b) return true;
		if (p.a * b > a * p.b) return false;
		return i < p.i;
	}

	void swap(P &p) {
		a ^= p.a; p.a ^= a; a ^= p.a;
		b ^= p.b; p.b ^= b; b ^= p.b;
		i ^= p.i; p.i ^= i; i ^= p.i;
	}
};

P p[max];
unsigned n;

int main(void) {
	scanf("%u", &n);

	p[0].i = 1;
	for (unsigned i = 0; i < n; ++i, p[i].i = i+1)
		scanf("%llu%llu", &p[i].a, &p[i].b);

	unsigned z = 0;
	for (unsigned i = 0; i < n; ++i) 
		if (i != z && !p[i].a && !p[i].b) { 
			p[i].swap(p[z]); 
			++z;
		}

	std::sort(p+z, p+n);
	
	for (unsigned i = z, j = 0; i < n;) {
		if (j < z) {
			if (p[j].i < p[i].i) { printf("%u ", p[j].i); ++j; }
			else { printf("%u ", p[i].i); ++i; }
		} else {
			printf("%u ", p[i].i);
			++i;
		}
	}

	printf("\n");
	return 0;
}

