#ifndef _MRF_H_
#define _MRF_H_

#include <string>
#include <vector>

#include <boost/graph/undirected_graph.hpp> 
#include <boost/graph/graphviz.hpp>

#include "Factor.h"

struct EdgeProperty
{
	int weight;
};

struct VertexProperty
{
	std::string name;
};

typedef boost::undirected_graph<VertexProperty, EdgeProperty> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;

struct mrfFacNumVarComp
{
	inline bool operator() (const std::pair<FactorVar,std::vector<unsigned int>>& elem1, const std::pair<FactorVar,std::vector<unsigned int>>& elem2)
	{
		return ((elem1.second).size() < (elem2.second).size());
	}
};

struct mrfNeighbVarComp
{
	mrfNeighbVarComp(std::vector<Factor> * _vecFac) : vecFac(_vecFac){};
	inline bool operator() (const std::pair<FactorVar,std::vector<unsigned int>>& elem1, const std::pair<FactorVar,std::vector<unsigned int>>& elem2)
	{
		unsigned int elem1Neighb=0, elem2Neighb=0;
		for(unsigned int i=0;i<elem1.second.size();i++)
		{
			elem1Neighb += ((*vecFac)[i].getVariables().size()-1);
		}
		for(unsigned int i=0;i<elem2.second.size();i++)
		{
			elem2Neighb += ((*vecFac)[2].getVariables().size()-1);
		}

		return (elem1Neighb < elem2Neighb);
	}
	std::vector<Factor> * vecFac;
};

struct mrfNeighbCardVarComp
{
	mrfNeighbCardVarComp(std::vector<Factor> * _vecFac) : vecFac(_vecFac){};
	inline bool operator() (const std::pair<FactorVar,std::vector<unsigned int>>& elem1, const std::pair<FactorVar,std::vector<unsigned int>>& elem2)
	{
		unsigned int elem1Weight=1, elem2Weight=1;
		for(unsigned int i=0;i<(elem1.second).size();i++)
		{
			FactorVarVector facs = (*vecFac)[(elem1.second)[i]].getVariables();
			for(unsigned int j=0;j<facs.size();j++)
			{
				if(facs[j].first != elem1.first.first)
				{
					elem1Weight *= facs[j].second;
				}
			}
		}
		for(unsigned int i=0;i<elem2.second.size();i++)
		{

			FactorVarVector facs = (*vecFac)[(elem2.second)[i]].getVariables();
			for(unsigned int j=0;j<facs.size();j++)
			{
				if(facs[j].first != elem2.first.first)
				{
					elem2Weight *= facs[j].second;
				}
			}
		}
		return (elem1Weight < elem2Weight);
	}
	std::vector<Factor> * vecFac;
};

class MarkovRandomField
{
public:
	MarkovRandomField(std::string _inputPath);
	~MarkovRandomField();

	Factor query(std::vector<std::string> * query);
	void clearOpCounter();

	bool buildcliquetree();
	bool drawMRF();
	bool drawcliquetree();

	void test();

private:
	unsigned int operationsCounter;
	Graph mrfgraph;
	Graph cliquetree;
	std::vector<Factor> factors;

	std::vector<std::pair<FactorVar,std::vector<unsigned int>>> mrf_variables;

	std::string inputPath;
	bool loadInput();
	std::vector<std::string> splitString(std::string str, std::string delimiter);

	Factor dumbQuery(std::vector<std::string> * query);
	Factor variableEliminationQuery(std::vector<std::string> * query);
	unsigned int counter;
};

#endif