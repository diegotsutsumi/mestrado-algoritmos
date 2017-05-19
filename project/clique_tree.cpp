#include <iostream> 
#include "MarkovRandomField.h"

 
int main(int,char*[])
{
	MarkovRandomField mrf("input3.mrf");

	mrf.run();
	mrf.drawMRF();
	mrf.drawCliqueTree();
  	return 0;
}