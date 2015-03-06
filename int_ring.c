#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "util.h"

int main(int argc, char *argv[]) {
	if (2 != argc){
        printf("Incorrect number of args");
        return 0;
    }

    int N = atoi(argv[1]);
    int rank, msgout, msgin, size;
    msgin = 0;
	int i;



    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    //Passing single integer as message around the ring
    for (i = 0; i < N; ++i) {

        //Handle processor 0 separately for correct start and end
        if (0 == rank) {
            msgout = msgin + rank;
            printf("My rank = %d. I send thee %d\n", rank, msgout);
            MPI_Send(&msgout, 1, MPI_INT, 1, 999, MPI_COMM_WORLD);
            MPI_Recv(&msgin, 1, MPI_INT, size-1, 999, MPI_COMM_WORLD, &status);
			printf("\n \n Processor 0 with final message %d. \n ",  N*size*(0 + size-1)/2);
        }
    
        //Other processors
        else {
            MPI_Recv(&msgin, 1, MPI_INT, rank-1, 999, MPI_COMM_WORLD, &status);
            msgout = msgin + rank;
            printf("My rank b %d. I send thee %d\n", rank, msgout);
            MPI_Send(&msgout, 1, MPI_INT, (rank+1)%size, 999, MPI_COMM_WORLD);
        }
    }	
	
	
	


    MPI_Finalize();
    return 0;
}


