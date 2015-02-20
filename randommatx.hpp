#define ROW 4
#define COL 4

#ifndef _RANDOMMATX_H
#define _RANDOMMATX_H

#include <iostream> 
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

class randommatx{
	private:
		int matrix[ROW][COL];
	public:
		randommatx();
		int random();
		void print();
		void init();
		void readMatrix(char* filename);
};

#endif