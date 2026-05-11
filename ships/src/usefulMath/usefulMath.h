#ifndef USEFULMATH_H
#define USEFULMATH_H

#include <stdio.h>
#define M_PI 3.14159265358979323846

double sqr(double);
double qud(double);
double smoothstep3(double);
double smoothstep5(double);
double p(double t);
double Gauss(int x, int y, double sigma, double lambda);
double LoG(int x, int y, double sigma, double lambda);
int LoGMatrix(int r, double sigma, double lambda, double*** mtx);
int GaussMatrix(int r, double sigma, double lambda, double*** mtx);
void normalizeMatrix(double*** mtx, int n);
void freeMtx(int n, void*** mtx);

#endif