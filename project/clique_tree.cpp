#include <iostream> 
#include "MarkovRandomField.h"

 
int main(int,char*[])
{
	MarkovRandomField mrf("input.mrf");

	mrf.test();
	mrf.drawMRF();
  	return 0;
}