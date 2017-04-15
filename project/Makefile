CXXFLAGS += -std=c++11 -O3
exec: cliquetree
debug: CXXFLAGS += -g -ggdb -O0 -Wall
debug: exec

clique_tree_objects = clique_tree.o MarkovRandomField.o

all : exec

cliquetree: $(clique_tree_objects)
	g++ -o clique_tree $(clique_tree_objects)

clique_tree: clique_tree.cpp
MarkovRandomField: MarkovRandomField.cpp

clean:
	rm -f *.o clique_tree