#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include "method.h"

int MAXITER    = 10000000; /* 10mln */
int MAXTIME    = 3000000; /* 50 min */
double EPSILON = 0.000000000001; /* 1/10^12 */
double EPSILON2= 0.000000000000000000000001; /* 1/10^24 */
double PI      = 3.1415926535897932384626433832795028841971693993751058209749445923078164;
double * good = NULL;

static unsigned int clock_sec = 0;
static unsigned int clock_usec = 0;

static inline double SQUARE(double x) { return x*x; }

static inline int STOP(double alfa, double * x, double * y, unsigned int n, int it, unsigned int t, int ok) {
	int i;
	double dif = 0;
	
	if ( good ) { /* gdy ustawione good, to testujemy czy x ~ good, */
		for ( i = 1; i < n ; ++i )
			dif += SQUARE(x[i] - good[i]);
	} else { /* wpp badamy residuum(nieznamy dokladnego rozwiazania), */
		dif += SQUARE(alfa*x[0] - x[1] - y[0]);
		for ( i = 1; i < n - 1; ++i )
			dif += SQUARE(alfa*x[i] - x[i-1] - x[i+1] - y[i]);
		dif += SQUARE(alfa*x[n-1] - x[n-2] - y[n-1]);
	}
	
	printf("i:%d t:%d e:%e                              \r", it, t/1000, fabs(dif));
	
	if ( ok || ((fabs(dif)) < EPSILON) || ( it > MAXITER ) || ( t / 1000 > MAXTIME ) )
		printf( "\nResults:\n Size = %d\n Time = %d\n Iter = %d\n Erro = %e\n", n, t, it, dif);
	else 
		return 0;
	return 1;
}

/* mierzenie czasu */
void tic(void) {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	clock_sec = tv.tv_sec;
	clock_usec = tv.tv_usec;
}

unsigned int toc(void) {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	return ( ( tv.tv_sec - clock_sec ) * 1000000 + ( tv.tv_usec - clock_usec ) );
}

void Jacobi(double * b, double a, double * x, unsigned int n) {
/* x(k+1) = M N x(k) - M b */
	double lambda = ( a + 2.0*cos(PI / (((double) n)+1.0)) );
	double px, tx;
	double eta = ( a - lambda );
	unsigned int i;
	int iterations = 0;
	bzero( x, sizeof(double)*n );

	tic(); 

	while ( ++iterations ) {
		px = x[0];
		x[0] = -( eta*x[0] - x[1] - b[0] ) / lambda;
		for ( i = 1; i < n - 1; ++i )	{
			tx = x[i];
			x[i] = -( eta*tx - px - x[i+1] - b[i] ) / lambda;
			px = tx;
		}
		x[n-1] = -( eta*x[n-1] - px - b[n-1] ) / lambda;

		if ( STOP(a, x, b, n, iterations, toc(), 0) )
			break;
	}
}

void Richardson(double * b, double a, double * x, unsigned int n) {
/*	double tau_opt = 2 / ( lambda_min + mlabda_max ) */
	double tau = ( ( 1.0 ) / (a) );
	double px, tx;
	int i, iterations = 0;
	bzero(x, sizeof(double)*n);
	
	tic();

	while ( ++iterations ) {
		px = x[0];
		x[0] = x[0] - tau*(a*x[0] - x[1] - b[0]);
		for ( i = 1; i < n - 1; ++i ) {
			tx = x[i];
			x[i] = tx - tau*(a*tx - x[i+1] - px - b[i]);
			px = tx;
		}
		x[n-1] = x[n-1] - tau*(a*x[n-1] - px - b[n-1]);
		
		if ( STOP(a, x, b, n, iterations, toc(), 0) )
			break;
	}
}

/* metoda CG */
void CG(double * b, double a, double * x, unsigned int n) {
	double * r, * p, * w, ro1, ro2, beta, alfa;
	int i, iterations;
	r = malloc(sizeof(double)*n);
	p = malloc(sizeof(double)*n);
	w = malloc(sizeof(double)*n);
	bzero(x, sizeof(double)*n);
	
	tic();

	r[0] = b[0] + a*x[0] - x[1];
	for ( i = 1; i < n - 1; ++i )
		r[i] = b[i] + a*x[i] - x[i+1] - x[i-1];
	r[n-1] = b[n-1] + a*x[n-1] - x[n-2];
	ro1 = 0;
	for ( i = 0; i < n; ++i )
		ro1 += r[i]*r[i];
	beta = 0;
	iterations = 0;

	while ( ++iterations ) {
		for ( i = 0; i < n; ++i )
			p[i] = r[i] + beta*p[i];
		w[0] = a*p[0] - p[1];
		for ( i = 1; i < n - 1; ++i )
			w[i] = a*p[i] - p[i+1] - p[i-1];
		w[n-1] = a*p[n-1] - p[n-2];
		alfa = 0;
		for ( i = 0; i < n; ++i )
			alfa += p[i]*w[i];
		alfa = ro1 / alfa;
		for ( i = 0; i < n; ++i )
			x[i] = x[i] + alfa*p[i];
		for ( i = 0; i < n; ++i )
			r[i] = r[i] - alfa*w[i];
		ro2 = 0;
		for ( i = 0; i < n; ++i )
			ro2 += r[i]*r[i];
		if ( ro1 <= EPSILON2 ) { 
			printf("\n| residuum | < epsilon = %e!\n", EPSILON2);
			STOP(a, x, b, n, iterations, toc(), 1);
			break; /* gdy wyniki bardzo dokladne, jest dzielenie przez 0 */
		}
		beta = ro2 / ro1;
		ro1 = ro2;
		
		if ( STOP(a, x, b, n, iterations, toc(), 0) )
			break;
	}

	free(r);
	free(p);
	free(w);
}

/* test */
void testall(double alfa, int LENGTH) {
	struct timeval tv;
	struct timezone tz;
	double * x = NULL, * y = NULL, * z = NULL;
	int i;

	y = malloc(sizeof(double)*LENGTH);
	z = malloc(sizeof(double)*LENGTH);
	
	gettimeofday(&tv, &tz);
	srand(tv.tv_usec);

	x = malloc(sizeof(double)*LENGTH);

	for ( i = 0; i < LENGTH; ++i )
		x[i] = ((double) ( (rand() % 1000000) - 500000 )) / 1024;
	
	y[0] = alfa*x[0] - x[1];
	for ( i = 1; i < LENGTH - 1; ++i )
		y[i] = alfa*x[i] - x[i+1] - x[i-1];
	y[LENGTH-1] = alfa*x[LENGTH-1] - x[LENGTH-2];
	
	good = x;
	printf("CG: \n");
	CG(y, alfa, z, LENGTH);
	printf("Richardson: \n");
	Richardson(y, alfa, z, LENGTH);
	printf("Jacobi: \n");
	Jacobi(y, alfa, z, LENGTH);
	good = NULL;
	
	free(x);
	free(y);
	free(z);
}
