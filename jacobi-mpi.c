#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "util.h"
#include <math.h>        

void midJacobi( double * f, double * u, double * v, int n, double h);
void rightJacobi( double * f, double * u, double * v, int n, double h);
void leftJacobi( double * f, double * u, double * v, int n, double h);
void swap(double * u, double * v, int n);

int main(int argc, char *argv[]) {
	
	// make sure we got all the arguments we need
	if (3 != argc){
        printf("Incorrect number of args");
        return 0;
    }  
    int Np1 = atoi(argv[1]);	// the input length corresponds to N+1 so we get N
	int T = atoi(argv[2]);		// number of iterations





    int rank, size; 			// size is number of processors
    const double L = 1.;       	// The length of the sides of the box
    const double h = L/(Np1); 	// There are N+1 intervals between x=0 and x=L.
    double  *u, *unew, *f;
	int i,k;
	timestamp_type start, finish;
    double time;

	// initializing MPI. Nothing interesting here
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

	// if you are the zeroh processor, print something 
	if (rank == 0) {
		printf("Length of array: %i\n", Np1);
		printf("Number of iterations: %i\n", T);
	}


	//printf("size: %i\n", size);
	if (Np1 % size != 0){
        printf("Can't distribute points uniformly between processors. Abort.\n");
        return 0;
    } 

	// size of array each processor holds. It "owns" only 1 through n-2 of them
	// because 0 and n-1 are boundary points
	int n = (Np1)/size + 2;

	// allocate memory for everything
    u     = (double *) calloc((size_t)n , sizeof(double)  );
    unew  = (double *) calloc((size_t)n , sizeof(double)  );
    f     = (double *) calloc((size_t)n , sizeof(double)  );
	
	// initialize f to ones	( change here if we solve for different f)
	for( i = 0; i < n ; i++ ) {
		f[i] = 1.0;
	}


	// start timing communication			
	get_timestamp(&start);

	//start iteration
	//printf("Processor %d STARTING ITERATION\n" ,rank);

	for( k = 0; k < T; k++ ) {
	
		//special case: beginning
		if (rank == 0){

			//send rightmost value processor owns (i.e. u[n-2]) to rank 1
			MPI_Send(&u[n-2], 1, MPI_DOUBLE, 1, 999, MPI_COMM_WORLD);

			//receive rightmost value of rank 1 (this processor doesn't "own"
			// u[n-1])
			MPI_Recv(&u[n-1], 1, MPI_DOUBLE, 1, 999, MPI_COMM_WORLD, &status);

			// perform one jacobi iteration for the left chunk
			leftJacobi(f, u, unew, n, h);

			// copy u^{k+1} into u^k
			swap(u , unew, n);	


		}


		//special case: rightmost 
		else if (rank == size-1){
		
			//send left most value this processor "owns" (i.e. u[1]) to its left neighbour
			MPI_Send(&u[1], 1, MPI_DOUBLE, rank-1, 999, MPI_COMM_WORLD);
		
			//receive into leftmost value this processor doesn't "own" (namely, u[0])
			MPI_Recv(&u[0], 1, MPI_DOUBLE, rank-1, 999, MPI_COMM_WORLD, &status);

			// perform one jacobi iteration for the right chunk
			rightJacobi(f, u, unew, n, h);

			// copy u^{k+1} into u^k
			swap(u , unew, n);
		}


		//middle part of array
		else{
		
			//send owned value to left neighbour 
			MPI_Send(&u[1], 1, MPI_DOUBLE, rank-1, 999, MPI_COMM_WORLD);

			// send owned value to right neighbour
			MPI_Send(&u[n-2], 1, MPI_DOUBLE, rank+1, 999, MPI_COMM_WORLD);
		
			// receive into unowned left point (which is u[0])
			MPI_Recv(&u[0], 1, MPI_DOUBLE, rank-1, 999, MPI_COMM_WORLD, &status);

			// receive into unowned right point (which is u[n-1])
			MPI_Recv(&u[n-1], 1, MPI_DOUBLE, rank+1, 999, MPI_COMM_WORLD, &status);
	

			// do a jacobi iteration on our owned array
			midJacobi( f, u, unew, n, h);

			// copy u^{k+1} into u^k
			swap(u , unew, n);	
		}

		// declare end of iteration
		//printf("Finished iteration #: %i\n", k);
	}
	

	// Total  time
	get_timestamp(&finish);
    time = timestamp_diff_in_seconds(start, finish);
	if (rank == 0) {
		printf("Time = %3.6f .\n" , time );
	


	}



	//MPI_Waitall(&request, &status);
	

	/*
	// print the leftmost interval	
	if (rank == 0) {
    	printf("I am processor %d and this is my result:\n", rank);
		for ( k = 1; k < n-1; k++ ){ // skip the zeroth entry since it is junk
			printf("%3.9f\n", u[k]);
		}
	}
	*/

	MPI_Finalize();
	
	// free everything
    free(unew);
    free(u);
    free(f);
    return 0;
}


