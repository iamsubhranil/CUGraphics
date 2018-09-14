#pragma once

typedef struct Mat* Matrix;

Matrix mat_new(int m, int n);
void mat_set(Matrix m, int i, int j, double val);
double mat_get(Matrix m, int i, int j);
void mat_fill(Matrix m, ...);
Matrix mat_mult(Matrix m1, Matrix m2);
Matrix mat_add(Matrix m1, Matrix m2);
Matrix mat_sub(Matrix m1, Matrix m2);
void mat_print(Matrix m1);
void mat_free(Matrix m);
