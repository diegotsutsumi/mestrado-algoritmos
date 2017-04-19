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
MRFFactor::MRFFactor(FactorVarVector var, std::vector<double> val)
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
	for(unsigned int i=0;i<variables.size();i++)
	{
		valSize=valSize*variables[i].second;
	}
	if(values.size()!=valSize)
	{
		return false;
	}
	return true;
}


FactorVarVector MRFFactor::getVariables()
{
	return variables;
}

FactorVarVector MRFFactor::getAssignment(unsigned int idx)
{
	FactorVarVector assRet;
	if(idx>=values.size())
	{
		return assRet;
	}
	
	assRet = variables;
	unsigned int den;
	for(unsigned int i=0;i<assRet.size();i++)
	{
		den=1;
		for(unsigned int j=0; j<i; j++)
		{
			den = den*variables[j].second;
		}
		assRet[i].second = idx / (den);
		assRet[i].second = assRet[i].second % variables[i].second;
	}
	return assRet;
}

unsigned int MRFFactor::getIndex(FactorVarVector assign)
{
	unsigned int retIdx=0;

	if(assign.size() != variables.size())
	{
		return -1;
	}

	for(unsigned int i=0;i<variables.size();i++)
	{
		if(assign[i].first!=variables[i].first || assign[i].second>=variables[i].second)
		{
			return -1;
		}
	}

	for(unsigned int i=0;i<assign.size();i++)
	{
		if(i==0)
		{
			retIdx = retIdx + assign[i].second;
		}
		else
		{
			unsigned int mul=1;
			for(unsigned int j=0; j<i; j++)
			{
				mul = mul*variables[j].second;
			}
			retIdx = retIdx + assign[i].second*mul;
		}
	}
	return retIdx;
}

double MRFFactor::getValue(FactorVarVector assignment)
{
	unsigned int idx = getIndex(assignment);
	if(idx >= values.size())
	{
		throw std::invalid_argument("Invalid Argument");
	}
	return values[idx];
}

double MRFFactor::getValue(unsigned int idx)
{
	if(idx >= values.size())
	{
		throw std::invalid_argument("Invalid Argument");
	}
	return values[idx];
}

void MRFFactor::printFactor()
{
	for(int i=((int)(variables.size())-1);i>=0;i--)
	{
		std::cout << variables[i].first << " ";
	}
	std::cout << std::endl;
	FactorVarVector ass;
	for(unsigned int i=0;i<values.size();i++)
	{
		ass = getAssignment(i);
		for(int j=((int)(ass.size())-1);j>=0;j--)
		{
			std::cout << ass[j].second << "     ";
		}
		std::cout << " | ";
		std::cout << getValue(i) << std::endl;
	}
}

MRFFactor MRFFactor::factorEliminationOperation(MRFFactor *a, FactorVar *eliminateVar, std::function<int(MRFFactor*,FactorVarVector*,unsigned int,unsigned int,unsigned int)> operation)
{
	FactorVarVector new_ass, ass;
	FactorVarVector new_variables;
	std::vector<double> new_values;

	ass = variables;
	FactorVarVector::iterator iter = std::find_if(ass.begin(), ass.end(), comp(eliminateVar->first));
	unsigned int eliminateIndex = (iter - ass.begin());
	if(iter==ass.end())
	{
		//Variable not found
		return *a;
	}

	for(unsigned int i=0; i<variables.size(); i++)
	{
		if(eliminateVar->first != variables[i].first)
		{
			new_variables.push_back(variables[i]);
		}
	}
	unsigned int valSize=1;
	for(unsigned int i=0;i<new_variables.size();i++)
	{
		valSize=valSize*new_variables[i].second;
	}
	new_values.resize(valSize,0);

	MRFFactor newFactor(new_variables, new_values);
	for(unsigned int i=0;i<valSize;i++)
	{
		new_ass = newFactor.getAssignment(i);

		for(unsigned int j=0;j<ass.size();j++)
		{
			if(j<eliminateIndex)
			{
				ass[j].second = new_ass[j].second;
			}
			else if(j>eliminateIndex)
			{
				ass[j].second = new_ass[j-1].second;
			}
		}

		newFactor.values[i] = operation(a,&ass,eliminateIndex,variables[eliminateIndex].second,eliminateVar->second);
	}
	return newFactor;
}

MRFFactor MRFFactor::factorBinaryOperation(MRFFactor *a, MRFFactor *b, std::function<int(int,int)> operation)
{
	MRFFactor ret;
	if(!a->is_valid() || !b->is_valid())
	{
		std::cerr << "Invalid B or A: " << std::endl;
		return ret;
	}
	for(unsigned int i=0;i<a->variables.size();i++)
	{
		ret.variables.push_back(a->variables[i]);
	}
	for(unsigned int i=0;i<b->variables.size();i++)
	{
		if(std::find(ret.variables.begin(), ret.variables.end(), b->variables[i])==ret.variables.end())
		{
			ret.variables.push_back(b->variables[i]);
		}
	}
	unsigned int valSize=1;
	for(unsigned int i=0;i<ret.variables.size();i++)
	{
		valSize=valSize*ret.variables[i].second;
	}
	FactorVarVector ass, assB, assA;

	assB = b->variables;
	assA = a->variables;
	ret.values.resize(valSize, 0);
	for(unsigned int i=0;i<valSize;i++)
	{
		ass = ret.getAssignment(i);

		for(unsigned int j=0; j<assB.size(); j++)
		{
			FactorVarVector::iterator iter = std::find_if(ass.begin(), ass.end(), comp(assB[j].first));
			if(iter != ass.end())
			{
				assB[j].second = iter->second;
			}
			else
			{
				std::cerr << "ERROR, should never end up here" << std::endl;
				return ret;
			}
		}
		
		for(unsigned int j=0; j<assA.size(); j++)
		{
			FactorVarVector::iterator iter = std::find_if(ass.begin(), ass.end(), comp(assA[j].first));
			if(iter != ass.end())
			{
				assA[j].second = iter->second;
			}
			else
			{
				std::cerr << "ERROR, should never end up here" << std::endl;
				return ret;
			}
		}
		ret.values[i] = operation(b->getValue(assB),a->getValue(assA));
	}
	return ret;
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

			for(unsigned int i=0; i<factorNumber; i++)
			{

				if(getline(inputFile,line))
				{
					factorVar.clear();
					factorVar = splitString(line, " ");
					vd_vector.clear();
					for(unsigned int j=0;j<factorVar.size();j++)
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
					for(unsigned int j=0; j<vd_vector.size();j++)
					{
						for(unsigned int k=j; k<vd_vector.size(); k++)
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