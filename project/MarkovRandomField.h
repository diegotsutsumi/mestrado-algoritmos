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
		MRFFactor ret;
		if(!this->is_valid() || !b.is_valid())
		{
			std::cerr << "Invalid B or This: " << std::endl;
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
		for(int i=0;i<ret.variables.size();i++)
		{
			valSize=valSize*ret.variables[i].second;
		}
		FactorVarVector ass, assB, assThis;

		assB = b.variables;
		assThis = this->variables;
		ret.values.resize(valSize, 0);
		for(int i=0;i<valSize;i++)
		{
			ass = ret.getAssignment(i);
			for(int j=0; j<assB.size(); j++)
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

			for(int j=0; j<assThis.size(); j++)
			{
				FactorVarVector::iterator iter = std::find_if(ass.begin(), ass.end(), comp(assThis[j].first));
				if(iter != ass.end())
				{
					assThis[j].second = iter->second;
				}
				else
				{
					std::cerr << "ERROR, should never end up here" << std::endl;
					return ret;
				}
			}

			ret.values[i] = b.getValue(assB) * this->getValue(assThis);
		}

		return ret;
	}
	//TODO Marginalize
	//TODO Factor Sum
	//TODO Factor Reduction

	FactorVarVector getVariables();
	int getValue(FactorVarVector assignment);
	int getValue(unsigned int idx);

	FactorVarVector getAssignment(unsigned int idx);
	unsigned int getIndex(FactorVarVector assign);

	void printFactor();
private:
	FactorVarVector variables;
	std::vector<int> values;

	bool is_valid();
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