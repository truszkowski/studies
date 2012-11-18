int Pow(int a, int n) {
	int r = 1;
	while (n-- > 0)
		r *= a;
	return r;
}


#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_linalg.h>
#define PI M_PI
#define MAXLEN 1111111
#define LIMIT MAXLEN
#define KEY 6

double EPSILON, N, K, h, M;
gsl_vector * F, * G1, * G2, * X;
gsl_function fun;
gsl_integration_workspace *workspace;
gsl_integration_qawo_table * table;

inline double fi(int i, double x) { // funkcja fi 
	return ( x >= (i-1)*h && x < i*h ? x/h - i + 1 :
		( x >= i*h && x <= (i+1)*h ? -x/h + i + 1 : 0 ) );
}
inline double fip(int i, double x) { // pochodna fi
	return ( x >= (i-1)*h && x < i*h ? 1.0/h :
		( x >= i*h && x <= (i+1)*h ? -1.0/h : 0.0 ) );
}
int fisign; // znak fip, okreslany zewnetrznie,
inline double fipp(int i, double x) { // pochodna fi
	return ( x >= (i-1)*h && x < i*h ? fisign*1.0/h :      //  1 // potem calkujemy tak, ze i 
		( x >= i*h && x <= (i+1)*h ? fisign*1.0/h : 0.0 ) ); // -1 // tak wiemy z jakim znakiem,
}
inline double f1(double x) { // pierwsza f
	return x*(x-2*K*PI)*exp(-x*x);
}
inline double f1p(double x) { // pochodna pierwszej f
	return 2*exp(-x*x)*(-x*x*x + 2*K*PI*x*x + x - K*PI);
}
inline double f2(double x) { // druga f
	return sin(K*PI*x)*exp(-x*x);
}
inline double f2p(double x) { // pochodna drugiej f
	return exp(-x*x)*(K*PI*cos(K*PI*x) - 2*x*sin(K*PI*x));
}
inline double f2p_sin(double x) { // pochodna drugiej f
	return -exp(-x*x)*2*x; // *sin(K*PI*x);
}
inline double f2p_cos(double x) { // pochodna drugiej f
	return exp(-x*x)*K*PI; // *cos(K*PI*x)
}
int f1qi = 0, f2qi = 0; // indeksy dla fi
inline double f1q(double x, void * p) { // funkcja podcalkowa dla pierwszej f
	return x*f1p(x)*fipp(f1qi, x);
}
inline double f2q(double x, void * p) { // funkcja podcalkowa dla drugiej f
	return x*f2p(x)*fipp(f2qi, x);
}
inline double f2q_sin(double x, void * p) { // funkcja podcalkowa dla drugiej f
	return x*f2p_sin(x)*fipp(f2qi, x); // sam sin
}
inline double f2q_cos(double x, void * p) { // funkcja podcalkowa dla drugiej f
	return x*f2p_cos(x)*fipp(f2qi, x); // sam cos
}


//  M A I N  R E C T  //


int main_rect() {

	double result1=0, result2=0;
	
	F  = gsl_vector_alloc((int)N);   // Macierz G jest trojdiagonalna symetryczna
	G1 = gsl_vector_alloc((int)N);   // wiec mozna ja opisac jedynie poprzez 2
	G2 = gsl_vector_alloc((int)N-1); // wektory- diagonali i poddiagonali.
	X  = gsl_vector_alloc((int)N);   // 
	
	for (int i = 0; i < N; ++i)
		gsl_vector_set(G1, i, 2*(i-1.0)/h);
	for (int i = 0; i < N-1; ++i)
		gsl_vector_set(G2, i, -(2.0*i+1.0)/2.0/h);
	
	workspace = gsl_integration_workspace_alloc(LIMIT);
	
	for (f1qi = 1; f1qi <= N; ++f1qi) { // na dwie tury,
		fisign = 1;	
		if (f1qi < 2) {
			result1 = 0;
			for (double i = ((double)f1qi-1.0)*h + h/M/2.0; i < ((double)f1qi)*h; i += h/M)
				result1 += f1q(i,0)*h/M;
		} else result1 = -result2;
		gsl_vector_set(F, f1qi-1, result1);
	
		fisign = -1;
		result2 = 0;
		for (double i = ((double)f1qi)*h + h/M/2.0; i < ((double)f1qi+1)*h; i += h/M)
			result2 += f1q(i,0)*h/M;
		gsl_vector_set(F, f1qi-1, result1 +result2);
		if( f1qi % 1000 == 0 ) { printf("%d  \r", f2qi); fflush(stdout); }
	}
	
	fprintf(stderr,"1 qad done.\n");
	gsl_linalg_solve_symm_tridiag(G1,G2,F,X);
//	for (int i = 0; i < N; ++i)
//		fprintf(stderr,"h[%d] = %e\n",i,gsl_vector_get(X,i));
	fprintf(stderr,"1 linalg done.\n");

	gsl_integration_workspace_free(workspace);
	gsl_vector_free(F);
	gsl_vector_free(G1);
	gsl_vector_free(G2);
	gsl_vector_free(X);	
	return 0;
}


//  M A I N  //


int main(int argc, char * argv[]) {

	if (argc < 4) {
		fprintf(stderr, "./%s <N> <K> <epsilon> {<M>}\n", argv[0]);
		return 1;
	} 
	
	N = atof(argv[1]);
	K = atof(argv[2]);
	EPSILON = atof(argv[3]);
	h = 2*PI/(N+1);
	
	M = 1;
	if (argc > 4)
		M = atof(argv[4]);
		
	main_rect();
	
	return 0;
}
