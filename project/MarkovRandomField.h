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

class MarkovRandomField
{
public:
	MarkovRandomField(std::string _inputPath);
	~MarkovRandomField();

	Factor query(std::vector<std::string> * query);

	bool buildcliquetree();
	bool drawMRF();
	bool drawcliquetree();

	void test();

private:
	unsigned int operationsCounter;
	Graph mrfgraph;
	Graph cliquetree;
	std::vector<Factor> factors;
	std::vector<FactorVar> mrf_variables;

	std::string inputPath;
	bool loadInput();
	std::vector<std::string> splitString(std::string str, std::string delimiter);

	Factor dumbQuery(std::vector<std::string> * query);
	unsigned int counter;
};

#endif