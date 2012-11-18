#include <cstdio>
const unsigned MAX = 40;

unsigned i, j;
unsigned h, w, m;
unsigned map[MAX][MAX];
unsigned a, b, c, d, e;

__inline__ const short &max2(const short &x, const short &y) {
	return x <= y ? y:x;
}
__inline__ const short &max3(const short &x, const short &y, const short &z) {
	return x <= y ? (y <= z ? z:y) : (x <= z ? z:x);
}
__inline__ const short &max4(const short &x, const short &y, const short &z, const short &t) {
	return x <= y ? (y <= z ? (z <= t ? t:z):(y <= t ? t:y)):(x <= z ? (z <= t ? t:z):(x <= t ? t:x));
}
__inline__ const short &max5(const short &x, const short &y, const short &z, const short &t, const short &u) {
	return x <= y ? 
		(y <= z ? (z <= t ? (t <= u ? u:t):(z <= u ? u:z)):(y <= t ? (t <= u ? u:t):(y <= u ? u:y)))
		:
		(x <= z ? (z <= t ? (t <= u ? u:t):(z <= u ? u:z)):(x <= t ? (t <= u ? u:t):(x <= u ? u:x)));
}

struct V {
	short v[5*MAX*MAX*MAX*MAX], zero;

	short &operator()(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e) {
		if (a > h || b > w || c >= b || d >= c || e >= d || a < 0 || b < 0 || c < 0 || d < 0 || e < 0) return zero;
		return v[MAX*MAX*MAX*MAX*(a%5) + MAX*MAX*MAX*b + MAX*MAX*c + MAX*d + e];
	}
	short &operator()(unsigned a, unsigned b, unsigned c, unsigned d) {
		if (a > h || b > w || c >= b || d >= c || a < 0 || b < 0 || c < 0 || d < 0) return zero;
		return v[MAX*MAX*MAX*a + MAX*MAX*b + MAX*c + d];
	}
	short &operator()(unsigned a, unsigned b, unsigned c) {
		if (a > h || b > w || c >= b || a < 0 || b < 0 || c < 0) return zero;
		return v[MAX*MAX*a + MAX*b + c];
	}
	short &operator()(unsigned a, unsigned b) {
		if (a > h || b > w || a < 0 || b < 0) return zero;
		return v[MAX*a + b];
	}
};

V v;

void do1(void) {
	for (a = 0; a < h; ++a)
		for (b = 0; b < w; ++b)
			v(a,b) = max2(v(a-1,b), v(a,b-1)) + map[a][b];

	printf("%u\n", v(h-1,w-1));
}

void do2(void) {
	for (a = 0; a < h-1; ++a)
		for (b = 0; b < w; ++b)
			for (c = 0; c < b; ++c)
				v(a,b,c) = max3(
						v(a-1,b,c) + map[a][b] + map[a+1][c],
						v(a,b,c-1) + map[a+1][c],
						v(a,b-1,c) + map[a][b]);	

	printf("%u\n", v(h-2,w-1,w-2) + 
			map[0][0] +
			map[h-1][w-1]);
}

void do3(void) {
	for (a = 0; a < h-2; ++a)
		for (b = 0; b < w; ++b)
			for (c = 0; c < b; ++c)
				for (d = 0; d < c; ++d)
					v(a,b,c,d) = max4(
							v(a-1,b,c,d) + map[a][b] + map[a+1][c] + map[a+2][d],
							v(a,b-1,c,d) + map[a][b],
							v(a,b,c-1,d) + map[a+1][c],
							v(a,b,c,d-1) + map[a+2][d]);

	printf("%u\n", v(h-3,w-1,w-2,w-3) + 
			map[0][0] + map[0][1] + map[1][0] +
			map[h-1][w-1] + map[h-1][w-2] + map[h-2][w-1]);
}

void do4(void) {
	for (a = 0; a < h-3; ++a)
		for (b = 0; b < w; ++b)
			for (c = 0; c < b; ++c)
				for (d = 0; d < c; ++d)
					for (e = 0; e < d; ++e)
						v(a,b,c,d,e) = max5(
								v(a-1,b,c,d,e) + map[a][b] + map[a+1][c] + map[a+2][d] + map[a+3][e],
								v(a,b-1,c,d,e) + map[a][b],
								v(a,b,c-1,d,e) + map[a+1][c],
								v(a,b,c,d-1,e) + map[a+2][d],
								v(a,b,c,d,e-1) + map[a+3][e]);

	printf("%u\n", v(h-4,w-1,w-2,w-3,w-4) + 
			map[0][0] + map[0][1] + map[0][2] + map[1][1] + map[1][0] + map[2][0] + 
			map[h-1][w-1] + map[h-1][w-2] + map[h-1][w-3] + map[h-2][w-2] + map[h-2][w-1] + map[h-3][w-1]);
}

int main(void) {
	scanf("%u%u%u", &h, &w, &m);

	for (i = 0; i < h; ++i)
		for (j = 0; j < w; ++j)
			scanf("%u", &map[i][j]);

	if (w <= m || h <= m) {
		short s = 0;
		for (a = 0; a < h; ++a)
			for (b = 0; b < w; ++b) 
				s += map[a][b];

		printf("%u\n", s);
	} else
		switch (m) {
			case 4: do4(); break;
			case 3: do3(); break;
			case 2: do2(); break;
			case 1: do1(); break;
		}

	return 0;
}
