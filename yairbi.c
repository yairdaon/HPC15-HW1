#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// declare function
void jacobiIteration(double *f, double * u, double *v, int n);

double resNorm(double * u , double * f , int n);

int main (int argc, char **argv)
{




	// Arguiments: gridsize and number of iter
	if (argc != 3) {
		fprintf(stderr, "Wrong nubmer of arguments. Aborting.\n");
    	abort();
	}
	
	
  /*if (argc == 2) { 

    int n1 = atoi(argv[1]);
    int gs1, jac1, t;
    clock_t start, diff;
    printf("\n\n\n");
    printf("Preparing to run. n = %d. \n", n1);

    start = clock();
    gs1  = fullGS(n1);
    diff = clock() - start;
    t = (double)diff / CLOCKS_PER_SEC;
    printf("Gauss Seidel. n = %d used. %d iterations. Elapsed time =  %d seconds.\n", n1, gs1, t);



    start = clock();
    jac1  = fullJacobi(n1);
    diff = clock() - start;
    t = (double)diff / CLOCKS_PER_SEC;
    printf("Jacobi.  n = %d. used %d iterations. Elapsed time =  %d seconds.\n", n1, jac1, t);

  } else if ( argc == 3) {

    int n1 = atoi(argv[1]);
    int n2 = atoi(argv[2]);

    printf("\n\n\n");
    printf("Preparing to run. n1 = %d, n2 = %d. \n", n1, n2);

    int gs1  = fullGS(n1);
    printf("GS  n = %d used %d iterations\n", n1, gs1  );

    int jac1 = fullJacobi(n1);
    printf("Jac n = %d used %d iterations\n", n1, jac1 );

    int gs2  = fullGS(n2);
    printf("GS  n = %d used %d iterations\n", n2, gs2  );

    int jac2 = fullJacobi(n2);
    printf("GS  n = %d used %d iterations\n", n2, jac2 );
  } else {
    fprintf(stderr, "Wrong nubmer of arguments. Aborting.\n");
    abort();*/
  }

  return 0;
}
  
int fullJacobi(int n){

  printf("\n\n\n");



  // INITIALIZATION STEPS!!!!


  // getting arguments
  int jit  = 0;
  int jump = 1000*n;

  double  *u, *v ,*f;
  int i,j;
  double hneg2 = (n+1) * (n+1);
  double initial;
  double factor = 1e6;
  double jacResNorm;

  u  = (double *) malloc(sizeof(double) * n  );
  v  = (double *) malloc(sizeof(double) * n  );
  f  = (double *) malloc(sizeof(double) * n  );



  // set values and stuff
  for( i = 0 ; i < n ; i++) {
    f[i] = 1.0;
    u[i] = 0.0;
  }

    
  initial = resNorm(u, f, n);
  jacResNorm = initial;


  // FINISHED INITIALIZATION - NOW THE NUMERICS!!!


  // do jacobi iterations, two per round
  while( jacResNorm > initial/factor ) {
   
    jacobiIteration( f, u, v, n);
    jit++;
    jacResNorm = resNorm(v, f, n);
    if( jit % jump == 0 )
      printf("Jacobi: ||Au^(%d) - f|| = %f \n", jit, jacResNorm );

    jacobiIteration(f, v, u, n); 
    jit++;
    jacResNorm = resNorm(u, f, n);
    
    /*if( jit % jump == 0 )
      printf("Jacobi: ||Au^(%d) - f|| = %f \n", jit, jacResNorm );*/
 
  }

 


  // print results
  printf("Results: \n");
  printf("Jacobi: %d iterations,  ||Au - f|| = %f \n",  jit, jacResNorm );

  // free everything
  free(f);
  free(v);
  free(u);
  return jit;

}



int fullGS(int n) {
 
  printf("\n\n\n");


  // setting arguments
  int gsit = 0;
  int jump = 1000*n;

  double  *f, *gs;
  int i,j;
  double hneg2 = (n+1) * (n+1) ;
  double initial;
  double factor = 1e6;
  double gsResNorm;

  f  = (double *) malloc(sizeof(double) * n  );
  gs = (double *) malloc(sizeof(double) * n  );



  // set values and stuff
  for( i = 0 ; i < n ; i++) {
    f[i] = 1.0;
    gs[i] = 0.0;
  }

  
  initial = resNorm(gs, f, n);
  gsResNorm  = initial;


  // do gauss Seidel iterations, one per round
  while( gsResNorm > initial/factor ) {
   
    gsIteration( f, gs, n);
    gsit++;
    gsResNorm = resNorm(gs , f, n);
    /*if( gsit % jump  == 0) 
      printf("Gauss Seidel: ||Au^(%d) - f|| = %f \n", gsit, gsResNorm );*/
  }



  // print results
  printf("Results: \n");
  printf("Gauss Seidel: %d iterations, ||Au - f|| = %f \n", gsit, gsResNorm );

  // free everything
  free(gs);
  free(f);
  return gsit;

}




void gsIteration( double * f, double * u ,  int n){
  /*
    f is an nx1 vector,
    u is an nx1 vector.
  */


  //iteration indices
  int i,j;

  int hneg2 = (n+1) * (n+1);
 
  u[0] = (f[0] + hneg2*u[1]) / (2.0*hneg2); 

  // the iteration!!!
  for( i = 1; i < n-1; i++){
    u[i] = (f[i] + hneg2*u[i+1] + hneg2*u[i-1]) / (2.0*hneg2);    
  }
  
  u[n-1] = (f[n-1] + hneg2*u[n-2]) / (2.0*hneg2);
  
}




void jacobiIteration( double * f, double * u, double * v, int n){
  /*
    the current value is at u, the next value is written
    to v. 
    f is an nx1 vector,
    u, v are also nx1
  */

  //iteration indices
  int i,j;
  int hneg2 = (n+1)*(n+1);
 

  v[0] = (f[0] + hneg2*u[1]) / (2.0*hneg2);

  // the iteration!!!
  for( i = 1; i < n-1; i++){
    
    v[i] = (f[i] + u[i-1]*hneg2 + u[i+1]*hneg2) / (2.0*hneg2);
  }
  v[n-1] = (f[n-1] + hneg2*u[n-2]) / (2.0*hneg2);
 
  
}

double resNorm(double * u, double * f,  int n) {

  int i;
  double norm, tmp;
  int hneg2 = (n+1) * (n+1);

  tmp  = f[0] - 2.0*hneg2*u[0] + hneg2*u[1];
  norm = tmp*tmp;

  for(i = 1 ; i < n-1 ; i++){

    tmp = f[i] - 2.0*hneg2*u[i] + hneg2*u[i-1] + hneg2*u[i+1];
    norm += tmp*tmp;
  }

  tmp = f[n-1] -2.0*hneg2*u[i] + hneg2*u[n-2];
  norm += tmp*tmp;
 
  return sqrt(norm);
}
