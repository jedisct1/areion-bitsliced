#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "areion.h"

static double seconds(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

static void run_benchmark(const char* name, size_t len, void (*permute)(uint8_t*))
{
    const size_t warmup = 10000;
    const size_t iterations = 200000;
    const size_t reps = 5;
    uint8_t state[64] = { 0 };
    double best = 0.0;
    size_t rep;
    size_t i;

    for (i = 0; i < warmup; i++) {
        permute(state);
    }
    for (rep = 0; rep < reps; rep++) {
        const double start = seconds();
        for (i = 0; i < iterations; i++) {
            permute(state);
        }
        const double elapsed = seconds() - start;
        if (rep == 0 || elapsed < best) {
            best = elapsed;
        }
    }
    printf("%s: %8.2f ns/permutation, %8.2f MiB/s  [%02x]\n", name,
        best * 1e9 / (double)iterations,
        (double)(iterations * len) / (best * 1024.0 * 1024.0), state[0]);
}

int main(void)
{
    run_benchmark("AREION-256", 32, areion256_permute);
    run_benchmark("AREION-512", 64, areion512_permute);
    return 0;
}
