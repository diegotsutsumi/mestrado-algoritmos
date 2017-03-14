#include "Exception.h"
#include <iostream>
#include <string>
#include "CNFChecker.h"
//#include "Socket.h"

using namespace std;

//O(n+11m) + O(3n-2) + O(m) + O(2^m)*O(2m)*O(3n-2)
//O(4n+12m-2) + O((3n-2)(m^2)(2^m))

//Simplifying ---> O(n*m^2*2^m)

int main (int argc, char* argv[])
{
	try
	{
		cout << endl;
		CNFChecker checker("input.cnf");//O(n+11m);
		//CNFChecker checker("(1∧!2)∨(1∧3)∨(!2∧4)∨(3∧5∧!6)", "True True False False True False");//O(n+11m)
		
		std::string response = checker.run();//O(3n-2)
		cout << endl << endl;
		cout << "Response for given valuation: " << response << endl;
		if(response!="True")
		{
			unsigned int varSize = checker.getLastVar();
			std::vector<bool> values;
			for(int i=0; i<varSize; i++)//O(m)
			{
				values.push_back(false);
			}

			unsigned int iSize=(1<<varSize);//O(m)
			for(int i=0; i<iSize; i++)//O(2^m)
			{
				for(int j=0;j<varSize;j++)//O(m)
				{
					values[j]=(bool)(((1<<j) & i)>>j);//O(2m)
				}
				checker.setValArray(values);
				response = checker.run(); //O(3n-2)
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