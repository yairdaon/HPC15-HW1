#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "util.h"
	
int main(int argc, char *argv[]) {
	
	if (2 != argc){
        printf("Incorrect number of args");
        return 0;
    }

    int iterations = atoi(argv[1]); // number of passes around the ring
	int out = 0;
	int in  = 0; 
    int rank, size;
	int i;
 	int nsent = 0;
	int ngot  = 0;
	timestamp_type start, finish;
    double time;

	
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

	/* Array passing code is commented
	int arrSize    = 262144; // this many ints == 2MB of data
	int * msgout; 
	int * msgin;
	msgout = calloc((size_t) arrSize , sizeof(int));	
	msgin  = calloc((size_t) arrSize , sizeof(int));
	*/
	
	// start timing communication			
	get_timestamp(&start);


    // Passing message in the ring
    for (i = 0; i < iterations; i++) {



        // Processor 0 is different
        if (0 == rank) {

			// Increment message (necessary though no incrementation is done)
			out = in + rank;

			// Send out array
            //MPI_Send(msgout, arrSize, MPI_INT, 1, 999, MPI_COMM_WORLD);


			// Send out integer
            MPI_Send(&out, 1, MPI_INT, 1, 999, MPI_COMM_WORLD);
			nsent++;

			// Receive array
            //MPI_Recv(msgin, arrSize, MPI_INT, size-1, 999, MPI_COMM_WORLD, &status);


			// Receive integer
            MPI_Recv(&in, 1, MPI_INT, size-1, 999, MPI_COMM_WORLD, &status);
			ngot++;
        }
    
        //Other processors
        else {
	
			// Receive array
            //MPI_Recv(msgin, arrSize, MPI_INT, rank-1, 999, MPI_COMM_WORLD, &status);

			
			// Receive integer
            MPI_Recv(&in, 1, MPI_INT, rank-1, 999, MPI_COMM_WORLD, &status);
			ngot++;

			// Increment message
			out = in + rank;

			// Send array
            //MPI_Send(msgout, arrSize, MPI_INT, (rank+1)%size, 999, MPI_COMM_WORLD);


			// Send  integer
            MPI_Send(&out, 1, MPI_INT, (rank+1)%size, 999, MPI_COMM_WORLD);
			nsent++;

			
        }
    }	



	// Total communication time
	get_timestamp(&finish);
    time = timestamp_diff_in_seconds(start, finish);
	if (rank == 0) {
		printf("Final message %d. Should be %d\n" , in, iterations*size*(size-1)/2);	
		printf("Communication time = %f bytes/sec.\n" ,  sizeof(int)*iterations*(size-1)/time );
	


	}

	/*	
	// Array communication
	if (rank == 0) {
		printf("total time = %f \n", time);
		int bytesSent = (size-1)*iterations*arrSize*sizeof(int);
		printf("Average communication time = %d  bytes/second \n." , (int)(bytesSent/time));
	}

	*/
	
	
	

	/* if we have calloced arrays we free them
	free(msgin);
	free(msgout);
	*/    

	MPI_Finalize();
    return 0;
}


