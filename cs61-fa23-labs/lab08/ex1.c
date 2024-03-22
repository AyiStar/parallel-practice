#include "ex1.h"

void v_add_naive(double* x, double* y, double* z) {
    #pragma omp parallel
    {
        for(int i=0; i<ARRAY_SIZE; i++)
            z[i] = x[i] + y[i];
    }
}

// Adjacent Method
void v_add_optimized_adjacent(double* x, double* y, double* z) {
    #pragma omp parallel
    {
        int n_threads = omp_get_num_threads();
        int tid = omp_get_thread_num();
        for (int i = tid; i < ARRAY_SIZE; i += n_threads) {
            z[i] = x[i] + y[i];
        }
    }
}

// Chunks Method
void v_add_optimized_chunks(double* x, double* y, double* z) {
    int n_threads;
    #pragma omp parallel
    {
        n_threads = omp_get_num_threads();
        int tid = omp_get_thread_num();
        int chunk_size = ARRAY_SIZE / n_threads;
        int chunk_start = tid * chunk_size;
        int chunk_end = (tid + 1) * chunk_size;
        for (int i = chunk_start; i < chunk_end; i++) {
            z[i] = x[i] + y[i];
        }
    }
    
    for (int i = (ARRAY_SIZE / n_threads * n_threads); i < ARRAY_SIZE; i++) {
        z[i] = x[i] + y[i];
    }
}
