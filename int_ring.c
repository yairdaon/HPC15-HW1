#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "util.h"

void alter(int * in, int *out, int add, int size) {
	
	int i;
	for (i = 0 ; i < size ; i++){
		//printf("  %d  ",i);
		*(out+i) = *(in+i) + add;
	}
}
	
int main(int argc, char *argv[]) {
	
	if (3 != argc){
        printf("Incorrect number of args");
        return 0;
    }

    int iterations = atoi(argv[1]);
	int arrSize    = atoi(argv[2]);
    int rank, size;
	int * msgout;
	int * msgin;
	int i;
 	int nsent = 0;
	int ngot  = 0;

    /*timestamp_type time1, time2;
	get_timestamp(&time2);
    diff = timestamp_diff_in_seconds(time1, time2);*/

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


	msgout = calloc((size_t) arrSize , sizeof(int));	
	msgin  = calloc((size_t) arrSize , sizeof(int));
	/*for(i = 0 ; i < size ; i++){
		msgout[i] = rank;
		msgin[i]  = rank;
	}*/	

    //Passing single integer as message around the ring
    for (i = 0; i < iterations; ++i) {

        //Handle processor 0 separately for correct start and end
        if (0 == rank) {

			
	
			// send out
            MPI_Send(msgout, arrSize, MPI_INT, 1, 999, MPI_COMM_WORLD);
			nsent++;
			//printf("Rank = %d. Sent     %d array.\n", rank, nsent   );

			// receive
            MPI_Recv(msgin, arrSize, MPI_INT, size-1, 999, MPI_COMM_WORLD, &status);
			ngot++;
			//printf("Rank = %d. Received %d array.\n", rank, ngot   );
			
			alter(msgin ,msgout, rank, size);

        }
    
        //Other processors
        else {
	
			// receive
            MPI_Recv(msgin, arrSize, MPI_INT, rank-1, 999, MPI_COMM_WORLD, &status);
			ngot++;
			//printf("Rank = %d. Received %d array.\n", rank, ngot   );

			// alter message
		    alter(msgin ,msgout, rank, size);

			// send out
            MPI_Send(msgout, arrSize, MPI_INT, (rank+1)%size, 999, MPI_COMM_WORLD);
			nsent++;
			//printf("Rank = %d. Sent     %d array.\n", rank, nsent   );
			
        }
    }	
	
	
	
	

	printf("\nRank = %d. msgin[%d] =  %d\n" ,rank ,rank, msgin[i]);	


	free(msgin);
	free(msgout);
    MPI_Finalize();
    return 0;
}


