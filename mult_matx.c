#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 4

double A[N][N];
double B[N][N];
double res[N][N];

void  MatrixMult(	double local_A[],
					double local_x[],
					double local_y[],
					int local_m,
					int n,
					int local_n,
					MPI_Comm comm){
	
	int rank, size, interval, remainder;
  	double time1, time2, time3;  // for timing measurements
	MPI_Init(NULL,NULL);
  	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	double *x;
	int local_i,j;
	int local_ok = 1;

	x = malloc(n*sizeof(double));
	MPI_Allgather(local_x,local_n,MPI_DOUBLE,x,local_n,MPI_DOUBLE,comm);

	for(local_i=0;local_i<local_m;local_i++){
		local_y[local_i] = 0.0;
		for (j=0;j<n;j++){
			local_y[local_i] += local_A[local_i*n+j]*x[j];
		}
	}
	free(x);
	MPI_Finalize();
}

void createMatx(){
     // generate random matrices
	srand (time(NULL));
	for(i=0;i<N;i++) 
		for(j=0;j<N;j++){ 
			A[i][j] = (rand() % 19 - 9);
	} // for

	srand (time(NULL)*2);
	for(i=0;i<N;i++) 
		for(j=0;j<N;j++){ 
			B[i][j] = (rand(srand (time(NULL))*2) % 19 - 9);
	} // for
}

int main(){
	createMatx();
	MatrixMult(A,B,res,N,N,N);
}