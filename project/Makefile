CXXFLAGS += -std=c++11 -Wall
exec: cliquetree
debug: CXXFLAGS += -g -ggdb -O0
debug: exec

clique_tree_objects = clique_tree.o MarkovRandomField.o Factor.o

all : exec

cliquetree: $(clique_tree_objects)
	g++ -o clique_tree $(clique_tree_objects)

clique_tree: clique_tree.cpp
Factor: Factor.cpp
MarkovRandomField: MarkovRandomField.cpp

clean:
	rm -f *.o clique_tree