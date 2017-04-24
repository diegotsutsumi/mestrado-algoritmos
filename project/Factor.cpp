#include <iostream>
#include <exception>
#include "Factor.h"

Factor::Factor()
{
}
Factor::Factor(FactorVarVector var, std::vector<double> val)
{
	variables = var;
	values = val;
	updateIsValid();
	if(!is_valid)
	{
		std::cerr << "Invalid factor" << std::endl;
	}
}
Factor::~Factor()
{
}

void Factor::updateIsValid()
{
	if(variables.size()==0 && values.size()==0)
	{
		is_valid = true;
		return;
	}
	unsigned int valSize=1;
	for(unsigned int i=0;i<variables.size();i++)
	{
		valSize=valSize*variables[i].second;
	}
	if(values.size()!=valSize)
	{
		is_valid = false;
		return;
	}
	is_valid = true;
	return;
}

void Factor::setCounter(unsigned int * a)
{
	operationsCounter = a;
}

void Factor::setVariables(FactorVarVector _var)
{
	variables = _var;
	updateIsValid();
}

FactorVarVector Factor::getVariables()
{
	return variables;
}

void Factor::setValues(std::vector<double> _val)
{
	values = _val;
	updateIsValid();
}

double Factor::getValue(FactorVarVector assignment)
{
	if(!is_valid)
	{
		throw std::runtime_error("Invalid Factor: getValue assignment");
	}
	
	unsigned int idx;
	try
	{
		idx = getIndex(assignment);
	}
	catch(std::invalid_argument& ia)
	{
		throw ia;
	}

	if(idx >= values.size())
	{
		throw std::out_of_range("Wrong assigment");
	}
	
	return values[idx];
}

double Factor::getValue(unsigned int idx)
{
	if(!is_valid)
	{
		throw std::runtime_error("Invalid Factor: getValue uint");
	}
	if(idx >= values.size())
	{
		throw std::out_of_range("Index out of range");
	}
	return values[idx];
}

FactorVarVector Factor::getAssignment(unsigned int idx)
{
	if(!is_valid)
	{
		throw std::runtime_error("Invalid Factor: getAssignment");
	}

	if(idx>=values.size())
	{
		throw std::out_of_range("Input out of Range");
	}
	FactorVarVector assRet = variables;
	unsigned int den;
	for(unsigned int i=0;i<assRet.size();i++)
	{
		den=1;
		for(unsigned int j=0; j<i; j++)
		{
			den = den*variables[j].second;
		}
		assRet[i].second = idx / (den);
		assRet[i].second = assRet[i].second % variables[i].second;
	}
	return assRet;
}

unsigned int Factor::getIndex(FactorVarVector assign)
{
	unsigned int retIdx=0;

	if(assign.size() != variables.size())
	{
		throw std::invalid_argument("Bad variables size");
	}

	for(unsigned int i=0;i<variables.size();i++)
	{
		if(assign[i].first!=variables[i].first || assign[i].second>=variables[i].second)
		{
			throw std::invalid_argument("Bad variables name or assignment value");
		}
	}

	for(unsigned int i=0;i<assign.size();i++)
	{
		if(i==0)
		{
			retIdx = retIdx + assign[i].second;
		}
		else
		{
			unsigned int mul=1;
			for(unsigned int j=0; j<i; j++)
			{
				mul = mul*variables[j].second;
			}
			retIdx = retIdx + assign[i].second*mul;
		}
	}
	return retIdx;
}

bool Factor::isValid()
{
	return is_valid;
}

void Factor::clear()
{
	variables.clear();
	values.clear();
}

void Factor::printFactor()
{
	if(!is_valid)
	{
		std::cout << "Invalid Factor Print" << std::endl;
	}
	for(int i=((int)(variables.size())-1);i>=0;i--)
	{
		std::cout << variables[i].first << "  ";
	}
	std::cout << std::endl;
	FactorVarVector ass;
	for(unsigned int i=0;i<values.size();i++)
	{
		ass = getAssignment(i);
		for(int j=((int)(ass.size())-1);j>=0;j--)
		{
			std::cout << ass[j].second;
			for(int k=0;k<((int)(variables[j].first.size())-1);k++)
			{
				std::cout << " ";
			}
			std::cout << "  ";
		}
		std::cout << "   | ";
		std::cout << getValue(i) << std::endl;
	}
}

double Factor::normalize()
{
	if(!is_valid)
	{
		return 0;
	}

	double partitionZ = 0;
	//double aux;

	for(unsigned int i=0;i<values.size();i++)
	{
		/*aux = partitionZ + values[i];
		if(aux < partitionZ)
		{
			std::cout << "Partition Function Overflow, diving factor by 2" << std::endl;
			*this = *this/2;
			i=0;
			partitionZ = 0;
			aux = 0;
			continue;
		}*/
		partitionZ += values[i];
		(*operationsCounter)++;
	}
	*this = *this/partitionZ;

	return partitionZ;
}

