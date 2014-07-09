all: hypergraph.o main.cc
	clang -std=c++11 -lstdc++ main.cc hypergraph.o -o fast_weaver

hypergraph.o: hypergraph.cc hypergraph.hh grammar.o semiring.hh
	clang -std=c++11 -lstdc++ -c hypergraph.cc grammar.o

grammar.o: grammar.cc grammar.hh
	clang -std=c++11 -lstdc++ -c grammar.cc

clean:
	rm fast_weaver hypergraph.o grammar.o

