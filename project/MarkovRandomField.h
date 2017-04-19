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
	MRFFactor(FactorVarVector var, std::vector<double> val);
	~MRFFactor();

	MRFFactor operator*(MRFFactor b)
	{
		auto f = [](double a, double b){return a*b;};
		return factorBinaryOperation(this, &b, f);
	}
	MRFFactor operator*(double b)
	{
		MRFFactor newFac(variables,values);

		for(unsigned int i=0;i<values.size();i++)
		{
			newFac.values[i] = values[i]*b;
		}
		return newFac;
	}
	MRFFactor operator+(MRFFactor b)
	{
		auto f = [](double a, double b){return a+b;};
		return factorBinaryOperation(this, &b, f);
	}
	MRFFactor operator-(MRFFactor b)
	{
		auto f = [](double a, double b){return a-b;};
		return factorBinaryOperation(this, &b, f);
	}
	MRFFactor operator/(double b)
	{
		MRFFactor newFac(variables,values);

		for(unsigned int i=0;i<values.size();i++)
		{
			newFac.values[i] = values[i]/b;
		}
		return newFac;
	}

	FactorVarVector getVariables();
	double getValue(FactorVarVector assignment);
	double getValue(unsigned int idx);

	FactorVarVector getAssignment(unsigned int idx);
	unsigned int getIndex(FactorVarVector assign);

	void printFactor();

	void marginalize(FactorVar margVar)
	{
		MRFFactor newFactor = factorEliminationOperation(this,&margVar,
								[](MRFFactor *_this, FactorVarVector *ass,unsigned int elim_index, unsigned int elim_card, unsigned int elim_ass)
								{
									double sum=0;
									for(unsigned int j=0;j<elim_card;j++)
									{
										(*ass)[elim_index].second = j;
										sum = sum + _this->getValue(*ass);
									}
									return sum;
								});
		variables = newFactor.variables;
		values = newFactor.values;
	}
	void reduce(FactorVar reductionVar)
	{
		MRFFactor newFactor = factorEliminationOperation(this,&reductionVar,
								[](MRFFactor *_this, FactorVarVector *ass,unsigned int elim_index, unsigned int elim_card, unsigned int elim_ass)
								{
									(*ass)[elim_index].second = elim_ass;
									return _this->getValue(*ass);
								});
		variables = newFactor.variables;
		values = newFactor.values;
	}
	double normalize();

private:
	FactorVarVector variables;
	std::vector<double> values;

	bool is_valid();

	MRFFactor factorBinaryOperation(MRFFactor *a, MRFFactor *b, std::function<int(int,int)> operation);
	MRFFactor factorEliminationOperation(MRFFactor *a, FactorVar *eliminateVar, std::function<int(MRFFactor*,FactorVarVector*,unsigned int,unsigned int,unsigned int)> operation);
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
	std::vector<MRFFactor> Factors;

	std::string inputPath;
	bool loadInput();
	std::vector<std::string> splitString(std::string str, std::string delimiter);
};

#endif