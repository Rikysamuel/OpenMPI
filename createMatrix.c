#define ROW 4
#define COL 4

#include <stdio.h>

int matrix[ROW][COL];

int main(){
	int i=0,j=0;
	srand (time(NULL));
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            matrix[i][j] = rand() % 19 - 9;
        }
    }

    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            printf("%d ",matrix[i][j]);
        }
        printf("\n");
    }

    srand (time(NULL)*3/2);
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            matrix[i][j] = rand() % 19 - 9;
        }
    }

    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            printf("%d ",matrix[i][j]);
        }
        printf("\n");
    }
}