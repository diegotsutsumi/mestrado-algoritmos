#include <iostream>
#include <fstream>
#include <exception>
#include "MarkovRandomField.h"

MarkovRandomField::MarkovRandomField(std::string _inputPath)//O(n+11m)
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