void swap(double * u, double * unew, int n) {
	/*
	* copy unew into u
	*/
	
	int i;
	for ( i = 0; i < n; i++ ) {        
				 u[i] = unew[i]; 
	}

}



void midJacobi( double * f, double * u, double * unew, int n, double h){
	/*
	perform Jacobi iteration on for a non endpoint processor
	the current processor runs a jacobi and has no view of the boundary
	
	current values are at u, next values are written to v.
	 
	f is an nx1 vector,
	u, v are also nx1
	every processor "owns" his points, which are 1 to n-2
	*/

	//iteration indices
	int i;
	double h2 = h*h;

	// every processor only "owns" indices 1 through n-2 so these
	// are the indices that change
	for( i = 1; i < n-1; i++){	
		unew[i] = (f[i]*h2 + u[i-1] + u[i+1]) / 2.0;
	}	  
}


void leftJacobi( double * f, double * u, double * unew, int n, double h){
	/*
	perform Jacobi iteration on for a left endpoint processor
	the current processor runs a jacobi and sees only the left boundary
	the current value is at u, the next value is written
	to v. 
	f is an nx1 vector,
	u, v are also nx1
	this processor "owns" his points, which are 1 to n-2. For this processor
	the point u[1] is the boundary point so it is never changed.
	*/

	//iteration indices
	int i;
	double h2 = h*h;

	// every processor only owns indices 1 through n-1 so these are the only 
	// indices that change (at least, potentially). For this processor the
	// point u[1] is the left boundary point so it is never changed 
	// (it is not really "owned" by this processor).
	
	for( i = 2; i < n-1; i++){	
		unew[i] = (f[i]*h2 + u[i-1] + u[i+1]) / 2.0;
	}	
	
	// make sure left boundary condition holds. In this case, the index
	// zero is junk (it corresponds to -h, which is outside our boundary).
	unew[1] = 0.0;
}  



void rightJacobi( double * f, double * u, double * unew, int n, double h){
	/*
	perform Jacobi iteration on for a right endpoint processor
	the current processor runs a jacobi and sees only the right boundary
	the current value is at u, the next value is written
	to v. 
	f is an nx1 vector,
	u, v are also nx1
	this processor "owns" his points, which are 1 to n-2. For this processor
	the point u[n-1] is the right boundary point so it is never changed.
	*/

	//iteration indices
	int i;
	double h2 = h*h;

	// every processor only owns indices 1 through n-2 so these are the only 
	// indices that change (at least, potentially). For this processor 
	// the point u[n-2] is the right boundary point so it is never
	//  changed (it is not really "owned"  by this processor).
	for( i = 1; i < n-2; i++){	
		unew[i] = (f[i]*h2 + u[i-1] + u[i+1]) / 2.0;
	}	  

	// make sure right boundary condition holds
	unew[n-2] = 0.0;
}
