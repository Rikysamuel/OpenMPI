#include<stdio.h>
#include<mpi.h>

#define ROW 512
#define COL 512
#define MASTER_TO_SLAVE_TAG 1
#define SLAVE_TO_MASTER_TAG 4

int rank;
int size;
int i, j, k;
double mat_a[ROW][COL];
double mat_b[ROW][COL];
double mat_res[ROW][COL];
double start_time;
double end_time;
int low_bound;
int upper_bound;
int portion;

MPI_Status status;
MPI_Request request;

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

//print matrix to the screen
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
            printf("%8.2f  ", mat_res[i][j]);
    }
    printf("\n\n");
}

void printSerial(){
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
            printf("%8.2f  ", mat_res[i][j]);
    }
    printf("\n\n");
}

// Multiply matrix - serial
void multiplyMatrix()
{
  int i, j, k;
  for(i=0;i<ROW;i++)
    for(j=0;j<COL;j++)
        for(k=0,mat_res[i][j]=0; k<ROW; k++)
            mat_res[i][j] += mat_a[i][k] * mat_b[k][j];
}

int main(int argc, char *argv[])
{
    // initialization
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        readFile("5.txt");
        start_time = MPI_Wtime();
        
        if(size==1){
            portion=1;
        } else{
            portion = (ROW / (size - 1));
        }
        for (i = 1; i < size; i++) {
            low_bound = (i - 1) * portion;

            if (((i + 1) == size) && ((ROW % (size - 1)) != 0)) {
                upper_bound = ROW;
            } else {
                upper_bound = low_bound + portion;
            }
            
            // Send low_bound, upper_bound, and the cell to the slaves
            MPI_Isend(&low_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &request);
            MPI_Isend(&upper_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &request);
            MPI_Isend(&mat_a[low_bound][0], (upper_bound - low_bound) * COL, MPI_DOUBLE, i, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &request);
        }
    }

    // Broadcast mat_b to all process
    MPI_Bcast(&mat_b, ROW*COL, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank > 0) {
        MPI_Recv(&low_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&upper_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&mat_a[low_bound][0], (upper_bound - low_bound) * COL, MPI_DOUBLE, 0, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &status);
        
        for (i = low_bound; i < upper_bound; i++) {
            for (j = 0; j < COL; j++) {
                for (k = 0; k < ROW; k++) {
                    mat_res[i][j] += (mat_a[i][k] * mat_b[k][j]);
                }
            }
        }


        MPI_Isend(&low_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &request);
        MPI_Isend(&upper_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &request);
        MPI_Isend(&mat_res[low_bound][0], (upper_bound - low_bound) * COL, MPI_DOUBLE, 0, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &request);
    }

    // master gathers the result of the multiplication from the slaves
    if (rank == 0) {
        for (i = 1; i < size; i++) {
            MPI_Recv(&low_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(&upper_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &status);
            MPI_Recv(&mat_res[low_bound][0], (upper_bound - low_bound) * COL, MPI_DOUBLE, i, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &status);
        }
        end_time = MPI_Wtime();
        printf("\nRunning Time = %f\n\n", end_time - start_time);
        // print();
    }
 
    if (size==1){
        start_time = MPI_Wtime();
        multiplyMatrix();
        end_time = MPI_Wtime();
        // printSerial();
        printf("\nSerial Running Time = %f\n\n", end_time - start_time);
    }
    
    MPI_Finalize();
    return 0;
}