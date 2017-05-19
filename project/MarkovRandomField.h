#ifndef _MRF_H_
#define _MRF_H_

#include <string>
#include <vector>

#include <boost/graph/undirected_graph.hpp> 
#include <boost/graph/graphviz.hpp>

#include "Factor.h"

/*struct EdgeProperty
{
	int weight;
};*/
typedef boost::property<boost::edge_weight_t,int> EdgeProperty;

struct VertexProperty
{
	std::string name;
};

typedef boost::undirected_graph<VertexProperty, EdgeProperty> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
typedef boost::graph_traits<Graph>::edge_descriptor EdgeDescriptor;
typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
typedef boost::graph_traits<Graph>::adjacency_iterator AdjacencyIterator;

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
		FactorVarVector facVec, auxFacVec;
		facVec.clear();
		for(unsigned int i=0;i<elem1.second.size();i++)
		{
			auxFacVec = (*vecFac)[elem1.second[i]].getVariables();
			facVec.insert(facVec.end(), auxFacVec.begin(), auxFacVec.end());
		}
		std::sort(facVec.begin(), facVec.end());
		facVec.erase(std::unique(facVec.begin(), facVec.end()), facVec.end());
		elem1Neighb = facVec.size() - 1;
		facVec.clear();

		for(unsigned int i=0;i<elem2.second.size();i++)
		{
			auxFacVec = (*vecFac)[elem2.second[i]].getVariables();
			facVec.insert(facVec.end(), auxFacVec.begin(), auxFacVec.end());
		}
		std::sort(facVec.begin(), facVec.end());
		facVec.erase(std::unique(facVec.begin(), facVec.end()), facVec.end());
		elem2Neighb = facVec.size() - 1;
		return (elem1Neighb < elem2Neighb);
	}
	std::vector<Factor> * vecFac;
};

struct mrfNeighbCardVarComp
{
	mrfNeighbCardVarComp(std::vector<Factor> * _vecFac) : vecFac(_vecFac){};
	inline bool operator() (const std::pair<FactorVar,std::vector<unsigned int>>& elem1, const std::pair<FactorVar,std::vector<unsigned int>>& elem2)
	{
		unsigned int elem1Neighb=1, elem2Neighb=1;
		FactorVarVector facVec, auxFacVec;
		facVec.clear();
		for(unsigned int i=0;i<elem1.second.size();i++)
		{
			auxFacVec = (*vecFac)[elem1.second[i]].getVariables();
			facVec.insert(facVec.end(), auxFacVec.begin(), auxFacVec.end());
		}
		std::sort(facVec.begin(), facVec.end());
		facVec.erase(std::unique(facVec.begin(), facVec.end()), facVec.end());
		for(unsigned int i=0;i<facVec.size();i++)
		{
			elem1Neighb *= facVec[i].second;
		}
		
		facVec.clear();
		for(unsigned int i=0;i<elem2.second.size();i++)
		{
			auxFacVec = (*vecFac)[elem2.second[i]].getVariables();
			facVec.insert(facVec.end(), auxFacVec.begin(), auxFacVec.end());
		}
		std::sort(facVec.begin(), facVec.end());
		facVec.erase(std::unique(facVec.begin(), facVec.end()), facVec.end());
		for(unsigned int i=0;i<facVec.size();i++)
		{
			elem2Neighb *= facVec[i].second;
		}

		return (elem1Neighb < elem2Neighb);
	}
	std::vector<Factor> * vecFac;
};

struct clique_vis
{
	clique_vis(std::vector<std::vector<VertexDescriptor>> * _out) : out(_out)
	{ }

	template <typename Clique, typename Graph>
	void clique(const Clique& c, const Graph& g)
	{
		typename Clique::const_iterator i, end = c.end();
		std::vector<VertexDescriptor> newClique;
		size_t idx = out->size();
		out->push_back(newClique);
		for(i = c.begin(); i != end; ++i)
		{
			(*out)[idx].push_back(*i);
		}
	}
	std::vector<std::vector<VertexDescriptor>> * out;
};

class MarkovRandomField
{
public:
	MarkovRandomField(std::string _inputPath);
	~MarkovRandomField();

	Factor query(std::vector<std::string> * query);
	void clearOpCounter();

	bool buildCliqueTree();
	bool drawMRF();
	bool drawCliqueTree();
	

	void run();

private:
	unsigned int operationsCounter;
	Graph mrfgraph;
	Graph cliquetree;
	std::vector<Factor> factors;

	std::vector<std::pair<FactorVar,std::vector<unsigned int>>> mrf_variables;

	std::string inputPath;
	bool loadInput();
	std::vector<std::string> splitString(std::string str, std::string delimiter);
	bool draw(Graph inGraph, std::string name, bool drawWeight);

	Factor dumbQuery(std::vector<std::string> * query);
	Factor variableEliminationQuery(std::vector<std::string> * query, std::string heuristics_type);

	Graph triangulateGraph(Graph inGraph);
	std::vector<std::vector<VertexDescriptor>> findAllCliques(Graph * inGraph);
	unsigned int counter;
};

#endif