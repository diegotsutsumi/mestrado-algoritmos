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
		cout << endl;
		CNFChecker checker("input.cnf");
		//CNFChecker checker("(1∧!2)∨(1∧3)∨(!2∧4)∨(3∧5∧!6)", "True True False False True False");
		
		std::string response = checker.run();
		cout << endl << endl;
		cout << "Response for given valuation: " << response << endl;
		if(response!="True")
		{
			unsigned int varSize = checker.getLastVar();
			std::vector<bool> values;
			for(int i=0; i<varSize; i++)
			{
				values.push_back(false);
			}

			unsigned int iSize=(1<<varSize);
			for(int i=0; i<iSize; i++)
			{
				for(int j=0;j<varSize;j++)
				{
					values[j]=(bool)(((1<<j) & i)>>j);
				}
				checker.setValArray(values);
				response = checker.run();
				if(response=="True")
				{
					break;
				}
			}
		}

		if(response=="True")
		{
			cout << "CNF is satisfiable" << endl;
		}
		else
		{
			cout << "CNF is NOT satisfiable" << endl;
		}
		cout << endl;
	}
	catch(Exception &e)
	{
		cout << "Exception caught: " << e.description() << endl;
	}

	return 0;
}