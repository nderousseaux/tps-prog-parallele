#include <stdio.h>
#ifdef _OPENMP
#include <omp.h> 
#else
	#define omp_get_thread_num ( ) 0
#endif


void nb_thread_kernel() {
  size_t nb_threads = 0;
  #pragma omp parallel
  {    
    nb_threads++;
  }
  printf("nb_threads = %zu\n", nb_threads);
}

void nb_thread_kernel_1() {
  size_t nb_threads = 0;
  #pragma omp parallel
  {
    #pragma omp single
    nb_threads = omp_get_num_threads();
  }
  printf("nb_threads = %zu\n", nb_threads);
}

void nb_thread_kernel_2() {
  size_t nb_threads = 0;
  #pragma omp parallel
  {
    #pragma omp critical    
    nb_threads++;
  }
  printf("nb_threads = %zu\n", nb_threads);
}


int main() {
  nb_thread_kernel();
  nb_thread_kernel_1();
  nb_thread_kernel_2();
  return 0;
}
