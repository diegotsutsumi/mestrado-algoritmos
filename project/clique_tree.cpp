#include <iostream> 
#include "MarkovRandomField.h"

 
int main(int,char*[])
{
	MarkovRandomField mrf("input.mrf");
	/*std::vector<double> values = {6,53,67,1,2,7,3,8,10,1,2,3};
	std::vector<std::pair<std::string, unsigned int>> variables, testCase;
	std::pair<std::string, int> example;

	example = std::make_pair("node1", 2);
	variables.push_back(example);
	example = std::make_pair("node2", 3);
	variables.push_back(example);
	example = std::make_pair("node3", 2);
	variables.push_back(example);
	std::cout << "Creating first factor" << std::endl;
	MRFFactor mrff(variables, values);
	mrff.printFactor();
	std::cout << std::endl << std::endl;


	values = {4,53,67,1,2,2,3,8,1,1,2,3};
	variables[2].first = "node6";
	std::cout << "Creating second factor" << std::endl;
	MRFFactor mrff1(variables, values);
	mrff1.printFactor();
	std::cout << std::endl << std::endl;

	std::cout << "Multiplying two factors" << std::endl;
	MRFFactor mrff2 = mrff * mrff1;
	mrff2.printFactor();
	std::cout << std::endl << std::endl;

	mrff2.reduce(std::make_pair("node2",1));
	//mrff2.printFactor();
	mrff2.normalize();
	mrff2.printFactor();
	std::cout << std::endl << std::endl;*/

	mrf.test();
	mrf.drawMRF();
  	return 0;
}

/*void UndirectedGraph()
{
	// undirected_graph is a subclass of adjacency_list which gives you object oriented access to functions like add_vertex and add_edge, which makes the code easier to understand. However, it hard codes many of the template parameters, so it is much less flexible.

	typedef boost::undirected_graph<VertexProperty, EdgeProperty> Graph;
	Graph g;
	VertexProperty v;
	v.name = "Node1";
	Graph::vertex_descriptor v0 = g.add_vertex(v);
	v.name = "Node2";
	Graph::vertex_descriptor v1 = g.add_vertex(v);

	EdgeProperty e;
	e.weight = 2;
	g.add_edge(v0, v1, e);

	std::ofstream dotfile("example.dot");
	write_graphviz (dotfile, g, boost::make_label_writer(boost::get(&VertexProperty::name, g)),boost::make_label_writer(boost::get(&EdgeProperty::weight, g)));
}*/