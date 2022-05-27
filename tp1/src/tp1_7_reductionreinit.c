#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define HYPERTHREADING 1      // 1 if hyperthreading is on, 0 otherwise
#define ERROR          1.e-20 // Acceptable precision
#define MAX_VAL        5      // Random values are [0, MAX_VAL]

// Matrix size (5120: UHD TV)
#define N 5120

// Reference computation kernel (do not touch)
/**
 * reduction_reinit function:
 * this function returns the sum of all the elements of a square matrix
 * and sets the matrix elements to 0.
 * \param[in,out] A    The matrix to use (reset to 0 by the function).
 * \param[out]    sum  The sum of the elements of the input matrix.
 */
void reduction_reinit_reference(double A[N][N], double* sum) { 
  *sum = 0.;

  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < N; j++) {
      *sum += A[i][j];
      A[i][j] = 0.;
    }
  }
}

// Computation kernel (to parallelize)
void reduction_reinit_kernel(double A[N][N], double* sum) { 
  double newSum = 0.;

  #pragma omp parallel for reduction(+:newSum)
  for(size_t i = 0; i < N; i++) {
    for(size_t j = 0; j < N; j++) {
      newSum += A[i][j];
      A[i][j] = 0.;
    }
  }

  *sum = newSum;
}

int main() {
  double* AR = malloc(N * N * sizeof(double));
  double* AK = malloc(N * N * sizeof(double));
  double sum_ref, sum_ker;
  double time_reference, time_kernel, speedup, efficiency;

  // Initialization by random values
  srand((unsigned int)time(NULL));
  for (size_t i = 0; i < N * N; i++)
    AR[i] = AK[i] = (float)rand()/(float)(RAND_MAX/MAX_VAL);

  time_reference = omp_get_wtime();
  reduction_reinit_reference((double (*)[N])AR, &sum_ref);
  time_reference = omp_get_wtime() - time_reference;
  printf("Reference time : %3.5lf s\n", time_reference);

  time_kernel = omp_get_wtime();
  reduction_reinit_kernel((double (*)[N])AK, &sum_ker);
  time_kernel = omp_get_wtime() - time_kernel;
  printf("Kernel time    : %3.5lf s\n", time_kernel);

  speedup = time_reference / time_kernel;
  efficiency = speedup / (omp_get_num_procs() / (1 + HYPERTHREADING));
  printf("Speedup ------ : %3.5lf\n", speedup);
  printf("Efficiency --- : %3.5lf\n", efficiency);

  // Check if the result differs from the reference
  if (abs(sum_ref - sum_ker) > ERROR) {
    printf("Bad results :-(((\n");
    exit(1);
  }
  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < N; j++) {
      if (AR[i * N + j] != AK[i * N + j]) {
        printf("Bad results :-(((\n");
        exit(1);
      }
    }
  }
  printf("OK results :-)\n");

  free(AK);
  free(AR);
  return 0;
}
