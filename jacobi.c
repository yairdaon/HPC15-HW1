#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
//#include "util.h"

#include <math.h>         // to use math functions  

int main(int argc, char *argv[]) {
	if (3 != argc){
        printf("Incorrect number of args");
        return 0;
    }  
    int M = atoi(argv[1]);   //get length of array
	int T = atoi(argv[2]);   //get number of iterations




    int rank, size; //size is number of processors
    int N; //length without endpoints
    const double L = 1.;       // The length of the sides of the box
    const double h = L/(N-1);  // h = Delta x = Delta y.  There are N+1 intervals between x=0 and x=L.
    double  *u, *unew;
	int i;

    /*INITIALIZE
	int i;
    for ( i = 0; i < N+2; i++ ) {        
         u[i] = 0;
         unew[i] = 0;
    }
	*/

    MPI_Status status;
    //MPI_Request request;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		printf("Length of array: %i\n", M);
		printf("Number of iterations: %i\n", T);
	}


	printf("size: %i\n", size);
	if (M % size != 0){
        printf("M is not multiple of p\n");
        return 0;
    } 
	N = M/size;
    u     = (double *) calloc((size_t)N+2 , sizeof(double)  );
    unew  = (double *) calloc((size_t)N+2 , sizeof(double)  );

	//start iteration
	printf("STARTING ITERATION\n");
	int k = 0;
while ( k < T ) {
	//special case: beginning
	if (rank == 0){
		for ( i = 2; i < N+1; i++ ) {        
			unew[i] = 0.5*(u[i-1]+u[i+1]-h*h);   //update u
		}
		//send
		MPI_Send(&u[N], 1, MPI_DOUBLE, rank+1, 998, MPI_COMM_WORLD);
		//receive
		MPI_Recv(&u[N+1], 1, MPI_DOUBLE, rank+1, 999, MPI_COMM_WORLD, &status);
		unew[1] = 0;   //update u
		unew[N+1] = 0.5*(u[N-1]+u[N+1]-h*h);   //update u
		for ( i = 0; i < N+2; i++ ) {        
		     u[i] = unew[i];        	       //update u
	    }
	}


	//special case: end
	else if (rank == size-1){
		for ( i = 2; i < N+1; i++ ) {        
			unew[i] = 0.5*(u[i-1]+u[i+1]-h*h);   //update u
		}
		//send
		MPI_Send(&u[1], 1, MPI_DOUBLE, rank-1, 999, MPI_COMM_WORLD);
		//receive
		MPI_Recv(&u[0], 1, MPI_DOUBLE, rank-1, 998, MPI_COMM_WORLD, &status);
		unew[1] = 0.5*(u[0]+u[2]-h*h);   //update u
		unew[N+1] = 0;   //update u
		for ( i = 0; i < N+2; i++ ) {        
		     u[i] = unew[i];        	       //update u
	    }
	}


	//middle part of array
	else{

		for ( i = 2; i < N+1; i++ ) {        
			unew[i] = 0.5*(u[i-1]+u[i+1]-h*h);   //update u
		}
		//send right
		MPI_Send(&u[1], 1, MPI_DOUBLE, rank-1, 999, MPI_COMM_WORLD);
		MPI_Send(&u[N], 1, MPI_DOUBLE, rank+1, 998, MPI_COMM_WORLD);
		//receive
		MPI_Recv(&u[0], 1, MPI_DOUBLE, rank-1, 998, MPI_COMM_WORLD, &status);
		MPI_Recv(&u[N+1], 1, MPI_DOUBLE, rank+1, 999, MPI_COMM_WORLD, &status);
		unew[1] = 0.5*(u[0]+u[2]-h*h);   //update u
		unew[N+1] = 0.5*(u[N-1]+u[N+1]-h*h);   //update u
		for ( i = 0; i < N+2; i++ ) {        
		     u[i] = unew[i];        	       //update u
	    }
	}
printf("Finished iteration #: %i\n", k);
k++;
}
	
	//end iteration

    printf("HAPPY END\n");
	//MPI_Waitall(&request, &status);
	printf("This is the result:\n");
	for ( i = 0; i < size; i++ ){
		if (rank == i){
	    printf("I am processor %i and this is my result:\n", i);
		for ( k = 1; k < N+2; k++ ){
			printf("%3.2f\n", u[k]);
		}
		}
	}

	MPI_Finalize();
	// free everything
    free(unew);
    free(u);
    return 0;
}


