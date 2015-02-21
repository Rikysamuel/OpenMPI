/**********************************************************************************************
* Matrix Multiplication Program using MPI.
*
* Viraj Brian Wijesuriya - University of Colombo School of Computing, Sri Lanka.
* 
* Works with any type of two matrixes [A], [B] which could be multiplied to produce a matrix [c].
*
* Master process initializes the multiplication operands, distributes the muliplication 
* operation to worker processes and reduces the worker results to construct the final output.
*  
************************************************************************************************/
#include<stdio.h>
#include<mpi.h>
#define ROW 4 //rows of input
#define COL 4 //columns of input
#define MASTER_TO_SLAVE_TAG 1 //tag for messages sent from master to slaves
#define SLAVE_TO_MASTER_TAG 4 //tag for messages sent from slaves to master

int rank; //process rank
int size; //number of processes
int i, j, k; //helper variables
double mat_a[ROW][COL]; //declare input [A]
double mat_b[ROW][COL]; //declare input [B]
double mat_result[ROW][COL]; //declare output [C]
double start_time; //hold start time
double end_time; // hold end time
int low_bound; //low bound of the number of rows of [A] allocated to a slave
int upper_bound; //upper bound of the number of rows of [A] allocated to a slave
int portion; //portion of the number of rows of [A] allocated to a slave
MPI_Status status; // store status of a MPI_Recv
MPI_Request request; //capture request of a MPI_Isend

// read matrix from file
void readFile(char* filename)
{
    FILE *file = fopen(filename, "r");

    int i=0,j=0,k=0;
    int num;
    while(fscanf(file, "%d", &num) > 0) {
        if(i<ROW){
            if(j<COL){
                mat_a[i][j] = num;
                j++;
                if(j==COL){
                    j=0;
                    i++;
                }
            }
        } else{
            // printf("masuk\n");
            if(j<COL){
                mat_b[k][j] = num;
                j++;
                if(j==COL){
                    j=0;
                    i++;
                    k++;
                }
            }
        }
    }
    fclose(file);
}

//print matrix to screen
void print()
{
    //matrix_a
    for (i = 0; i < ROW; i++) {
        printf("\n");
        for (j = 0; j < COL; j++)
            printf("%8.2f  ", mat_a[i][j]);
    }
    printf("\n\n\n");

    //matrix_b
    for (i = 0; i < ROW; i++) {
        printf("\n");
        for (j = 0; j < COL; j++)
            printf("%8.2f  ", mat_b[i][j]);
    }

    //matrix_result
    printf("\n\n\n");
    for (i = 0; i < ROW; i++) {
        printf("\n");
        for (j = 0; j < COL; j++)
            printf("%8.2f  ", mat_result[i][j]);
    }
    printf("\n\n");
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // initialization
    if (rank == 0) {
        readFile("1.txt");
        start_time = MPI_Wtime();
        for (i = 1; i < size; i++) {
            portion = (ROW / (size - 1));
            low_bound = (i - 1) * portion;
            if (((i + 1) == size) && ((ROW % (size - 1)) != 0)) {//if rows of [A] cannot be equally divided among slaves
                upper_bound = ROW; //last slave gets all the remaining rows
            } else {
                upper_bound = low_bound + portion; //rows of [A] are equally divisable among slaves
            }
            //send the low bound first without blocking, to the intended slave
            MPI_Isend(&low_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &request);
            //next send the upper bound without blocking, to the intended slave
            MPI_Isend(&upper_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &request);
            //finally send the allocated row portion of [A] without blocking, to the intended slave
            MPI_Isend(&mat_a[low_bound][0], (upper_bound - low_bound) * COL, MPI_DOUBLE, i, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &request);
        }
    }
    //broadcast [B] to all the slaves
    MPI_Bcast(&mat_b, ROW*COL, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    /* work done by slaves*/
    if (rank > 0) {
        //receive low bound from the master
        MPI_Recv(&low_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &status);
        //next receive upper bound from the master
        MPI_Recv(&upper_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &status);
        //finally receive row portion of [A] to be processed from the master
        MPI_Recv(&mat_a[low_bound][0], (upper_bound - low_bound) * COL, MPI_DOUBLE, 0, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &status);
        for (i = low_bound; i < upper_bound; i++) {//iterate through a given set of rows of [A]
            for (j = 0; j < COL; j++) {//iterate through columns of [B]
                for (k = 0; k < ROW; k++) {//iterate through rows of [B]
                    mat_result[i][j] += (mat_a[i][k] * mat_b[k][j]);
                }
            }
        }
        //send back the low bound first without blocking, to the master
        MPI_Isend(&low_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &request);
        //send the upper bound next without blocking, to the master
        MPI_Isend(&upper_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &request);
        //finally send the processed portion of data without blocking, to the master
        MPI_Isend(&mat_result[low_bound][0], (upper_bound - low_bound) * COL, MPI_DOUBLE, 0, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &request);
    }
    /* master gathers processed work*/
    if (rank == 0) {
        for (i = 1; i < size; i++) {// untill all slaves have handed back the processed data
            //receive low bound from a slave
            MPI_Recv(&low_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &status);
            //receive upper bound from a slave
            MPI_Recv(&upper_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &status);
            //receive processed data from a slave
            MPI_Recv(&mat_result[low_bound][0], (upper_bound - low_bound) * COL, MPI_DOUBLE, i, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &status);
        }
        end_time = MPI_Wtime();
        printf("\nRunning Time = %f\n\n", end_time - start_time);
        print();
    }
    MPI_Finalize(); //finalize MPI operations
    return 0;
}