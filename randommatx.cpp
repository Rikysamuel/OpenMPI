#include "randommatx.hpp"

randommatx::randommatx(){
	// init();
}

void randommatx::init(){
	srand (time(NULL));

	for (int i=0; i<ROW; i++){
		for (int j=0; j<COL; j++)
	    {
	      matrix[i][j]=random();
	    }
	}
}

int randommatx::random(){
	return (rand() % 19 - 9);
}

void randommatx::print(){
	for (int i=0; i<ROW; i++){
		for (int j=0; j<COL; j++)
	    {
	      cout << matrix[i][j] << " ";
	    }
	    cout << endl;
	}
}

void randommatx::readMatrix(char* filename){
	int val,i=0,j=0; 
	char *inname = (char*) "a.txt"; 
	ifstream infile(inname); 
	if (!infile) {
		cout << "There was a problem opening file " << inname << " for reading." << endl; 
		return;
	} 
	cout << "Opened " << inname << " for reading." << endl; 
	while (infile >> val) { 
		if(j<4){
			matrix[i][j]=val;
			j++;
		}
		if(j==4){
			i++;
			j=0;
		}
	} 
}