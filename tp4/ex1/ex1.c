#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>

int main( int argc, char **argv )
{
	int rang, size;
	char hn[100];
	gethostname( hn, 100 );

	if( MPI_Init( &argc, &argv ) )
	{
		fprintf(stderr, "Erreur MPI_Init\n" );
		exit( 1 );
	}

        MPI_Comm_rank( MPI_COMM_WORLD, &rang );
        MPI_Comm_size( MPI_COMM_WORLD, &size );

        printf( "Je suis le processus numÃ©ro %d sur %d (sur la machine %s)\n", rang, size, hn );

	MPI_Finalize();
	return( 0 );
}