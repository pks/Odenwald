COMPILER=clang
CFLAGS=-std=c++11 -O3


all: grammar.o hypergraph.o main.cc
	$(COMPILER) $(CFLAGS) -std=c++11 -lstdc++ -lm -lmsgpack grammar.o hypergraph.o main.cc -o fast_weaver


hypergraph.o: hypergraph.cc hypergraph.hh grammar.o semiring.hh sparse_vector.hh weaver.hh
	$(COMPILER) $(CFLAGS) -g -c hypergraph.cc

grammar.o: grammar.cc grammar.hh sparse_vector.hh util.hh
	$(COMPILER) $(CFLAGS) -g -c grammar.cc

test: test_grammar test_sparse_vector

test_grammar: test_grammar.cc grammar.o
	$(COMPILER) $(CFLAGS) -lstdc++ -lm grammar.o test_grammar.cc -o test_grammar

test_sparse_vector: test_sparse_vector.cc sparse_vector.hh
	$(COMPILER) $(CFLAGS) -lstdc++ -lm test_sparse_vector.cc -o test_sparse_vector

clean:
	rm -f fast_weaver hypergraph.o grammar.o test_grammar test_sparse_vector

