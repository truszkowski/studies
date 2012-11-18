#include <cstdio>
#include <cstring>
typedef unsigned short int u16;
typedef unsigned long int u32;

const u32 MAX = 16;
u32 n, m, W; // max: 16, 200, 1000 //
u32 w[MAX];
u16 e[MAX];
bool map[0xffff+1];
u16 fifol[0xffff+1];
u16 fifor[0xffff+1];
u32 depth[0xffff+1];
u32 fstart = 0, fend = 0;
u32 out = 0;
bool can[0xffff+1];

void gen(u16 all) {
	u16 sub = all;
	while (sub) {
		u32 i = 0, sum = 0;
		u16 t = sub;
		while (t) {
			if (t & 1) {
				sum += w[i];
				if (e[i] & sub) break;
			}
			t >>= 1;
			++i;
		}
		if (!t) can[sub] = (sum <= W);

		--sub;
		sub &= all;
	}
}

void rec(void) {
	// LEFT, RIGHT, DEPTH.
	u16 left = fifol[fstart], right = fifor[fstart];
	u32 d = depth[fifol[fstart]];
	if (d & 1) { 
//		printf("d = %lu\n", depth[fifol[fstart]]);
	}
	++fstart;
	
	u16 sub = left, left2, right2;
	// left != 0 //

	// Dla kazdego podzbioru sub zbioru left,
	while (sub) {
		// ktory jest poprawny,
		if (can[sub]) {
			// przenosimy do zbioru right.
			left2 = left & (~sub);
			right2 = right | sub;

			// Czy konczymy?
			if (!left2) {
				out = depth[0] = d+1;
				fstart = fend; // THROW
				return;
			}

			// Jednym z elementow z right wraca do left
			for (u32 i = 0; i < 16; ++i)
				if (!map[left2 ^ (1 << i)] && (right2 & (1 << i))) {
					map[left2 ^ (1 << i)] = true;
					fifol[fend] = left2 ^ (1 << i);
					fifor[fend] = right2 ^ (1 << i);
					depth[fifol[fend]] = d + 2;
					++fend;
				}
		}

		--sub;
		sub &= left;
	}
}

int main(void) {
	scanf("%lu%lu%lu", &n, &m, &W);

	for (u32 i = 0; i < n; ++i) scanf("%lu", w+i);
	for (u32 i = 0; i < m; ++i) {
		u32 a, b;
		scanf("%lu%lu", &a, &b);
		e[a-1] |= 1 << (b - 1);
		e[b-1] |= 1 << (a - 1);
	}

	fifol[fend] = 0xffff >> (16-n);
	fifor[fend] = 0x0;
	depth[fifol[fend]] = 0;
	map[fifol[fend]] = true;
	++fend;

	gen(fifol[0]);

	while (fstart < fend) rec();

	if (out) printf("%lu\n", depth[0]); else printf("NIE\n");

	return 0;
}


