#include <cstdio>
#include <cstring>
typedef unsigned long long int u64;
typedef unsigned long int u32;
const u32 MAX = 213;
const u32 MOD = 65536;
const u32 POWER = 30;
u32 n = MAX, m = 0;

class matrixNN {
	private:
		u64 data[MAX*MAX];

	public:
		matrixNN(void) { bzero(data, sizeof(u64)*n*n); }
		matrixNN(const matrixNN &mnn) { memcpy(data, mnn.data, sizeof(u64)*n*n); }
		~matrixNN(void) { }

		u64 *operator[](u32 i) { return data + (i*n); }
		const u64 *operator[](u32 i) const { return data + (i*n); }
		matrixNN operator*(const matrixNN &m) {
			matrixNN x;
			x.opt(*this, m);
			return x;
		}
		void ikj(const matrixNN &m1, const matrixNN &m2) {
			register u32 i; u32 j, k;
			for (i = 0; i < n; ++i)
				for (k = 0; k < n; ++k)
					for (j = i; j < n; ++j)
						data[i*n+j] = ( data[i*n+j] + m1.data[i*n+k] * m2.data[k*n+j] ) % MOD;
//			for (i = 0; i < n; ++i)
//				for (j = i; j < n; ++j)
//					data[j*n+i] = ( data[i*n+j] %= MOD );
			for (i = 0; i < n-1; ++i) 
				data[(n-1)*n+i] = data[(n-2)*n+i] = 0;
		}
		void opt(const matrixNN &m1, const matrixNN &m2) { 
			register u32 i; u32 j, k; // Mnozymy prawie symetryczne macierze.
			for (i = 0; i < n-2; ++i) { // sym_ikj
				for (k = 0; k < i; ++k)
					for (j = i; j < n-2; ++j)
						data[i*n+j] = ( data[i*n+j] + m1.data[k*n+i] * m2.data[k*n+j] );	
				for (k = i; k < n-2; ++k) {
					for (j = i; j < k; ++j)
						data[i*n+j] = ( data[i*n+j] + m1.data[i*n+k] * m2.data[j*n+k] );
					for (j = k; j < n-2; ++j)
						data[i*n+j] = ( data[i*n+j] + m1.data[i*n+k] * m2.data[k*n+j] );
				}
			}
			for (j = 0; j < n-2; ++j) { // sym_ji
				for (i = 0; i < j; ++i)
					data[i*n+n-2] = ( data[i*n+n-2] + m1.data[i*n+j] * m2.data[j*n+n-2] );
				for (i = j; i < n-2; ++i) 
					data[i*n+n-2] = ( data[i*n+n-2] + m1.data[j*n+i] * m2.data[j*n+n-2] );
			}
			for (j = 0; j < n-2; ++j) { // sym_ji
				for (i = 0; i < j; ++i)
					data[i*n+n-1] = ( data[i*n+n-1] + m1.data[i*n+j] * m2.data[j*n+n-1] );
				for (i = j; i < n-2; ++i) 
					data[i*n+n-1] = ( data[i*n+n-1] + m1.data[j*n+i] * m2.data[j*n+n-1] );
			}
			for (i = 0; i < n-2; ++i) { // sym_ji
				data[i*n+n-2] = ( data[i*n+n-2] + m1.data[i*n+n-2] * m2.data[(n-2)*n+n-1] );
				data[i*n+n-1] = ( data[i*n+n-1] + m1.data[i*n+n-1] * m2.data[(n-1)*n+n-1] );
			}

			data[n*(n-2)+n-2] = 0;
			data[n*(n-1)+n-1] = data[n*(n-2)+n-1] = 1;

			for (i = 0; i < n; ++i)
				for (j = i; j < n; ++j)
					data[i*n+j] %= MOD;
		}
		void print(void) const {
			for (u32 i = 0; i < n; ++i) {
				for (u32 j = 0; j < n; ++j) 
					printf("%llu ", data[i*n+j]);
				printf("\n");
			}
			printf("---\n");
		}
};

class matrix1N {
	private:
		u64 data[MAX];

	public:
		matrix1N(void) { bzero(data, sizeof(u64)*n); }
		matrix1N(const matrix1N &m1n) { memcpy(data, m1n.data, sizeof(u64)*n); }
		~matrix1N(void) { }

		u64 &operator[](u32 i) { return data[i]; }
		const u64 &operator[](u32 i) const { return data[i]; }
		matrix1N operator*(const matrixNN &m) {
			matrix1N x;
			x.opt(*this, m);
			return x;
		}
		void opt(const matrix1N &m1n, const matrixNN &mnn) { 
			register u32 i; u32 j; // Mnozymy przez prawie symetryczna macierz.
			for (j = 0; j < n-2; ++j) { // ji
				for (i = 0; i < j; ++i) 
					data[i] = ( data[i] + m1n.data[j] * mnn[i][j] );
				for (i = j; i < n-2; ++i)
					data[i] = ( data[i] + m1n.data[j] * mnn[j][i] );
			}
			for (i = 0; i < n-2; ++i)
				data[n-2] = ( data[n-2] + m1n.data[i] * mnn[i][n-2] );
			for (i = 0; i < n; ++i) 
				data[n-1] = ( data[n-1] + m1n.data[i] * mnn[i][n-1] );

			for (i = 0; i < n; ++i)
				data[i] %= MOD;
		}
};

matrixNN g[POWER]; // Policzone grafy G^(2^0), G^(2^1), G^(2^2), ... G^(2^29).

void pre(void) { //-processing
	for (u32 i = 1; i < POWER; ++i)
		g[i].ikj(g[i-1],g[i-1]);
}

void ask(u32 k) {
	matrix1N v;
	u32 bit = 0;
	u32 i;
	v[0] = 1;

	// Interesujacy jest tylko 1 wiersz pierwszej z wymnarzanych macierzy.
	// rownowazne przemnozeniu: [1,0,0,...,0] x Macierz.
	while (k) {
		if (k & 1) {
			for (i = 0; i < n; ++i)
				v[i] = g[bit][0][i];
			++bit;
			k >>= 1;
			break;
		}
		++bit;
		k >>= 1;
	}

	// Dalej juz mnozymy wektor x macierz
	while (k) {
		if (k & 1) v = v*g[bit];
		++bit;
		k >>= 1;
	}

	// Obliczamy sume wartosci elementow wektora.
	u64 sum = 0;
	for (i = 0; i < n; ++i)
		sum = ( sum + v[i] ) % MOD;

	printf("%llu\n", sum);
}

int main(void) {
	u32 a, b, t, T;
	scanf("%lu%lu%lu", &n, &m, &T);
	++n;

	for (u32 i = 0; i < m; ++i) {
		scanf("%lu%lu", &a, &b);
		// Z ostatniego wierzcholka mamy juz nigdzie nie pojsc.
		if (a != n-1) g[0][a-1][b-1] = 1;
		if (b != n-1) g[0][b-1][a-1] = 1;
	}
	// Rozszerzamy nasz graf.
	for (u32 i = 0; i < n; ++i) 
		g[0][i][n-1] = 1;
	
	pre(); //-processing...

	while (T--) {
		scanf("%lu", &t);
		ask(t);
	}

	return 0;
}
