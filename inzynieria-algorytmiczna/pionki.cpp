#include <cstdio>

int main(void) {
	unsigned p, n;
	scanf("%u", &p);
	while (p--) {
		scanf("%u", &n);
		n %= 34;
		printf("%s\n", (n == 0 || n == 4 || n == 8 || n == 14 || 
					n == 20 || n == 24 || n == 28) ? "C" : "B");
	}
	return 0;
}

