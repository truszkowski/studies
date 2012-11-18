#include <iostream>
#include <cstdlib>
#include <cmath>
using namespace std;

double ocena(double punkty) {
	const double wynik = (punkty+4)/3;
	return wynik<3 ? 2 : min(5., floor(2*wynik)/2);
}

int main(int argc, char *argv[]) {
	if (argc == 2) cout << ocena(atof(argv[1])) << endl;
	else cerr << argv[0] << " <punkty>\n";

	return 0;
}

