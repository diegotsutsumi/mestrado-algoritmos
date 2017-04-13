// STL
#include <iostream>                  // for std::cout
 
// Boost
#include <boost/graph/undirected_graph.hpp>// A subclass to provide reasonable arguments to adjacency_list for a typical undirected graph
#include <boost/graph/graphviz.hpp>
 
void UndirectedGraph();
 
int main(int,char*[])
{
	UndirectedGraph();
  	return 0;
}

struct EdgeProperty
{
	int weight;
};

struct VertexProperty
{
	std::string name;
};

void UndirectedGraph()
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
}