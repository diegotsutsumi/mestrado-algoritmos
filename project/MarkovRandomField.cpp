#include <iostream>
#include <fstream>
#include "Exception.h"
#include "MarkovRandomField.h"

MarkovRandomField::MarkovRandomField(std::string _inputPath)//O(n+11m)
{
	inputPath = _inputPath;

	if(!loadInput())
	{
		throw Exception("Invalid Input");
	}
}

MarkovRandomField::~MarkovRandomField()
{
}

bool MarkovRandomField::loadInput()
{
	if(inputPath=="")
	{
		return false;
	}
	if(MRFGraph.num_vertices()!=0)
	{
		//TODO: Clear Graph
	}

	std::ifstream inputFile(inputPath);
	std::string line;
	if (inputFile.is_open())
	{
		getline(inputFile,line)
		try
		{
			while ( getline (inputFile,line) )
			{
				snitchNum = std::stoi(line);
				if(snitchNum == snitchSerialNum)
				{
					alreadyExists=true;
					break;
				}
			}
		}
		catch (const std::invalid_argument& ia)
		{
			std::cerr << "ActiveSnitch 1 Invalid argument STOI: " << ia.what() << '\n';
			inputFile.close();
			return false;
		}
		inputFile.close();
		getline(inputFile,CNFStr); //O(n)
		inputFile.close();
	}
	else
	{
		std::cerr << "Could not ope file" << std::endl;
	}
	return true;

}