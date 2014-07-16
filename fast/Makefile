all: hypergraph.o main.cc
	clang -std=c++11 -lstdc++ -lm hypergraph.o -I./msgpack-c/include/ main.cc -o fast_weaver

hypergraph.o: hypergraph.cc hypergraph.hh grammar.o semiring.hh
	clang -std=c++11 -I./msgpack-c/include/ -c hypergraph.cc

grammar.o: grammar.cc grammar.hh
	clang -std=c++11 -c grammar.cc

clean:
	rm -f fast_weaver hypergraph.o grammar.o

