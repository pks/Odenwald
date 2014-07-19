all: hypergraph.o main.cc
	clang -std=c++11 -lstdc++ -lm -lmsgpack hypergraph.o main.cc -o fast_weaver

hypergraph.o: hypergraph.cc hypergraph.hh grammar.o semiring.hh
	clang -std=c++11 -lmsgpack -c hypergraph.cc

grammar.o: grammar.cc grammar.hh
	clang -std=c++11 -c grammar.cc

make_paks: make_paks.cc
	g++ -std=c++11 -lmsgpack make_paks.cc -o make_paks

read_pak: read_pak.cc
	g++ -std=c++11 -lmsgpack read_pak.cc -o read_pak


clean:
	rm -f fast_weaver hypergraph.o grammar.o

