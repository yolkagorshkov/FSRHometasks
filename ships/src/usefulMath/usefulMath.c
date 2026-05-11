#include "usefulMath.h"
#include <math.h>
#include <stdlib.h>


double sqr(double x) {return x * x;}
double qud(double x) {return x * x * x * x;}

double smoothstep3(double t) {
    if (t <= 0) return 0;
    if (t >= 1) return 1;
    return sqr(t) * (3 - 2 * t);
}

double smoothstep5(double t) {
    if (t <= 0) return 0;
    if (t >= 1) return 1;
    return t * sqr(t) * (6 * sqr(t) - 15 * t + 10);
}

double p(double t) {
    return t * (2 - t);
}

double Gauss(int x, int y, double sigma, double lambda) {
    return exp(-((sqr(x) + sqr(y)) / sqr(sigma))) * lambda / (2 * M_PI * sqr(sigma));
}

double LoG(int x, int y, double sigma, double lambda) {
    return lambda / (M_PI * qud(sigma)) * (1 - (sqr(x) + sqr(y)) / sqr(sigma)) * exp(-((sqr(x) + sqr(y)) / sqr(sigma)));
}

int LoGMatrix(int r, double sigma, double lambda, double*** mtx) {
    int d = ((r << 1) | 1);
    *mtx = (double**)malloc(d * sizeof(double*));
    if (!(*mtx)) return 1;
    for (int i = 0; i < d; i++) (*mtx)[i] = (double*)malloc(d * sizeof(double));
    for (int i = 0; i < d; i++) {
        for (int j = 0; j < d; j++) {
            int x = j - r;
            int y = r - i;
            (*mtx)[i][j] = LoG(x, y, sigma, lambda);
        }
    }
    return 0;
}

int GaussMatrix(int r, double sigma, double lambda, double*** mtx) {
    int d = ((r << 1) | 1);
    *mtx = (double**)malloc(d * sizeof(double*));
    if (!(*mtx)) return 1;
    for (int i = 0; i < d; i++) (*mtx)[i] = (double*)malloc(d * sizeof(double));
    for (int i = 0; i < d; i++) {
        for (int j = 0; j < d; j++) {
            int x = j - r;
            int y = r - i;
            (*mtx)[i][j] = Gauss(x, y, sigma, lambda);
        }
    }
    return 0;
}

void normalizeMatrix(double*** mtx, int n) {
    double divisor = 0;
    for (int i = 0; i < n * n; i++) divisor += ((*mtx)[i/n][i%n] > 0) ? (*mtx)[i/n][i%n] : 0;
    for (int i = 0; i < n * n; i++) (*mtx)[i/n][i%n] /= divisor;
}

void freeMtx(int n, void*** mtx) {
    for (int i = 0; i < n; i++) free((*mtx)[i]);
    free(*mtx);
}