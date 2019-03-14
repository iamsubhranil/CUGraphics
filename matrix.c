#ifdef DEBUG
#include "display.h"
#endif
#include "matrix.h"

#include <malloc.h>
#include <memory.h>
#include <stdarg.h>

typedef struct Mat {
	double *values;
	int     m, n;
} Mat;

Mat *mat_new(int m, int n) {
	Mat *mt    = (Mat *)malloc(sizeof(Mat));
	mt->m      = m;
	mt->n      = n;
	mt->values = (double *)malloc(sizeof(double) * m * n);
	return mt;
}

static inline int mat_idx(Mat *m, int i, int j) {
	return (m->n * i) + j;
}

void mat_set(Mat *m1, int i, int j, double val) {
	m1->values[mat_idx(m1, i, j)] = val;
}

double mat_get(Mat *m1, int i, int j) {
	return m1->values[mat_idx(m1, i, j)];
}

void mat_fill(Mat *m1, ...) {
	va_list args;
	va_start(args, m1);
	for(int i = 0; i < m1->m; i++) {
		for(int j = 0; j < m1->n; j++) {
			mat_set(m1, i, j, va_arg(args, double));
		}
	}
	va_end(args);
}

void mat_print(Mat *m1) {
	printf("\n");
	for(int i = 0; i < m1->m; i++) {
		for(int j = 0; j < m1->n; j++) {
			printf("%3.5g", mat_get(m1, i, j));
		}
		printf("\n");
	}
}

Mat *mat_mult(Mat *m1, Mat *m2) {
	if(m1->n != m2->m) {
#ifdef DEBUG
		perr("Multiplying matrix with different dimensions : (%d x %d) and (%d "
		     "x %d)!",
		     m1->m, m1->n, m2->m, m2->n);
#endif
		return NULL;
	}
	Mat *res = mat_new(m1->m, m2->n);
	// Row loop
	for(int i = 0; i < res->m; i++) {
		// Column loop
		for(int j = 0; j < res->n; j++) {
			// printf("\n%d", j);
			// fflush(stdout);
			double sum = 0;
			for(int k = 0; k < m1->n; k++) {
				sum += (mat_get(m1, i, k) * mat_get(m2, k, j));
			}
			mat_set(res, i, j, sum);
		}
	}
	return res;
}

Mat *mat_add(Mat *m1, Mat *m2) {
	if(m1->m != m2->m || m1->n != m2->n) {
#ifdef DEBUG
		perr("Adding matrix with different dimensions : (%d x %d) and (%d x "
		     "%d)!",
		     m1->m, m1->n, m2->m, m2->n);
#endif
		return NULL;
	}
	Mat *res = mat_new(m1->m, m1->n);
	for(int i = 0; i < m1->m; i++) {
		for(int j = 0; j < m1->n; j++) {
			mat_set(res, i, j, mat_get(m1, i, j) + mat_get(m2, i, j));
		}
	}
	return res;
}

Mat *mat_sub(Mat *m1, Mat *m2) {
	if(m1->m != m2->m || m1->n != m2->n) {
#ifdef DEBUG
		perr("Subtracting matrix with different dimensions : (%d x %d) and (%d "
		     "x %d)!",
		     m1->m, m1->n, m2->m, m2->n);
#endif
		return NULL;
	}
	Mat *res = mat_new(m1->m, m1->n);
	for(int i = 0; i < m1->m; i++) {
		for(int j = 0; j < m1->n; j++) {
			mat_set(res, i, j, mat_get(m1, i, j) - mat_get(m2, i, j));
		}
	}
	return res;
}

void mat_free(Mat *m1) {
	free(m1->values);
	free(m1);
}
