#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>

/* matrix size */
#define N 10

// Random values are [0, MAX_VAL]
#define MAX_VAL 5

// Nomber of checks
#define NBCHECKS 10
// acceptable error (in check)
#define ERROR   1.e-20

#define DIFFTEMPS(a,b) (((b).tv_sec - (a).tv_sec) + ((b).tv_usec - (a).tv_usec)/1000000.)

/* global to avoid stack overflow */
float a[N][N],b[N][N],c[N][N];
int main(int argc, char **argv)
{
	if (MPI_Init(&argc, &argv)){
		fprintf(stderr, "Erreur MPI_Init\n");
  }

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  struct timeval tv_init, tv_begin, tv_end;
  gettimeofday( &tv_init, NULL);


  float aa[N/size][N],cc[N/size][N];

  /***************************************************************************/
  // initialization
  if(rank == 0){
    srand((unsigned int)time(NULL));
    for (int i=0;i<N;i++)
      for (int j=0;j<N;j++)
      {
        b[i][j]=(double)rand()/(double)(RAND_MAX/MAX_VAL);
        a[i][j]=(double)rand()/(double)(RAND_MAX/MAX_VAL);
      }
  }
  /***************************************************************************/
  // compute
  gettimeofday( &tv_begin, NULL);

  //On envoie chaque ligne de la matrice à aux processus
  MPI_Scatter(a, N*N/size, MPI_FLOAT, aa, N*N/size, MPI_FLOAT,0,MPI_COMM_WORLD);
  //MPI_Bcast(b, N*N, MPI_FLOAT, 0, MPI_COMM_WORLD); //On envoie b à tout le monde (pas nécessaire)
  MPI_Barrier(MPI_COMM_WORLD);

  //Multiplication de vecteurs pour chaque processus
  float sum = 0.;
  for (int k = 0; k < N/size; k++){
    for (int i = 0; i < N; i++){
      for(int j = 0; j<N; j++){
        sum +=  aa[k][j] * b[j][i];
      }
      cc[k][i]= sum;
      sum = 0.;
    }
  }
  MPI_Gather(cc, N*N/size, MPI_FLOAT, c, N*N/size, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);        



  // // for(int i=0;i<N;i++)
  // //   for(int j=0;j<N;j++)
  // //     c[i][j] = 0.;
  // // for(int i=0 ; i<N ; i++)
  // //   for(int k=0 ; k<N ; k++)
  // //     for(int j=0 ; j<N ; j++)
  // //       c[i][j] += a[i][k] * b[k][j];


  gettimeofday( &tv_end, NULL);

  /***************************************************************************/
  // check some arbitrary values
  if(rank == 0){
    int check_ok = 1;
    for(int checks=0 ; checks<NBCHECKS ; checks++)
    {
      int i = rand()%N;
      int j = rand()%N;
      float val = 0.;
      for(int k=0 ; k<N ; k++)
        val += a[i][k] * b[k][j];
      if(fabs(val - c[i][j]) > ERROR)
      {
        fprintf(stderr, "BAD RESULTS !");
        fprintf(stderr, " (value[%d][%d] = %g should be %g)\n",
                i, j, c[i][j], val);
        check_ok = 0;
      }
    }
    if(check_ok)
      fprintf(stderr, "Ok results :)\n");
  }

  /***************************************************************************/
  /* execution times */
  printf("Init : %lfs, Compute : %lfs\n",
         DIFFTEMPS(tv_init,tv_begin),
         DIFFTEMPS(tv_begin,tv_end));

  MPI_Finalize();
  return( 0 );
}
