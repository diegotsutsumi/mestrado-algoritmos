#include <iostream>
#include <fstream>
#include <exception>
#include <algorithm>
#include "MarkovRandomField.h"

MarkovRandomField::MarkovRandomField(std::string _inputPath)
{
	inputPath = _inputPath;
	counter = 0;

	if(!loadInput())
	{
		mrfgraph.clear();
		cliquetree.clear();
		factors.clear();
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
	if(mrfgraph.num_vertices()!=0)
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

	std::vector<Factor> factors_aux;
	std::vector<double> values;
	Factor toBePushed;

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
					std::vector<std::string> facSplit;
					FactorVarVector mrff;
					for(unsigned int j=0;j<factorVar.size();j++)
					{
						facSplit.clear();
						facSplit = splitString(factorVar[j], "-");

						mrff.push_back(std::make_pair(facSplit[0],std::stoi(facSplit[1])));

						vp.name = facSplit[0];
						alreadyThere = false;
						unsigned int k=0;
						for (vertices = boost::vertices(mrfgraph); vertices.first != vertices.second; ++vertices.first)
						{
							if(mrfgraph[*vertices.first].name == vp.name)
							{
								alreadyThere = true;
								break;
							}
							k++;
						}
  
						if(!alreadyThere)
						{
							vd_vector.push_back(mrfgraph.add_vertex(vp));
							std::vector<unsigned int> mrfV = {i};
							mrf_variables.push_back(std::make_pair(std::make_pair(facSplit[0],std::stoi(facSplit[1])),mrfV));
						}
						else
						{
							vd_vector.push_back(*vertices.first);
							(mrf_variables[k].second).push_back(i);
						}
					}

					toBePushed.setVariables(mrff);
					toBePushed.setCounter(&counter);
					factors_aux.push_back(toBePushed);
					ep.weight = 1;
					for(unsigned int j=0; j<vd_vector.size();j++)
					{
						for(unsigned int k=j; k<vd_vector.size(); k++)
						{
							if(j!=k && !edge(vd_vector[j],vd_vector[k],mrfgraph).second)
							{
								mrfgraph.add_edge(vd_vector[j],vd_vector[k],ep);
							}
						}
					}
				}
				else
				{
					throw std::runtime_error("Wrong number of Factor Variable Lines");
				}
			}

			for(unsigned int i=0; i<factorNumber; i++)
			{
				if(getline(inputFile,line))
				{
					factorVar.clear();
					factorVar = splitString(line, " ");

					values.clear();
					for(unsigned int j=0;j<factorVar.size();j++)
					{
						values.push_back(std::stod(factorVar[j]));
					}
					factors_aux[i].setValues(values);
					if(!factors_aux[i].isValid())
					{
						throw std::runtime_error("Probably mismatched variables / values sizes");
					}
				}
				else
				{
					throw std::runtime_error("Wrong number of Factor Value Lines");
				}
			}
			factors = factors_aux;
		}
		catch (const std::invalid_argument& ia)
		{
			std::cerr << "Invalid argument: " << ia.what() << std::endl;
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
		return false;
	}
	return true;
}

