#include <cstdio>
#include <cstring>

#define BITMAX 1625001
#define PARTSIZE 1000107
int bits[BITMAX];
unsigned primes1229[1229];
unsigned last1229[1229];
	
#define isprime(x) ( (bits[(x) >> 6] & (1 << (((x) & 63) >> 1))) )
#define mark(x) bits[(x) >> 6] &= ~(1 << (((x) & 63) >> 1))

int main(void) {
	unsigned a, b, m, t = 0;
	unsigned long long s = 0;
	scanf("%u%u%u", &a, &b, &m);
	
	register unsigned p, i;
	unsigned j, k, l;

	memset(bits, 0xff, sizeof(int)*BITMAX);
	primes1229[0] = 2;
	primes1229[1] = 3;

	for (p = 5, k = 2; p <= 10000; p += 4) {
		if (isprime(p)) { 
			primes1229[k] = p; 
			for (i = p*p, j = p*(p+2); i <= PARTSIZE; i += 6*p, j += 6*p) { mark(i); mark(j); } 
			last1229[k] = i;
			++k; 
		}
		p += 2;
		if (isprime(p)) { 
			primes1229[k] = p; 
			for (i = p*p, j = p*(p+4); i <= PARTSIZE; i += 6*p, j += 6*p) { mark(i); mark(j); } 
			last1229[k] = i;
			++k; 
		}
	}

	for (k = PARTSIZE << 1;; k += PARTSIZE) {
		l = k < b ? k : b;
		for (j = 2; j < 1229; ++j) {
			for (i = last1229[j]; i <= l; i += 2*primes1229[j])
				mark(i); 
			last1229[j] = i;
		}
		if (k >= b) break;
	}

	if (a <= 2 && 2 <= b) { if (!t) s += 2; if (++t == m) t = 0; }
	if (a <= 3 && 3 <= b) { if (!t) s += 3; if (++t == m) t = 0; }
	if (a < 5) a = 5; else if (!(a & 1)) ++a;
	if (!(a % 3)) a += 2;

	if (a % 3 == 2)
		for (i = a; i <= b; i += 4) {
			if (isprime(i)) { if (!t) s += i; if (++t == m) t = 0; }
			i += 2;
			if (i > b) break;
			if (isprime(i)) { if (!t) s += i; if (++t == m) t = 0; }
		}
	else
		for (i = a; i <= b; i += 2) {
			if (isprime(i)) { if (!t) s += i; if (++t == m) t = 0; }
			i += 4;
			if (i > b) break;
			if (isprime(i)) { if (!t) s += i; if (++t == m) t = 0; }
		}

	printf("%llu\n", s);
	return 0;
}


