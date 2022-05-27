#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char **argv){
	if (MPI_Init(&argc, &argv)){
		fprintf(stderr, "Erreur MPI_Init\n");
  }

  if (argc < 3){
    fprintf(stderr, "Erreur\n");
  }
  
  int nbMessages = atoi(argv[1]);
  int tailleMessage = atoi(argv[2]);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if (size < 2){
		fprintf(stderr, "Erreur\n");
  }
  
  char data[tailleMessage];
  MPI_Status s;

  double start = MPI_Wtime();

  if(rank == 0){
    for(int i = 0; i<nbMessages; i++){
      MPI_Ssend(&data, tailleMessage, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
      MPI_Recv(&data, tailleMessage, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &s);
    }
  }
  else if(rank == 1){
    for(int i = 0; i<nbMessages; i++){
      MPI_Recv(&data, tailleMessage, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &s);
      MPI_Ssend(&data, tailleMessage, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
    
  }
  double end = MPI_Wtime();
  double duree = end-start;

  printf("Temps (process %d) : %f\n", rank, duree);
  double duree_globale;
  MPI_Reduce(&duree, &duree_globale, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  if(rank == 0){
    double temps1message = (((duree_globale)/2)/nbMessages)/2;
    double debit = (1/temps1message)*tailleMessage;
    printf("%d pingpong effectués avec un message de taille %d\n", nbMessages, tailleMessage);
    printf("Temps de transmission d'un message %fs\n", temps1message);
    printf("Débit : %f Goctet/s\n", debit/1000000000);

    if(duree_globale > 2){
      const char* filename = "out_distant.csv";

      FILE* output_file = fopen(filename, "a");
      if (!output_file) {
          perror("fopen");
          exit(EXIT_FAILURE);
      }
      fprintf(output_file,"%d;%d;%f;%f;%f\n", nbMessages, tailleMessage, duree_globale/2, temps1message, debit);

      fclose(output_file);
    }
    else{
      printf("Trop court\n");
    }
  }

  MPI_Finalize();
  return(0);

}