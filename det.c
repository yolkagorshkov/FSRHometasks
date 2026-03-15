#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef unsigned long long ull;
typedef long long ll;
typedef long double ld;

typedef struct matrix {
    int n;
    double** mtx;
} matrix;

matrix initMatrix(int n) {
    matrix m;
    m.n = n;
    m.mtx = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) m.mtx[i] = (double*)malloc(n * sizeof(double));
    return m;
}

void freeMatrix(matrix m) {
    for (int i = 0; i < m.n; i++) free(m.mtx[i]);
    free(m.mtx);
}

void swapRows(matrix m, int i, int j) {
    if (i == j) return;
    double* temp = m.mtx[i];
    m.mtx[i] = m.mtx[j];
    m.mtx[j] = temp;
}

int Gauss(matrix m) {
    int swaps = 0;
    int n = m.n;

    for (int i = 0; i < n; i++) {
        int maxRow = i;
        double maxVal = fabs(m.mtx[i][i]);
        
        for (int k = i + 1; k < n; k++) {
            if (fabs(m.mtx[k][i]) > maxVal) {
                maxVal = fabs(m.mtx[k][i]);
                maxRow = k;
            }
        }

        if (maxRow != i) {
            swapRows(m, i, maxRow);
            swaps++;
        }

        if (fabs(m.mtx[i][i]) < 1e-12) continue; 

        for (int j = i + 1; j < n; j++) {
            double lambda = m.mtx[j][i] / m.mtx[i][i];
            
            for (int k = i; k < n; k++) {
                m.mtx[j][k] -= lambda * m.mtx[i][k];
            }
        }
    }
    
    return swaps;
}

double Det(matrix m) {
    
    matrix copy = initMatrix(m.n);
    for (int i = 0; i < m.n; i++) {
        for (int j = 0; j < m.n; j++) {
            copy.mtx[i][j] = m.mtx[i][j];
        }
    }

    int swaps = Gauss(copy);
    double det = 1.0;

    for (int i = 0; i < copy.n; i++) {
        if (fabs(copy.mtx[i][i]) < 1e-12) return 0.0;
        det *= copy.mtx[i][i];
    }

    det = ((swaps & 1) ? -1 : 1) * det;

    freeMatrix(copy);
    return det;
}

int main() {
    int n = 4;
    matrix m = initMatrix(n);

    double mtx[4][4] = {
        {0.0,  2.0,  3.0,  4.0},
        {1.0,  0.0,  8.0,  3.0},
        {2.0, -1.0,  0.0,  1.0},
        {3.0,  1.0, -2.0,  0.0}
    };

    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) m.mtx[i][j] = mtx[i][j];

    double determinant = Det(m);
    
    printf("%lf", determinant);

    freeMatrix(m);

}
