CXXFLAGS += -std=c++11
exec: cnfchecker
debug: CXXFLAGS += -g -ggdb -O0 -Wall
debug: exec

cnf_checker_objects = cnf_checker.o CNFChecker.o

all : exec

cnfchecker: $(cnf_checker_objects)
	g++ -o cnf_checker $(cnf_checker_objects)

CNFChecker: CNFChecker.cpp
cnf_checker: cnf_checker.cpp

clean:
	rm -f *.o cnf_checker