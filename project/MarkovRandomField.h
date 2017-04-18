#ifndef _MRF_H_
#define _MRF_H_

#include <string>
#include <vector>
#include <algorithm>

#include <boost/graph/undirected_graph.hpp>// A subclass to provide reasonable arguments to adjacency_list for a typical undirected graph
#include <boost/graph/graphviz.hpp>

typedef std::pair<std::string, int> FactorVar;
typedef std::vector<FactorVar> FactorVarVector;

struct comp
{
	comp(std::string const& s) : _s(s) { }

	bool operator () (FactorVar const& p)
	{
		return (p.first == _s);
	}

	std::string _s;
};

class MRFFactor
{
public:
	MRFFactor();
	MRFFactor(FactorVarVector var, std::vector<int> val);
	~MRFFactor();

	MRFFactor operator*(MRFFactor b)
	{
		auto f = [](int a, int b){return a*b;};
		return factorBinaryOperation(*this, b, f);
	}
	MRFFactor operator+(MRFFactor b)
	{
		auto f = [](int a, int b){return a+b;};
		return factorBinaryOperation(*this, b, f);
	}
	MRFFactor operator-(MRFFactor b)
	{
		auto f = [](int a, int b){return a-b;};
		return factorBinaryOperation(*this, b, f);
	}

	//TODO Factor Reduction
	//TODO Calculate Partition Function
	//TODO Normalize

	FactorVarVector getVariables();
	int getValue(FactorVarVector assignment);
	int getValue(unsigned int idx);

	FactorVarVector getAssignment(unsigned int idx);
	unsigned int getIndex(FactorVarVector assign);

	void printFactor();

	void marginalize(FactorVar margVar);
	void reduce(FactorVar reductionVar);
private:
	FactorVarVector variables;
	std::vector<int> values;

	bool is_valid();

	MRFFactor factorBinaryOperation(MRFFactor a, MRFFactor b, std::function<int(int,int)> operation);
};

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

	bool buildCliqueTree();
	bool drawMRF();
	bool drawCliqueTree();

private:
	Graph MRFGraph;
	Graph CliqueTree;

	std::string inputPath;
	bool loadInput();
	std::vector<std::string> splitString(std::string str, std::string delimiter);
};

#endif