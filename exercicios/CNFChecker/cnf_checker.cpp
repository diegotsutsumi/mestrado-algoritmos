#include "Exception.h"
#include <iostream>
#include <string>
#include "CNFChecker.h"
//#include "Socket.h"

using namespace std;

int main (int argc, char* argv[])
{
	try
	{
		//CNFChecker checker("input.cnf");
		CNFChecker checker("1∧!2∨1∧3∨!2∧4∨3∧5∧!6", "True True False False True False");
		cout << checker.run() << endl;
	}
	catch(Exception &e)
	{
		cout << "Exception caught: " << e.description() << endl;
	}

	return 0;
}