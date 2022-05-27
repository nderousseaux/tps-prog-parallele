#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>
#define HYPERTHREADING 1 // 1 if hyperthreading is on, 0 otherwise
#define MAX_VAL        5 // Random values are [0, MAX_VAL]

double f(double x, double y, unsigned int time) {
  sleep(time);
  return x + 2 * y;
}

// Reference computation kernel (do not touch)
void dag_reference(double r1, double r2, double r3, double* r) {
  double d1, d2, d3, d4, d5, d6;

  d1 = f(r1, r2, 1);
  d2 = f(r2, r3, 1);
  d3 = f(d1, d2, 1);
  d4 = f(r1, r3, 2);
  d5 = f(r2, d2, 1);
  d6 = f(d5, d4+d3, 1);

  *r = d6;
}

// Computation kernel (to parallelize)
void dag_kernel_without_task(double r1, double r2, double r3, double* r) {
  double d1, d2, d3, d4, d5, d6;

  #pragma omp parallel sections
  {
    #pragma omp section
    d1 = f(r1, r2, 1);
    #pragma omp section
    d2 = f(r2, r3, 1);
    #pragma omp section
    d4 = f(r1, r3, 2);
  }
  #pragma omp parallel sections
  {
    #pragma omp section
    d3 = f(d1, d2, 1);
    #pragma omp section
    d5 = f(r2, d2, 1);
  }
  d6 = f(d5, d4+d3, 1);

  *r = d6;
}

// Computation kernel (to parallelize)
void dag_kernel(double r1, double r2, double r3, double* r) {
  double d1, d2, d3, d4, d5, d6;

    #pragma omp parallel
    {
      #pragma omp single
      {
        #pragma omp task depend(out:d1)
        d1 = f(r1, r2, 1);
        #pragma omp task depend(out:d2)
        d2 = f(r2, r3, 1);
        #pragma omp task depend(out:d4)
        d4 = f(r1, r3, 2);
        #pragma omp task depend(out:d3) depend(in:d1, d2)
        d3 = f(d1, d2, 1);
        #pragma omp task depend(out:d5) depend(in:d2)
        d5 = f(r2, d2, 1);
        #pragma omp task depend(out:d6) depend(in:d4, d3)
        d6 = f(d5, d4+d3, 1);
      }
    }

  *r = d6;

}

int main() {
  double val_ref, val_ker;
  double time_reference, time_kernel, speedup, efficiency;
  // Initialization by random values
  srand((unsigned int)time(NULL));
  double val1 = (double)rand()/(double)(RAND_MAX/MAX_VAL);
  double val2 = (double)rand()/(double)(RAND_MAX/MAX_VAL);
  double val3 = (double)rand()/(double)(RAND_MAX/MAX_VAL);
    
  time_reference = omp_get_wtime();
  dag_reference(val1, val2, val3, &val_ref);
  time_reference = omp_get_wtime() - time_reference;
  printf("Reference time : %3.5lf s\n", time_reference);
  
  time_kernel = omp_get_wtime();
  dag_kernel(val1, val2, val3, &val_ker);
  time_kernel = omp_get_wtime() - time_kernel;
  printf("Kernel time    : %3.5lf s\n", time_kernel);

  speedup = time_reference / time_kernel;
  efficiency = speedup / (omp_get_num_procs() / (1 + HYPERTHREADING));
  printf("Speedup ------ : %3.5lf\n", speedup);
  printf("Efficiency --- : %3.5lf\n", efficiency);

  // Check if the result differs from the reference
  if (val_ref != val_ker) {
    printf("Bad results :-(((\n");
    exit(1);
  }
  printf("OK results :-)\n");

  return 0;
}
