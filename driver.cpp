#include "randommatx.hpp"

int main(){
	randommatx *ran = new randommatx();
	// randommatx *ran2 = new randommatx();
	ran->init();
	// ran2->init();
	// ran->readMatrix((char*)"a.txt");
	ran->print();
	// cout << endl;
	// ran2->print();
}