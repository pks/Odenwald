all: hypergraph.o main.cc
	clang -std=c++11 -lstdc++ main.cc hypergraph.o json/libjson.a -o fast_weaver

hypergraph.o: hypergraph.cc hypergraph.hh grammar.o semiring.hh
	clang -std=c++11 -lstdc++ -c hypergraph.cc -I./msgpack-c/include/ grammar.o ./msgpack-c/lib/libmsgpack.a

grammar.o: grammar.cc grammar.hh
	clang -std=c++11 -lstdc++ -c grammar.cc

clean:
	rm fast_weaver hypergraph.o grammar.o

