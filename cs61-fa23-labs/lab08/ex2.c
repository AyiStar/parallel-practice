#include "ex2.h"

double dotp_naive(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    for (int i = 0; i < arr_size; i++)
        global_sum += x[i] * y[i];
    return global_sum;
}

// Critical Keyword
double dotp_critical(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    // TODO: Implement this function
    // Use the critical keyword here!
    #pragma omp parallel for
    for (int i = 0; i < arr_size; i++) {
        #pragma omp critical
        global_sum += x[i] * y[i];
    }
    return global_sum;
}

// Reduction Keyword
double dotp_reduction(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    // Use the reduction keyword here!
    #pragma omp parallel for reduction(+:global_sum)
    for (int i = 0; i < arr_size; i++) {
        global_sum += x[i] * y[i];
    }
    return global_sum;
}

// Manual Reduction
double dotp_manual_reduction(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    // Do NOT use the `reduction` directive here!
    int n_threads = 0;
    #pragma omp parallel
    {
        n_threads = omp_get_num_threads();
        int tid = omp_get_thread_num();
        int chunk_size = arr_size / n_threads;
        int chunk_start = tid * chunk_size;
        int chunk_end = chunk_start + chunk_size;
        if (tid == n_threads - 1) {
            chunk_end = arr_size;
        }
        double partial_sum = 0.0;
        for (int i = chunk_start; i < chunk_end; i++) {
            partial_sum += x[i] * y[i];
        }
        #pragma omp critical
        {
            global_sum += partial_sum;
        }
    }
    return global_sum;
}
