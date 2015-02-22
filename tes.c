#define ROW 64
#define COL 64
#define UNDEFINED -999

#include<stdio.h>
#include<stdlib.h>

int matrix[ROW][COL];
int matrix2[ROW][COL];

void init(int mat[ROW][COL]){
    int i=0,j=0;
    for (i = 0; i < ROW; ++i)
    {
        for (j = 0; j < COL; ++j)
        {
            mat[i][j] = UNDEFINED;
        }
    }
}

void readFile(char* filename){
    FILE *file = fopen(filename, "r");

    int i=0,j=0,k=0;
    int num;
    while(fscanf(file, "%d", &num) > 0) {
        if(i<ROW){
            if(j<COL){
                matrix[i][j] = num;
                j++;
                if(j==COL){
                    j=0;
                    i++;
                }
            }
        } else{
            // printf("masuk\n");
            if(j<COL){
                matrix2[k][j] = num;
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

void printMatrix(int mat[ROW][COL]){
    int i=0,j=0;
    for (i = 0; i < ROW; ++i)
    {
        for (j = 0; j < COL; ++j)
        {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

int main()
{
    init(matrix);
    init(matrix2);
    readFile((char*)"2.txt");
    printMatrix(matrix);
    printf("\n");
    printMatrix(matrix2);

    return 0;
}