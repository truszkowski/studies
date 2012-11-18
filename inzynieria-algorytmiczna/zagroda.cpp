#include <algorithm>
#include <cstdio>
#include <cmath>

const unsigned MAX = 10000;
const long double eps = 1.0E-12;
const long double eps2 = 1.0E-9;

unsigned n; // 3 <= n <= 10000 // 
long double d[MAX]; // 1 <= d[i] <= 1000 // 
long double sum = 0;

long double fun(const long double &r) {
	long double ang = 0;
	for (unsigned i = 0; i < n; ++i) ang += asin(5.0E-1*d[i]/r);
	return M_PI - ang;
}

long double gun(const long double &r) {
	long double ang = 0;
	for (unsigned i = 0; i < n-1; ++i) ang += asin(5.0E-1*d[i]/r);
	return ang - asin(5.0E-1*d[n-1]/r);
}

long double binary_search(const long double &min, const long double &max, long double (*fn)(const long double&)) {
	if (max-min < eps) return 5.0E-1*(min+max);
	long double x = 5.0E-1*(min+max);
	long double y = fn(x);
	if (y == 0) return x;
	return y < 0 ? binary_search(x, max, fn) : binary_search(min, x, fn);
}

int main(void) {
	scanf("%u", &n);
	for (unsigned i = 0; i < n; ++i) scanf("%Lf", d+i);
	for (unsigned i = 0; i < n; ++i) sum += d[i];

	std::sort(d, d+n);

	if (sum <= 2*d[n-1]) printf("NIE\n");
	else {
		long double r, wizard = fun(5.0E-1*d[n-1]);

		if (fabs(wizard) < eps2) r = 5.0E-1*d[n-1];
		else if (wizard < 0) r = binary_search(5.0E-1*d[n-1], sum*5.0E-1, fun);
		else {
			r = binary_search(5.0E-1*d[n-1], 1.0E5, gun);
			d[n-1] *= -1;
		}

		long double out = 0;
		for (unsigned i = 0; i < n; ++i)
			out += sqrt(r*r - d[i]*d[i]*2.5E-1)*d[i]*5.0E-1;

		long double best = floor((out+eps2)*1.0E1)*1.0E-1;

		bool organki = d[n-1]<0;
		if (organki) d[n-1] *= -1;
		printf("r: %.20Lf, p:%.20Lf\n", r, out);
		printf("%s: %.20Lf\n", organki ? "gun":"fun", organki ? gun(r):fun(r));

		printf("%.1Lf\n", best);
	}

	return 0;
}

