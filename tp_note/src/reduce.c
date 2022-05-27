// TP noté de programmation parallèle - exercice MPI - L3S6P 2021/2022.
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>


// variables globales, initialisées au début de main()
int rank, size;


// Fonction à écrire :
void ma_reduction(long *val, long *res, MPI_Op op)
{
  int rank, size;
  long r = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Status s;
  
  //Si le processus est le rang 0
  if(rank == 0){
    *res += *val;
    // On attend n-1 messages
    for(int i=0; i<size-1; i++){
      long data;
      MPI_Recv(&data, sizeof(long), MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &s);
      if(op == MPI_PROD){
        printf("%ld\n", data);
        r = r * data;
      }
      else if(op == MPI_MAX){
        if(data > r){
          r = data;
        }
      }
    }
    *res = r;
  }
  //Sinon on envoie notre nombre
  else{
    MPI_Ssend(val, sizeof(long), MPI_LONG, 0, 0, MPI_COMM_WORLD);
  }
  return;
}


// Programme de test, ne nécerssite pas de modification.
int main( int argc, char **argv )
{
  srand(time(0)+getpid());
  // init
  MPI_Init(&argc,&argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if(rank==0)
    printf("Nb processus : %d\n\n", size);

  long valeur = 1+rand()%100;

  // test 1 : MPI_MAX
  long resultat=-1, resultat_mpi=-2;
  int erreur = 0;

  ma_reduction(&valeur, &resultat, MPI_MAX);
  MPI_Reduce(&valeur, &resultat_mpi, 1, MPI_LONG, MPI_MAX, 0, MPI_COMM_WORLD);
  if(rank==0)
  {
    printf("Test 1 (MPI_MAX): ");
    printf("résultat = %ld, résultat MPI = %ld\n", resultat, resultat_mpi);
    if(resultat != resultat_mpi)
      erreur = 1;
  }

  // test 2 : MPI_PROD
  resultat=-1; resultat_mpi=-2;
  ma_reduction(&valeur, &resultat, MPI_PROD);
  MPI_Reduce(&valeur, &resultat_mpi, 1, MPI_LONG, MPI_PROD, 0, MPI_COMM_WORLD);
  if(rank==0)
  {
    printf("Test 2 (MPI_PROD): ");
    printf("résultat = %ld, résultat MPI = %ld\n", resultat, resultat_mpi);
    if(resultat != resultat_mpi)
      erreur = 1;
  }

  // fin
  if(rank==0)
  {
    if(erreur)
      printf("Bad results :-(((\n");
    else
      printf("OK results :-)\n");
  }

  MPI_Finalize();
  return( 0 );
}
