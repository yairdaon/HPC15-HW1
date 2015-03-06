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


	printf("Length of array: %i\n", M);
	printf("Number of iterations: %i\n", T);


    int rank, size; //size is number of processors
    int N; //length without endpoints
    const double L = 1.;       // The length of the sides of the box
    const double h = L/(N-1);  // h = Delta x = Delta y.  There are N+1 intervals between x=0 and x=L.
    double  *u, *unew;

    /*INITIALIZE
	int i;
    for ( i = 0; i < N+2; i++ ) {        
         u[i] = 0;
         unew[i] = 0;
    }
	*/

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


	printf("size: %i\n", size);
	if (M % size != 0){
        printf("M is not multiple of p\n");
        return 0;
    } 
	N = M/size;
    u  = (double *) calloc((size_t)N , sizeof(double)  );
    unew  = (double *) calloc((size_t)N , sizeof(double)  );

	//start iteration

	//special case: beginning

	//special case: end

	//middle part of array

   for ( i = 1; i < N+1; i++ ) {        
         unew[i] = 0.5*(u[i-1]+u[i+1]-h*h);   //update u
     }
   for ( i = 1; i < N+1; i++ ) {        
         u[i] = unew[i];        	       //update u
     }
	
	//end iteration

    MPI_Finalize();
	// free everything
    free(v);
    free(u);
    return 0;
}


