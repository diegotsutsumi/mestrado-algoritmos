#include <iostream>
#include <fstream>
#include <exception>
#include "MarkovRandomField.h"

MRFFactor::MRFFactor()
{

}
MRFFactor::~MRFFactor()
{

}
MRFFactor::MRFFactor(std::vector<std::pair<std::string, int>>var, std::vector<int> val)
{
	variables = var;
	values = val;
	if(!is_valid())
	{
		std::cerr << "Invalid factor" << std::endl;
		variables.clear();
		values.clear();
	}
}

bool MRFFactor::is_valid()
{
	unsigned int valSize=1;
	for(int i=0;i<variables.size();i++)
	{
		valSize=valSize*variables[i].second;
	}
	if(values.size()!=valSize)
	{
		return false;
	}
	else
	{
		return true;
	}
}

std::vector<std::pair<std::string,int>> MRFFactor::getAssignment(unsigned int idx)
{
	std::vector<std::pair<std::string, int>> assRet;
	if(idx>=values.size())
	{
		return assRet;
	}
	
	assRet = variables;
	unsigned int den;
	for(int i=0;i<assRet.size();i++)
	{
		den=1;
		for(int j=0; j<i; j++)
		{
			den = den*variables[j].second;
		}
		assRet[i].second = idx / (den);
		assRet[i].second = assRet[i].second % variables[i].second;
	}
	return assRet;
}

unsigned int MRFFactor::getIndex(std::vector<std::pair<std::string, int>> assign)
{
	unsigned int retIdx=0;

	if(assign.size() != variables.size())
	{
		return -1;
	}
	unsigned int varNum=1;
	unsigned int assVarNum=1;
	for(int i=0;i<variables.size();i++)
	{
		if(assign[i].first!=variables[i].first || assign[i].second>=variables[i].second)
		{
			return -1;
		}
	}

	for(int i=0;i<assign.size();i++)
	{
		if(i==0)
		{
			retIdx = retIdx + assign[i].second;
		}
		else
		{
			unsigned int mul=1;
			for(int j=0; j<i; j++)
			{
				mul = mul*variables[j].second;
			}
			retIdx = retIdx + assign[i].second*mul;
		}
	}
	return retIdx;
}

MarkovRandomField::MarkovRandomField(std::string _inputPath)
{
	inputPath = _inputPath;

	if(!loadInput())
	{
		throw std::runtime_error("Invalid Input");
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
	unsigned int factorNumber;
	std::vector<std::string> factorVar;
	VertexProperty vp;
	EdgeProperty ep;
	std::vector<VertexDescriptor> vd_vector;
	std::pair<VertexIterator,VertexIterator> vertices;
	bool alreadyThere;

	if (inputFile.is_open())
	{
		getline(inputFile,line);
		try
		{
			factorNumber = std::stoi(line);

			for(int i=0; i<factorNumber; i++)
			{

				if(getline(inputFile,line))
				{
					factorVar.clear();
					factorVar = splitString(line, " ");
					vd_vector.clear();
					for(int j=0;j<factorVar.size();j++)
					{
						vp.name = factorVar[j];
						alreadyThere = false;
						for (vertices = boost::vertices(MRFGraph); vertices.first != vertices.second; ++vertices.first)
						{
							if(MRFGraph[*vertices.first].name == vp.name)
							{
								alreadyThere = true;
								break;
							}
						}
  
						if(!alreadyThere)
						{
							vd_vector.push_back(MRFGraph.add_vertex(vp));
						}
						else
						{
							vd_vector.push_back(*vertices.first);
						}
					}
					ep.weight = 1;
					for(int j=0; j<vd_vector.size();j++)
					{
						for(int k=j; k<vd_vector.size(); k++)
						{
							if(j!=k && !edge(vd_vector[j],vd_vector[k],MRFGraph).second)
							{
								MRFGraph.add_edge(vd_vector[j],vd_vector[k],ep);
							}
						}
					}
				}
				else
				{
					throw std::runtime_error("invalid input");
				}
			}
		}
		catch (const std::invalid_argument& ia)
		{
			std::cerr << "Invalid argument STOI Invalid Input File: " << ia.what() << std::endl;
			inputFile.close();
			return false;
		}
		catch ( const std::runtime_error& e )
		{
			std::cerr << "Invalid Input File: " << e.what() << std::endl;
			inputFile.close();
			return false;
		}
		inputFile.close();
	}
	else
	{
		std::cerr << "Could not ope file" << std::endl;
	}
	return true;
}

bool MarkovRandomField::drawMRF()
{
	std::ofstream dotfile("MRF.dot");
	write_graphviz (dotfile, MRFGraph, boost::make_label_writer(boost::get(&VertexProperty::name, MRFGraph)),boost::make_label_writer(boost::get(&EdgeProperty::weight, MRFGraph)));
	return true;
}

std::vector<std::string> MarkovRandomField::splitString(std::string str, std::string delimiter)//O(str.size())
{
	std::vector<std::string> parts;
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