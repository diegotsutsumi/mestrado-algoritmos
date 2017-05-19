#include <iostream>
#include <fstream>
#include <exception>
#include <algorithm>
#include "MarkovRandomField.h"
#include <boost/graph/bron_kerbosch_all_cliques.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>

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
					for(unsigned int j=0; j<vd_vector.size();j++)
					{
						for(unsigned int k=j; k<vd_vector.size(); k++)
						{
							if(j!=k && !edge(vd_vector[j],vd_vector[k],mrfgraph).second)
							{
								mrfgraph.add_edge(vd_vector[j],vd_vector[k],1);
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
	return draw(mrfgraph,"MRF",false);
}

bool MarkovRandomField::drawCliqueTree()
{
	return draw(cliquetree,"CliqueTree",true);
}

bool MarkovRandomField::draw(Graph inGraph, std::string name, bool drawWeight)
{
	std::string dotName = name+".dot";
	std::string pngName = name+".png";
	std::string cmd = "dot -Tpng " + dotName + " -o " + pngName;
	std::ofstream dotfile(dotName);

	if(drawWeight)
	{
		write_graphviz (dotfile, inGraph, boost::make_label_writer(boost::get(&VertexProperty::name, mrfgraph)),boost::make_label_writer(boost::get(boost::edge_weight, mrfgraph)));
	}
	else
	{
		write_graphviz (dotfile, inGraph, boost::make_label_writer(boost::get(&VertexProperty::name, mrfgraph)));
	}
	if(system(cmd.c_str()))
	{
		std::cerr << "Error calling Graphviz" << std::endl;
	}
	unlink(dotName.c_str());
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

Factor MarkovRandomField::variableEliminationQuery(std::vector<std::string> * query, std::string heuristics_type)
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

	Factor newFactor;
	std::vector<Factor> newFactors = factors;
	newFactor.setCounter(&counter);
	std::vector<unsigned int> multFactorsVector;

	unsigned int newFacIdx;

	while(!mrfVar.empty())
	{
		unsigned int j;

		if(heuristics_type=="clique_num")
		{
			std::sort(mrfVar.begin(),mrfVar.end(),mrfFacNumVarComp());	
		}
		else if(heuristics_type == "neighbor_num")
		{
			std::sort(mrfVar.begin(),mrfVar.end(),mrfNeighbVarComp(&newFactors));
		}
		else if(heuristics_type == "neighbor_multiCard")
		{
			std::sort(mrfVar.begin(),mrfVar.end(),mrfNeighbCardVarComp(&newFactors));
		}
		
		margVar = mrfVar.front();
		mrfVar.pop_front();
		multFactorsVector.clear();
		std::cout << margVar.first.first << " ";
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
		if(multFactorsVector.size()==0 && (margVar.second).size()>0)
		{
			multFactorsVector.push_back((margVar.second)[0]);
		}

		std::sort(multFactorsVector.begin(),multFactorsVector.end());

		newFactor.marginalize(margVar.first);

		for(int intJ=((int)(multFactorsVector.size())-1);intJ>=0;intJ--)
		{
			newFactors.erase(newFactors.begin()+multFactorsVector[intJ]);
		}
		//newFactor.normalize();
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
	}
	std::cout << std::endl;
	
	newFactor=newFactors[0];
	for(unsigned int i=0;(i+1)<newFactors.size();i++)
	{
		newFactor = newFactor * newFactors[i+1];
	}
	std::cout << "VE Query Number of Operations: " << counter  << " heuristics: " << heuristics_type << std::endl;
	newFactor.normalize();
	return newFactor;
}

void MarkovRandomField::clearOpCounter()
{
	counter=0;
}

Factor MarkovRandomField::query(std::vector<std::string> * query)
{
	//return dumbQuery(query);
	return variableEliminationQuery(query, "neighbor_num");
}

void MarkovRandomField::run()
{
	unsigned long long dumbQueryNum = 1;
	std::vector<std::string> asking = {"Gabriela"};
	Factor queryFac;
//	Factor queryFac = dumbQuery(&asking);
//	queryFac.printFactor();

	for(unsigned int i=0; i<mrf_variables.size(); i++)
	{
		dumbQueryNum *= (unsigned long long)(mrf_variables[i].first.second);
	}
	std::cout << "Dumb Query operations: " << dumbQueryNum << std::endl;

	clearOpCounter();
	queryFac = variableEliminationQuery(&asking, "creation_order");
	queryFac.printFactor();
	
	clearOpCounter();
	queryFac = variableEliminationQuery(&asking, "clique_num");
	queryFac.printFactor();

	clearOpCounter();
	queryFac = variableEliminationQuery(&asking, "neighbor_num");
	queryFac.printFactor();

	clearOpCounter();
	queryFac = variableEliminationQuery(&asking, "neighbor_multiCard");
	queryFac.printFactor();
	
	buildCliqueTree();
}

bool MarkovRandomField::buildCliqueTree()
{
	Graph cliqueGraph;
	VertexProperty vp;
	std::vector<VertexDescriptor> vdVec;
	Graph chordalGraph = triangulateGraph(mrfgraph);
	std::vector<std::vector<VertexDescriptor>> cliques = findAllCliques(&chordalGraph);
	for(unsigned int i=0; i<cliques.size(); i++)
	{
		vp.name = "";
		for(unsigned int j=0; j<cliques[i].size(); j++)
		{
			vp.name = vp.name + chordalGraph[cliques[i][j]].name + ",";
			std::cout << chordalGraph[cliques[i][j]].name << ":" << i << ":" << j << " ";
		}
		std::cout << std::endl << vp.name << " " << cliques[i].size() << std::endl;
		vp.name = vp.name.substr(0,vp.name.size()-1);
		vdVec.push_back(cliqueGraph.add_vertex(vp));
	}

	int countEdge=0;
	for(unsigned int i=0; i<cliques.size(); i++)
	{
		for(unsigned int j=i+1; j<cliques.size(); j++)
		{
			countEdge=0;
			for(unsigned int k=0; k<cliques[i].size(); k++)
			{
				for(unsigned int m=0; m<cliques[j].size(); m++)
				{
					if(cliques[i][k] == cliques[j][m])
					{
						countEdge++;
					}
				}
			}
			if(countEdge>0)
			{
				cliqueGraph.add_edge(vdVec[i],vdVec[j],(0-countEdge));
			}
		}
	}
	draw(cliqueGraph,"cliqueGraph", true);

	std::vector<EdgeDescriptor> spanning_tree;
	boost::kruskal_minimum_spanning_tree(cliqueGraph, std::back_inserter(spanning_tree));
	std::pair<EdgeIterator,EdgeIterator> es = boost::edges(cliqueGraph);
	std::pair<VertexIterator,VertexIterator> vertices;
	std::vector<std::pair<VertexDescriptor,VertexDescriptor>> vdGrphTree;
	for (vertices = boost::vertices(cliqueGraph); vertices.first != vertices.second; ++vertices.first)
	{
		vdGrphTree.push_back(std::make_pair(cliquetree.add_vertex(cliqueGraph[*vertices.first]),*vertices.first));
	}

	for (EdgeIterator eit = es.first; eit != es.second; ++eit)
	{
		if(std::find(spanning_tree.begin(), spanning_tree.end(), *eit)!=spanning_tree.end())
		{
			unsigned i=0,j=0;
			for(i=0; i<vdGrphTree.size(); i++)
			{
				if(vdGrphTree[i].second == boost::source(*eit,cliqueGraph))
				{
					break;
				}
			}
			for(j=0; j<vdGrphTree.size(); j++)
			{
				if(vdGrphTree[j].second == boost::target(*eit,cliqueGraph))
				{
					break;
				}
			}
			cliquetree.add_edge(vdGrphTree[i].first,vdGrphTree[j].first,(0-get(boost::edge_weight_t(), cliqueGraph, *eit)));
		}
	}

	return true;
}

Graph MarkovRandomField::triangulateGraph(Graph inputGraph)
{
	Graph retGraph;
	VertexProperty vp;
	AdjacencyIterator ai, ai_end, ai_aux;
	AdjacencyIterator retai,retai_aux;
	std::pair<VertexIterator,VertexIterator> vertices, retvertices;
	VertexIterator chosen, retfound, retfound2;
	unsigned int count=0, minCount=999999;
	retGraph = inputGraph;
	std::cout << std::endl << std::endl << std::endl << std::endl;
	std::cout << "---------Creating Chordal Graph---------" << std::endl;
	while(inputGraph.num_vertices()>0)
	{
		minCount = 999999;
		for (vertices = boost::vertices(inputGraph); vertices.first != vertices.second; ++vertices.first)
		{
			count = 0;
			for (boost::tie(ai, ai_end) = boost::adjacent_vertices(*vertices.first, inputGraph); ai != ai_end; ++ai)
			{
				count++;
			}
			if(count < minCount)
			{
				minCount = count;
				chosen = vertices.first;
			}
		}
		std::cout << "Chosen " << inputGraph[*chosen].name << " Count " << minCount << std::endl;
		for (boost::tie(ai, ai_end) = boost::adjacent_vertices(*chosen, inputGraph); ai != ai_end; ++ai)
		{
			ai_aux = ai;
			for(++ai_aux; ai_aux != ai_end; ++ai_aux)
			{
				for(retvertices = boost::vertices(retGraph); retvertices.first != retvertices.second; ++retvertices.first)
				{
					if(inputGraph[*ai].name == retGraph[*retvertices.first].name)
					{
						retfound = retvertices.first;
					}
					else
					{
						if(inputGraph[*ai_aux].name == retGraph[*retvertices.first].name)
						{
							retfound2 = retvertices.first;
						}
					}
				}
				if(!boost::edge(*ai,*ai_aux,inputGraph).second)
				{
					std::cout << "Inserting Edge between " << retGraph[*retfound].name << " and " << retGraph[*retfound2].name << std::endl;
					retGraph.add_edge(*ai,*ai_aux);
				}
				if(!boost::edge(*retfound,*retfound2,retGraph).second)
				{
					retGraph.add_edge(*retfound,*retfound2);
				}
			}
		}
		boost::clear_vertex(*chosen,inputGraph);
		boost::remove_vertex(*chosen,inputGraph);
	}
	draw(retGraph,"chordalGraph", false);
	std::cout << "----------------------------------------" << std::endl;
	std::cout << std::endl << std::endl << std::endl << std::endl;
	return retGraph;
}

std::vector<std::vector<VertexDescriptor>> MarkovRandomField::findAllCliques(Graph *inGraph)
{
	std::vector<std::vector<VertexDescriptor>> cliques;
	clique_vis vis(&cliques);
	boost::bron_kerbosch_all_cliques(*inGraph, vis);
	std::cout << std::endl << std::endl << std::endl << std::endl;
	std::cout << "---------Finding All Maximal Cliques---------" << std::endl;
	for(unsigned int i=0; i<cliques.size(); i++)
	{
		std::cout << "Clique " << i << std::endl;
		for(unsigned int j=0; j<cliques[i].size(); j++)
		{
			std::cout << (*inGraph)[cliques[i][j]].name << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "---------------------------------------------" << std::endl;
	std::cout << std::endl << std::endl << std::endl << std::endl;
	return cliques;
}