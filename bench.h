#pragma once

typedef enum {
    BENCH_CREATE = 1,
    BENCH_FILL  = 2,
    BENCH_ADD = 3,
    BENCH_SUB = 4,
    BENCH_MULT = 5,
    BENCH_PUT = 6,
    BENCH_ALL = 7
} BenchType;
void bench(BenchType type);