bool MarkovRandomField::drawMRF()
{
	std::ofstream dotfile("MRF.dot");
	write_graphviz (dotfile, mrfgraph, boost::make_label_writer(boost::get(&VertexProperty::name, mrfgraph)),boost::make_label_writer(boost::get(&EdgeProperty::weight, mrfgraph)));
	system("dot -Tpng MRF.dot -o MRF.png");
	ulink("MRF.dot");
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

Factor MarkovRandomField::dumbQuery(std::vector<std::string> * query)
{
	Factor newFactor;
	newFactor.setCounter(&counter);
	unsigned int i=0;
	for(i=0,newFactor=factors[0];i<factors.size();i++)
	{
		if(i+1<factors.size())
		{
			newFactor = newFactor*factors[i+1];
		}
	}

	for(i=0;i<mrf_variables.size();i++)
	{
		if(std::find(query->begin(), query->end(), ((mrf_variables[i]).first).first)==query->end())
		{
			newFactor.marginalize(mrf_variables[i].first);
		}
	}
	newFactor.normalize();
	std::cout << "Dumb Query Number of Operations: " << counter << std::endl;
	return newFactor;
}

Factor MarkovRandomField::variableEliminationQuery(std::vector<std::string> * query)
{
	std::deque<std::pair<FactorVar,std::vector<unsigned int>>> mrfVar;
	std::pair<FactorVar,std::vector<unsigned int>> margVar;

	for(unsigned int i=0;i<mrf_variables.size();i++)
	{
		if(std::find(query->begin(), query->end(), ((mrf_variables[i]).first).first)==query->end())
		{
			mrfVar.push_back(mrf_variables[i]);
		}
	}

	std::sort(mrfVar.begin(),mrfVar.end(),mrfVarComp());

	Factor newFactor;
	std::vector<Factor> newFactors = factors;
	newFactor.setCounter(&counter);
	std::vector<unsigned int> multFactorsVector;

	unsigned int newFacIdx;

	while(!mrfVar.empty())
	{
		unsigned int j;
		margVar = mrfVar.front();
		mrfVar.pop_front();

		multFactorsVector.clear();
		for(j=0,newFactor=newFactors[(margVar.second)[0]];(j+1)<(margVar.second).size();j++)
		{
			if(j==0)
			{
				multFactorsVector.push_back((margVar.second)[0]);
				multFactorsVector.push_back((margVar.second)[1]);
			}
			else
			{
				multFactorsVector.push_back((margVar.second)[j+1]);
			}
			newFactor = newFactor * newFactors[(margVar.second)[j+1]];
		}

		std::sort(multFactorsVector.begin(),multFactorsVector.end());

		newFactor.marginalize(margVar.first);

		for(int intJ=((int)(multFactorsVector.size())-1);intJ>=0;intJ--)
		{
			newFactors.erase(newFactors.begin()+multFactorsVector[intJ]);
		}
		newFactors.push_back(newFactor);
		newFactors.shrink_to_fit();
		newFacIdx = newFactors.size() - 1;

		for(j=0; j<mrfVar.size(); j++)
		{
			std::vector<unsigned int> eraseIndexes;
			for(int k=((int)((mrfVar[j].second).size())-1); k>=0;k--)
			{
				bool erasing = false;
				unsigned int comp = 0;
				for(unsigned int l=0; l<multFactorsVector.size();l++)
				{
					if((mrfVar[j].second)[k] > multFactorsVector[l])
					{
						comp++;
					}
					if((mrfVar[j].second)[k] == multFactorsVector[l])
					{
						eraseIndexes.push_back(k);
						erasing = true;
						break;
					}
				}
				if(!erasing && comp>0)
				{
					(mrfVar[j].second)[k] = (mrfVar[j].second)[k] - comp;
				}
			}

			for(unsigned int k=0; k<eraseIndexes.size();k++)
			{
				(mrfVar[j].second).erase((mrfVar[j].second).begin()+eraseIndexes[k]);
			}
			if(eraseIndexes.size()>0)
			{
				(mrfVar[j].second).push_back(newFacIdx);
			}
			(mrfVar[j].second).shrink_to_fit();
		}
		std::sort(mrfVar.begin(),mrfVar.end(),mrfVarComp());
	}
	
	newFactor=newFactors[0];
	for(unsigned int i=0;(i+1)<newFactors.size();i++)
	{
		newFactor = newFactor * newFactors[i+1];
	}
	std::cout << "VE Query Number of Operations: " << counter << std::endl;
	newFactor.normalize();
	return newFactor;
}

void MarkovRandomField::clearOpCounter()
{
	counter=0;
}

void MarkovRandomField::test()
{
	std::vector<std::string> asking = {"CM","CA"};
	Factor queryFac = dumbQuery(&asking);
	queryFac.printFactor();
	
	clearOpCounter();
	queryFac = variableEliminationQuery(&asking);
	queryFac.printFactor();
}

Factor MarkovRandomField::query(std::vector<std::string> * query)
{
	//return dumbQuery(query);
	return variableEliminationQuery(query);
}