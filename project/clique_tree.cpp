#include <iostream> 
#include "MarkovRandomField.h"

 
int main(int,char*[])
{
	MarkovRandomField mrf("input2.mrf");

	mrf.test();
	mrf.drawMRF();
  	return 0;
}