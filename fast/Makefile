COMPILER=clang


all: hypergraph.o main.cc
	$(COMPILER) -std=c++11 -lstdc++ -lm -lmsgpack hypergraph.o main.cc -o fast_weaver

hypergraph.o: hypergraph.cc hypergraph.hh grammar.o semiring.hh
	$(COMPILER) -g -std=c++11 -c hypergraph.cc

grammar.o: grammar.cc grammar.hh
	$(COMPILER) -g -std=c++11 -c grammar.cc

clean:
	rm -f fast_weaver hypergraph.o grammar.o

