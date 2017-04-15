#ifndef _MRF_H_
#define _MRF_H_

#include <string>
#include <vector>

#include <boost/graph/undirected_graph.hpp>// A subclass to provide reasonable arguments to adjacency_list for a typical undirected graph
#include <boost/graph/graphviz.hpp>

struct EdgeProperty
{
	int weight;
};

struct VertexProperty
{
	std::string name;
};

/*class MRFFactor
{
public:
	MRFFactor();
	~MRFFactor();

private:
	std::vector<std::string> variables;
	std::vector<int> values;
};*/
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