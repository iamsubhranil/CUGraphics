#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bench.h"
#include "common.h"
#include "display.h"
#include "driver.h"
#include "matrix.h"

#define BENCH_DRAW_CALL_COUNT 500000
#define BENCH_MAT_CREATE_COUNT 200000
#define BENCH_RESULT_COUNT BENCH_MAT_CREATE_COUNT - 1
#define pbench(...)                       \
	phylw("\n[Benchmark] ", __VA_ARGS__); \
	fflush(stdout);

static clock_t start, end;
static Matrix  matrices[BENCH_MAT_CREATE_COUNT], result[BENCH_RESULT_COUNT];
static double  values[BENCH_MAT_CREATE_COUNT * 9];
static int     pixels[BENCH_DRAW_CALL_COUNT][2];

static void tstart() {
	start = clock();
}

static inline double telapsed() {
	end = clock();
	return (double)(end - start) / CLOCKS_PER_SEC;
}

static long rate(long itemcount) {
	return (long)(itemcount / telapsed());
}

static long mat_rate() {
	return rate(BENCH_MAT_CREATE_COUNT);
}

static inline void matrix_create() {
	for(int i = 0; i < BENCH_MAT_CREATE_COUNT; i++) {
		matrices[i] = mat_new(3, 3);
		mat_set(matrices[i], 1, 2, 4.5782783);
	}
}

static void bench_matrix_create() {
	pbench("Testing 3x3 matrix creation");
	tstart();
	matrix_create();
	printf("\t\t(%ld matrices/sec)", mat_rate());
}

static inline double randf(double a) {
	double x = ((double)rand() / (double)(RAND_MAX)) * a;
	return x;
}

static void bench_matrix_fill() {
	pbench("Testing 3x3 matrix fill");
	srand(time(NULL));
	for(size_t i = 0; i < BENCH_MAT_CREATE_COUNT; i++) {
		values[i] = randf(100000.0);
	}
	tstart();
	for(size_t i = 0; i < BENCH_MAT_CREATE_COUNT * 9; i += 9) {
		mat_fill(matrices[i / 9], values[i], values[i + 1], values[i + 2],
		         values[i + 3], values[i + 4], values[i + 5], values[i + 6],
		         values[i + 7], values[i + 8]);
	}
	printf("\t\t(%ld mat_fill/sec)", mat_rate());
}

static void free_result() {
	for(int i = 0; i < BENCH_RESULT_COUNT; i++) mat_free(result[i]);
}

static void bench_matrix_mult() {
	pbench("Testing 3x3 matrix multiplication");
	tstart();
	for(int i = 0; i < BENCH_RESULT_COUNT; i++) {
		result[i] = mat_mult(matrices[i], matrices[i + 1]);
	}
	printf("\t(%ld mat_mult/sec)", mat_rate());
	free_result();
}

static void bench_matrix_add() {
	pbench("Testing 3x3 matrix addition");
	tstart();
	for(int i = 0; i < BENCH_RESULT_COUNT; i++) {
		result[i] = mat_add(matrices[i], matrices[i + 1]);
	}
	printf("\t\t(%ld mat_add/sec)", mat_rate());
	free_result();
}

static void bench_matrix_sub() {
	pbench("Testing 3x3 matrix subtraction");
	tstart();
	for(int i = 0; i < BENCH_RESULT_COUNT; i++) {
		result[i] = mat_sub(matrices[i], matrices[i + 1]);
	}
	printf("\t(%ld mat_sub/sec)", mat_rate());
	free_result();
}

static void free_mat() {
	for(int i = 0; i < BENCH_MAT_CREATE_COUNT; i++) mat_free(matrices[i]);
}

// Assumes 0 <= max <= RAND_MAX
// Returns in the closed interval [0, max]
i64 random_at_most(i64 max) {
	i64
	    // max <= RAND_MAX < ULONG_MAX, so this is okay.
	    num_bins = max + 1,
	    num_rand = (i64)RAND_MAX + 1, bin_size = num_rand / num_bins,
	    defect = num_rand % num_bins;

	i64 x;
	do {
		x = random();
	}
	// This is carefully written not to overflow
	while(num_rand - defect <= (i64)x);

	// Truncated division is intentional
	return x / bin_size;
}

static void bench_draw() {
	init_driver();
	srand(time(NULL));
	int row = get_rows(), cols = get_columns();
	for(int i = 0; i < BENCH_DRAW_CALL_COUNT; i++) {
		pixels[i][1] = random_at_most(row - 1);
		pixels[i][0] = random_at_most(cols - 1);
	}
	tstart();
	for(int i = 0; i < BENCH_DRAW_CALL_COUNT; i++) {
		put_pixel(pixels[i][0], pixels[i][1]);
	}
	long rt = rate(BENCH_DRAW_CALL_COUNT);
	terminate_driver();
	pbench("Testing put_pixel calls\t\t(%ld put_pixel/sec)", rt);
}

void bench(BenchType type) {
	if(type != BENCH_CREATE && type != BENCH_ALL && type != BENCH_PUT) {
		pbench("Creating %ld 3x3 matrices..", BENCH_MAT_CREATE_COUNT);
		matrix_create();
	}
	switch(type) {
		case BENCH_CREATE: bench_matrix_create(); break;
		case BENCH_FILL: bench_matrix_fill(); break;
		case BENCH_ADD: bench_matrix_add(); break;
		case BENCH_MULT: bench_matrix_mult(); break;
		case BENCH_SUB: bench_matrix_sub(); break;
		case BENCH_PUT: bench_draw(); break;
		case BENCH_ALL:
			bench_matrix_create();
			bench_matrix_fill();
			bench_matrix_mult();
			bench_matrix_add();
			bench_matrix_sub();
			bench_draw();
			break;
	}
	if(type != BENCH_PUT)
		free_mat();
	printf("\n");
}
