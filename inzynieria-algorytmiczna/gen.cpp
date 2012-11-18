#include <cstdio>
#include <cstdlib>
#include <ctime>
typedef unsigned long int u32;
const u32 MAX = 50000, M = 500000;

int main(void) {
	srand(time(0));

	printf("%lu\n", MAX);
	for (u32 i = 0; i < MAX; ++i) {
		u32 a = rand()%M, b = rand()%M, c = rand()%M, d = rand()%M;
		while (a >= c) c = 1+rand()%M;
		while (b >= d) d = 1+rand()%M;

		printf("%lu %lu %lu %lu\n", a, b, c, d);
	}

	return 0;
}
