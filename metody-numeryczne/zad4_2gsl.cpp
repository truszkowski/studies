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


//  M A I N  G S L  Q U A D  //


int main_gsl_quad() {
// Na postawie N i K bede ustalal jak wiele moze byc przedzialów,
// Ustalilem recznie, na wyczucie.
// 

	int DEEP = (int)( log(2.0E7 / N) );
//	printf("%d\n", DEEP);
	int DLIMIT = Pow(2,DEEP);

	double result1, result2, diff1, diff2;
	double result3, result4, diff3, diff4;
	
	F  = gsl_vector_alloc((int)N);   // Macierz G jest trojdiagonalna symetryczna
	G1 = gsl_vector_alloc((int)N);   // wiec mozna ja opisac jedynie poprzez 2
	G2 = gsl_vector_alloc((int)N-1); // wektory- diagonali i poddiagonali.
	X  = gsl_vector_alloc((int)N);   // 
	
	for (int i = 0; i < N; ++i)
		gsl_vector_set(G1, i, 2*(i-1.0)/h);
	for (int i = 0; i < N-1; ++i)
		gsl_vector_set(G2, i, -(2.0*i+1.0)/2.0/h);
	
	workspace = gsl_integration_workspace_alloc(LIMIT);

	if ( K > 20 ) {
		DLIMIT = (int) log10(sqrt(5*K))*DLIMIT;
		DEEP = (int) log2(DLIMIT);
	}
	DLIMIT *= 4;
	DEEP *= 4;
	
	table = gsl_integration_qawo_table_alloc(K*PI, h, GSL_INTEG_SINE, DEEP);
	fun.function = &f2q_sin;
	
//	printf("%d %d\n", DEEP, DLIMIT);
	
// Mamy doczynienia z funkcja oscylujaca, uzyjmy wiec specjalnych
// kwadratur GSLa, oddzielnie dla sinus oddzielnie dla cosinus,
// 
	
	for (f2qi = 1; f2qi <= N; ++f2qi) { // sinus	
		fisign = 1;
		if ( f2qi < 2 ) // wartosci na siebie nachodza
			gsl_integration_qawo(&fun, (f2qi-1)*h, 0.0E0, EPSILON, DLIMIT, workspace, table, &result1, &diff1);
		else result1 = -result3;
		fisign = -1;
		gsl_integration_qawo(&fun, (f2qi  )*h, 0.0E0, EPSILON, DLIMIT, workspace, table, &result3, &diff3);

		gsl_vector_set(F, f2qi-1, result1+result3);
//		if( f2qi % 1000 == 0 ) { printf("%d  \r", f2qi); fflush(stdout); }
	}
		
	gsl_integration_qawo_table_free(table);
	table = gsl_integration_qawo_table_alloc(K*PI, h, GSL_INTEG_COSINE, DEEP);
	fun.function = &f2q_cos;
	
	for (f2qi = 1; f2qi <= N; ++f2qi) { // cosinus	
		fisign = 1;
		if ( f2qi < 2 ) // wartosci na siebie nachodza
			gsl_integration_qawo(&fun, (f2qi-1)*h, 0.0E0, EPSILON, DLIMIT, workspace, table, &result2, &diff2);
		else result2 = -result4;
		fisign = -1;
		gsl_integration_qawo(&fun, (f2qi  )*h, 0.0E0, EPSILON, DLIMIT, workspace, table, &result4, &diff4);	
		
		gsl_vector_set(F, f2qi-1, result1+result3+gsl_vector_get(F, f2qi-1));
//		if( f2qi % 1000 == 0 ) { printf("%d  \r", f2qi); fflush(stdout); }
	}
	
	gsl_integration_qawo_table_free(table);

// Mamy F wiec znajdujemy X (h*), rozwiazujac uklad Gh=F
// gdzie G symetryczna trojdiagonalna,
// 

//	fprintf(stderr,"2 quad done.\n");
	gsl_linalg_solve_symm_tridiag(G1,G2,F,X);
//	for (int i = 0; i < N; ++i)
//		fprintf(stderr,"h[%d] = %.15e\n",i,gsl_vector_get(X,i));
	fprintf(stderr,"2 linalg done.\n");
	
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
	
	main_gsl_quad();
	
	return 0;
}
