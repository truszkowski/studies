#ifndef __METODY_ITERACYJNE_PT209223_H__
#define __METODY_ITERACYJNE_PT209223_H__

extern void tic         (void);
extern unsigned int toc (void);

extern void Jacobi      (double * b, double alfa, double * x, unsigned int N);
extern void Richardson  (double * b, double alfa, double * x, unsigned int N);
extern void CG          (double * b, double alfa, double * x, unsigned int N);
extern void testall     (double alfa, int N);

#endif
