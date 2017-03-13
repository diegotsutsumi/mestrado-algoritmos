#include "CNFChecker.h"
#include <iostream>
#include <vector>
#include "Exception.h"

CNFChecker::CNFChecker(std::string _inputPath)//O(n+11m)
{
	inputPath = _inputPath;
	CNFStr="";
	ValStr="";
	if(!loadInput())
	{
		throw Exception("Invalid Input");
	}
}

CNFChecker::CNFChecker(std::string _CNFStr, std::string _ValStr)//O(n+11m)
{
	inputPath = "";
	CNFStr=_CNFStr;
	ValStr=_ValStr;
	if(!loadInput())
	{
		throw Exception("Invalid Input");
	}
}

CNFChecker::~CNFChecker()
{
}

std::string CNFChecker::run() //O(3n-2)
{
	bool val;

	std::vector<std::string> CNFor = splitString(CNFStr.substr(1,CNFStr.size()-2),")∨("); //O(n-2)
	std::vector<std::string> CNFand;

	for(unsigned int i=0; i<CNFor.size(); i++) //O(2n)
	{
		CNFand = splitString(CNFor[i],"∧");
		val=true;
		for(unsigned int j=0; j<CNFand.size(); j++)
		{
			if(CNFand[j][0]=='!')
			{
				val = !(ValArray[std::stoi(CNFand[j].substr(1, CNFand[j].size()-1))-1]);
			}
			else
			{
				val = ValArray[std::stoi(CNFand[j])-1];
			}

			if(!val)
			{
				break;
			}
		}
		if(val)
		{
			return "True";
		}
	}
	return "False";
}

unsigned int CNFChecker::getLastVar()
{
	return (unsigned int)ValArray.size();
}

bool CNFChecker::loadInput()//O(n+11m)
{
	std::ifstream inputFile;
	if(CNFStr=="" || ValStr=="")
	{
		try
		{
			if(inputPath=="")
			{
				throw Exception("Empty Input");
			}
			inputFile.open(inputPath, std::ifstream::in);
			getline(inputFile,CNFStr); //O(n)
			getline(inputFile,ValStr); //O(5m)
			inputFile.close();
		}
		catch (Exception &e)
		{
			return false;
		}
	}

	if(ValArray.size()==0)
	{
		std::vector<string> ValSplit = splitString(ValStr," "); //O(5m)
		for(unsigned int i=0;i<ValSplit.size();i++) //O(m)
		{
			if(ValSplit[i]=="True")
			{
				ValArray.push_back(true);
			}
			else
			{
				ValArray.push_back(false);
			}
		}
	}
	std::cout << "Given CNF: " << CNFStr << std::endl;
	std::cout << "Given Values: " << ValStr << std::endl;
	return true;
}

void CNFChecker::setValArray(std::vector<bool> _ValArray)//O(m) ?
{
	ValArray = _ValArray;
}

std::vector<string> CNFChecker::splitString(std::string str, std::string delimiter)//O(str.size())
{
	std::vector<string> parts;
	size_t pos = 0;
	std::string token;
	if(delimiter=="" || str=="")
	{
		parts.push_back(str);
		return parts;
	}

	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		token = str.substr(0, pos);
		parts.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	if(str.size()>0)
	{
		parts.push_back(str);
	}

	return parts;
}