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

class MRFFactor
{
public:
	MRFFactor();
	MRFFactor(std::vector<std::pair<std::string, int>> var, std::vector<int> val);
	~MRFFactor();

    MRFFactor operator*(MRFFactor b)
	{
		MRFFactor ret;
		if(!this->is_valid() || !b.is_valid())
		{
			return ret;
		}

		for(int i=0;i<this->variables.size();i++)
		{
			ret.variables.push_back(this->variables[i]);
		}
		for(int i=0;i<b.variables.size();i++)
		{
			if(std::find(ret.variables.begin(), ret.variables.end(), b.variables[i])==ret.variables.end())
			{
				ret.variables.push_back(b.variables[i]);
			}
		}
		unsigned int valSize=1;
		for(int i=0;i<=ret.variables.size();i++)
		{
			valSize=valSize*ret.variables[i].second;
		}

		/*TODO: FINISH*/
		return ret;
	}
	//TODO Marginalize
	//TODO Factor Sum
	//TODO Factor Reduction

	int getValue(std::vector<std::pair<std::string,int>> assignment);
	int getValue(unsigned int idx);
	std::vector<std::pair<std::string,int>> getAssignment(unsigned int idx);
	unsigned int getIndex(std::vector<std::pair<std::string, int>> assign);

private:
	std::vector<std::pair<std::string,int>> variables;
	std::vector<int> values;

	bool is_valid();
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