Factor Factor::factorEliminationOperation(Factor *a, FactorVar *eliminateVar, std::function<int(Factor*,FactorVarVector*,unsigned int,unsigned int,unsigned int)> operation)
{
	if(!a->isValid())
	{
		std::cerr << "Invalid factor factorEliminationOperation" << std::endl;
		//Invalid Factor
		return *a;
	}
	FactorVarVector new_ass, ass;
	FactorVarVector new_variables;
	std::vector<double> new_values;

	ass = variables;
	FactorVarVector::iterator iter = std::find_if(ass.begin(), ass.end(), comp(eliminateVar->first));
	unsigned int eliminateIndex = (iter - ass.begin());
	if(iter==ass.end())
	{
		//Variable not found
		return *a;
	}

	for(unsigned int i=0; i<variables.size(); i++)
	{
		if(eliminateVar->first != variables[i].first)
		{
			new_variables.push_back(variables[i]);
		}
	}
	unsigned int valSize=1;
	for(unsigned int i=0;i<new_variables.size();i++)
	{
		valSize=valSize*new_variables[i].second;
	}
	new_values.resize(valSize,0);

	Factor newFactor(new_variables, new_values);
	for(unsigned int i=0;i<valSize;i++)
	{
		try
		{
			new_ass = newFactor.getAssignment(i);
		}
		catch (std::runtime_error& rte)
		{
			std::cerr << rte.what() << std::endl;
			newFactor.clear();
			break;
		}
		catch (std::out_of_range& oor)
		{
			std::cerr << oor.what() << std::endl;
			newFactor.clear();
			break;
		}

		for(unsigned int j=0;j<ass.size();j++)
		{
			if(j<eliminateIndex)
			{
				ass[j].second = new_ass[j].second;
			}
			else if(j>eliminateIndex)
			{
				ass[j].second = new_ass[j-1].second;
			}
		}

		newFactor.values[i] = operation(a,&ass,eliminateIndex,variables[eliminateIndex].second,eliminateVar->second);
	}
	return newFactor;
}

Factor Factor::factorBinaryOperation(Factor *a, Factor *b, std::function<int(int,int)> operation)
{
	Factor ret;
	if(!a->is_valid || !b->is_valid)
	{
		std::cerr << "Invalid B or A: " << std::endl;
		return ret;
	}
	ret.setCounter(a->operationsCounter);
	for(unsigned int i=0;i<a->variables.size();i++)
	{
		ret.variables.push_back(a->variables[i]);
	}
	for(unsigned int i=0;i<b->variables.size();i++)
	{
		if(std::find(ret.variables.begin(), ret.variables.end(), b->variables[i])==ret.variables.end())
		{
			ret.variables.push_back(b->variables[i]);
		}
	}
	unsigned int valSize=1;
	for(unsigned int i=0;i<ret.variables.size();i++)
	{
		valSize=valSize*ret.variables[i].second;
	}
	FactorVarVector ass, assB, assA;

	assB = b->variables;
	assA = a->variables;
	ret.values.resize(valSize, 0);
	ret.updateIsValid();
	for(unsigned int i=0;i<valSize;i++)
	{
		try
		{
			ass = ret.getAssignment(i);			
		}
		catch (std::runtime_error& rte)
		{
			std::cerr << rte.what() << std::endl;
			ret.clear();
			break;
		}
		catch (std::out_of_range& oor)
		{
			std::cerr << oor.what() << std::endl;
			ret.clear();
			break;
		}

		for(unsigned int j=0; j<assB.size(); j++)
		{
			FactorVarVector::iterator iter = std::find_if(ass.begin(), ass.end(), comp(assB[j].first));
			if(iter != ass.end())
			{
				assB[j].second = iter->second;
			}
			else
			{
				std::cerr << "ERROR! Variable B not found" << std::endl;
				ret.clear();
				return ret;
			}
		}
		
		for(unsigned int j=0; j<assA.size(); j++)
		{
			FactorVarVector::iterator iter = std::find_if(ass.begin(), ass.end(), comp(assA[j].first));
			if(iter != ass.end())
			{
				assA[j].second = iter->second;
			}
			else
			{
				std::cerr << "ERROR! Variable A not found" << std::endl;
				ret.clear();
				return ret;
			}
		}

		try
		{
			double aValue = a->getValue(assA);
			double bValue = b->getValue(assB);

			(*ret.operationsCounter)++;

			ret.values[i] = operation(bValue,aValue);
		}
		catch (std::invalid_argument& ia)
		{
			std::cerr << ia.what() << std::endl;
			ret.clear();
			break;
		}
		catch (std::runtime_error& rte)
		{
			std::cerr << rte.what() << std::endl;
			ret.clear();
			break;
		}
		catch (std::out_of_range& oor)
		{
			std::cerr << oor.what() << std::endl;
			ret.clear();
			break;
		}
	}
	ret.updateIsValid();
	return ret;
}
