#pragma once

// Opaque matrix structure
typedef struct Mat* Matrix;

// Create a new (m x n) matrix
Matrix mat_new(int m, int n);
// Set the particular cell to the given value
void mat_set(Matrix m, int i, int j, double val);
// Get the value of a particular cell
double mat_get(Matrix m, int i, int j);
// Fill the matrix with values
// All (m x n) values MUST be present
void mat_fill(Matrix m, ...);
// Multiply two matrices and return the result in a new matrix
Matrix mat_mult(Matrix m1, Matrix m2);
// Add two matrices
Matrix mat_add(Matrix m1, Matrix m2);
// Subtract two matrices
Matrix mat_sub(Matrix m1, Matrix m2);
// Print the given matrix
void mat_print(Matrix m1);
// Free the given matrix
void mat_free(Matrix m);
