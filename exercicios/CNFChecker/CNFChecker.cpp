#include "CNFChecker.h"
#include <iostream>
#include <vector>
#include "Exception.h"

CNFChecker::CNFChecker(std::string _inputPath)
{
	inputPath = _inputPath;
	CNFStr="";
	ValStr="";
}

CNFChecker::CNFChecker(std::string _CNFStr, std::string _ValStr)
{
	inputPath = "";
	CNFStr=_CNFStr;
	ValStr=_ValStr;
}

CNFChecker::~CNFChecker()
{
}

std::string CNFChecker::run()
{
	std::ifstream inputFile;
	std::vector<bool> ValArray;

	if(CNFStr=="" || ValStr=="")
	{
		try
		{
			if(inputPath=="")
			{
				throw Exception("Empty Input");
			}
			inputFile.open(inputPath, std::ifstream::in);
			getline(inputFile,CNFStr);
			getline(inputFile,ValStr);
			inputFile.close();
		}
		catch (Exception &e)
		{
			return e.description();
		}
	}

	std::vector<string> ValSplit = splitString(ValStr," ");

	for(unsigned int i=0;i<ValSplit.size();i++)
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

	std::vector<std::string> CNFor = splitString(CNFStr,"∨");

	std::vector<std::string> CNFand;
	bool val;
	for(unsigned int i=0; i<CNFor.size(); i++)
	{
		std::cout << "And Expression: " << CNFor[i] << std::endl;
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

std::vector<string> CNFChecker::splitString(std::string str, std::string delimiter)
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