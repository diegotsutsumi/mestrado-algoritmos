#ifndef _FACTOR_H_
#define _FACTOR_H_
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

typedef std::pair<std::string, unsigned int> FactorVar;
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

class Factor
{
public:
	Factor();
	Factor(FactorVarVector var, std::vector<double> val);
	~Factor();

	Factor operator*(Factor b)
	{
		auto f = [](double a, double b){return (double)(a*b);};
		return factorBinaryOperation(this, &b, f);
	}
	Factor operator*(double b)
	{
		Factor newFac(variables,values);

		for(unsigned int i=0;i<values.size();i++)
		{
			(*operationsCounter)++;
			newFac.values[i] = (double)(values[i]*b);
		}
		return newFac;
	}
	Factor operator+(Factor b)
	{
		auto f = [](double a, double b){return (double)(a+b);};
		return factorBinaryOperation(this, &b, f);
	}
	Factor operator-(Factor b)
	{
		auto f = [](double a, double b){return (double)(a-b);};
		return factorBinaryOperation(this, &b, f);
	}

	/*Implement Factor Division by Another Factor*/

	Factor operator/(double b)
	{
		Factor newFac(variables,values);

		for(unsigned int i=0;i<values.size();i++)
		{
			(*operationsCounter)++;
			newFac.values[i] = (double)(values[i]/b);
		}
		return newFac;
	}

	void setCounter(unsigned int * a);

	void setVariables(FactorVarVector _var);
	FactorVarVector getVariables();

	void setValues(std::vector<double> _val);
	std::vector<double> getValues();
	double getValue(FactorVarVector assignment);
	double getValue(unsigned int idx);

	FactorVarVector getAssignment(unsigned int idx);
	unsigned int getIndex(FactorVarVector assign);

	bool isValid();

	void clear();

	void printFactor();

	void marginalize(FactorVar margVar)
	{
		Factor newFactor = factorEliminationOperation(this,&margVar,
								[](Factor *_this, FactorVarVector *ass,unsigned int elim_index, unsigned int elim_card, unsigned int elim_ass)
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
		(*operationsCounter) = (*operationsCounter) + values.size();
	}
	void reduce(FactorVar reductionVar)
	{
		Factor newFactor = factorEliminationOperation(this,&reductionVar,
								[](Factor *_this, FactorVarVector *ass,unsigned int elim_index, unsigned int elim_card, unsigned int elim_ass)
								{
									(*ass)[elim_index].second = elim_ass;
									return _this->getValue(*ass);
								});
		variables = newFactor.variables;
		values = newFactor.values;
	}
	double normalize();

private:
	unsigned int * operationsCounter;
	FactorVarVector variables;
	std::vector<double> values;

	bool is_valid;
	void updateIsValid();

	Factor factorBinaryOperation(Factor *a, Factor *b, std::function<double(double,double)> operation);
	Factor factorEliminationOperation(Factor *a, FactorVar *eliminateVar, std::function<double(Factor*,FactorVarVector*,unsigned int,unsigned int,unsigned int)> operation);
};

#endif