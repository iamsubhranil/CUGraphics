#ifdef DEBUG
#include "display.h"
#endif
#include "matrix.h"

#include <malloc.h>
#include <memory.h>
#include <stdarg.h>

typedef struct Mat{
    double **values;
    int m, n;
} Mat;

Mat* mat_new(int m, int n){
    Mat *mt = (Mat *)malloc(sizeof(Mat));
    mt->m = m;
    mt->n = n;
    mt->values = (double **)malloc(sizeof(double *) * m);
    for(int i = 0;i < m;i++){
        mt->values[i] = (double *)malloc(sizeof(double) * n);
        //memset(mt->values[i], 0, sizeof(double) * n);
    }
    return mt;
}

void mat_fill(Mat *m1, ...){
    va_list args;
    va_start(args, m1);
    for(int i = 0;i < m1->m;i++){
        for(int j = 0;j < m1->n;j++){
            m1->values[i][j] = va_arg(args, double);
        }
    }
    va_end(args);
}

void mat_set(Mat *m1, int i, int j, double val){
    m1->values[i][j] = val;
}

double mat_get(Mat *m1, int i, int j){
    return m1->values[i][j];
}

void mat_print(Mat *m1){
    printf("\n");
    for(int i = 0;i < m1->m;i++){
        for(int j = 0;j < m1->n;j++){
            printf("%3.5g", m1->values[i][j]);
        }
        printf("\n");
    }
}

Mat* mat_mult(Mat *m1, Mat *m2){
    if(m1->n != m2->m){
#ifdef DEBUG
        perr("Multiplying matrix with different dimensions : (%d x %d) and (%d x %d)!",
                m1->m, m1->n, m2->m, m2->n);
#endif
        return NULL;
    }
    Mat* res = mat_new(m1->m, m2->n);
    // Row loop
    for(int i = 0;i < res->m;i++){
        // Column loop
        for(int j = 0;j < res->n;j++){
            //printf("\n%d", j);
            //fflush(stdout);
            double sum = 0;
            for(int k = 0;k < m1->n;k++){
                sum += (m1->values[i][k] * m2->values[k][j]);
            }
            res->values[i][j] = sum;
        }
    }
    return res;
}

Mat* mat_add(Mat *m1, Mat *m2){
    if(m1->m != m2->m || m1->n != m2->n){
#ifdef DEBUG
        perr("Adding matrix with different dimensions : (%d x %d) and (%d x %d)!",
                m1->m, m1->n, m2->m, m2->n);
#endif
        return NULL;
    }
    Mat* res = mat_new(m1->m, m1->n);
    for(int i = 0;i < m1->m;i++){
        for(int j = 0;j < m1->n;j++){
            res->values[i][j] = m1->values[i][j] + m2->values[i][j];
        }
    }
    return res;
}

Mat* mat_sub(Mat *m1, Mat *m2){
    if(m1->m != m2->m || m1->n != m2->n){
#ifdef DEBUG
        perr("Subtracting matrix with different dimensions : (%d x %d) and (%d x %d)!",
                m1->m, m1->n, m2->m, m2->n);
#endif
        return NULL;
    }
    Mat* res = mat_new(m1->m, m1->n);
    for(int i = 0;i < m1->m;i++){
        for(int j = 0;j < m1->n;j++){
            res->values[i][j] = m1->values[i][j] - m2->values[i][j];
        }
    }
    return res;
}

void mat_free(Mat *m1){
    for(int i = 0;i < m1->m;i++)
        free(m1->values[i]);
    free(m1->values);
    free(m1);
}
