#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#define HYPERTHREADING 1      // 1 if hyperthreading is on, 0 otherwise
#define ERROR          1.e-20 // Acceptable precision
#define MAX_VAL        9      // Random values are [0, MAX_VAL]

// array size
#define N 10000000
// #define N 10000
// number of iterations
#define STEPS 10

void print(char *label, double vect[N], size_t len);
void swap(double **p1, double **p2);

double f(double x, double y, double z) {
  return pow(x*y*z, 1./3.);
}
/**
 *  --------------------------------------------------------
 *   Reference computation kernel (do not touch)
 *  --------------------------------------------------------
 *   Stencil computation, iterated, final result is in res.
 **/
int stencil1D_reference(double a[N], double r[N]) {
  r[0]   = a[0];
  r[N-1] = a[N-1];
  int  checksum = 0;

  // compute 1D stencil
  for (size_t step=0; step < STEPS; step++){
     for (size_t i = 1; i < N-1; i++) {
        r[i] = f(a[i-1], a[i], a[i+1]);
     }
     // after each step, make r the new input by exchanging a and r
     swap(&a, &r);
  } // all steps done

  #ifdef DEBUG
  print("r", r, 7);
  #endif

  // compute checksum on values in the final step
  for (size_t i=0; i < N; i++) {
	  checksum += (int)r[i];
  }
  return checksum;
}

/**
 *  --------------------------------------------------------
 *   Computation kernel (to parallelize)
 *  --------------------------------------------------------
 **/
int stencil1D_kernel(double a[N], double r[N]) {
  r[0]   = a[0];
  r[N-1] = a[N-1];
  int  checksum = 0;

  // compute 1D stencil STEPS time
  for(size_t step=0; step < STEPS; step++) { //TODO: Mieux paralléliser (question 1.2)
    #pragma omp parallel for
    for (size_t i = 1; i < N-1; i++) {
      r[i] = f(a[i-1], a[i], a[i+1]);
    }
    // after each step, make r the new input by exchanging a and r
    swap(&a, &r);
  } // all steps done

  #ifdef DEBUG
  print("r", r, 7);
  #endif

  // compute checksum on values in the final step
  #pragma omp parallel for reduction(+:checksum)
  for (size_t i=0; i < N; i++) {
	  checksum += (int)r[i];
  }
  return checksum;
}


// Utility
void print(char *label, double vect[N], size_t len) {
  printf("%s:\t[",label);
  for (size_t i=0;i<len;i++)
	  	printf("%lf, ",vect[i]);
  printf("... ");
  for (size_t i=N-len;i<N;i++)
	  	printf(", %.4lf",vect[i]);
  printf("]\n");
}

void swap(double **p1, double **p2) {
    double * tmp;
    tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}

/**
 * main
 **/

int main() {
  double * a        = malloc(N * sizeof(double));
  double * r        = malloc(N * sizeof(double));
  double * ref_a    = malloc(N * sizeof(double));
  double * ref_r    = malloc(N * sizeof(double));
  double time_reference, time_kernel, speedup, efficiency;
  int    check_r, check_k;

  // Initialization with random values
  //srand((unsigned int)time(NULL));
  for (size_t i = 0; i < N; i++) {
    a[i] = (float)rand()/(float)(RAND_MAX/MAX_VAL);
  }
  // Save initial values
  memcpy(ref_a, a, N*sizeof(double));

  time_reference = omp_get_wtime();
  // reference results are in ref_r after that
  // and check_k is the checksum returned
  check_r = stencil1D_reference(a, ref_r);
  time_reference = omp_get_wtime() - time_reference;
  printf("Reference time : %3.5lf s\n", time_reference);

  time_kernel = omp_get_wtime();
  // start again with original values, results are in r after that
  // and check_k is the checksum
  check_k = stencil1D_kernel(ref_a, r);
  time_kernel = omp_get_wtime() - time_kernel;
  printf("Kernel time -- : %3.5lf s\n", time_kernel);

  speedup = time_reference / time_kernel;
  efficiency = speedup / (omp_get_num_procs() / (1 + HYPERTHREADING));
  printf("Speedup ------ : %3.5lf\n", speedup);
  printf("Efficiency --- : %3.5lf\n", efficiency);

  // Check if the result differs from the reference
  for (size_t i = 0; i < N; i++) {
     if (fabs(ref_r[i] - r[i])> ERROR) {
        printf("Bad results :-(((  (at least, ref_r[%zu]!=r[%zu] (%.25lf!=%.25lf)\n",i,i,ref_r[i],r[i]);
        exit(1);
      }
  }
  // Check if checksum is identical
  if (check_r != check_k) {
      printf("Bad results :-(((  checksums differ by %d (%d != %d)\n",abs(check_r - check_k), check_r,check_k);
      exit(1);
  }
  printf("OK results :-)   [checksum=%d]\n",check_k);

  free(a);
  free(r);
  free(ref_a);
  free(ref_r);
  return 0;
}
