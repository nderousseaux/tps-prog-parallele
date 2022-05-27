#include <stdio.h>
int main() {
  #pragma omp parallel num_threads(100)
  {
    printf("Hello\n");
    printf("World\n");
  }
  return 0;
}
