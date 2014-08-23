COMPILER=g++
CFLAGS=-std=c++11 -O3
TCMALLOC=/home/pks/src/weaver/fast/gperftools-2.1/lib/libtcmalloc_minimal.a -pthread


all: hypergraph.o main.cc
	$(COMPILER) $(CFLAGS) -lstdc++ -lm -lmsgpack $(TCMALLOC) hypergraph.o main.cc -o fast_weaver

hypergraph.o: hypergraph.cc hypergraph.hh semiring.hh sparse_vector.hh weaver.hh
	$(COMPILER) $(CFLAGS) -g -c $(TCMALLOC) hypergraph.cc

clean:
	rm -f fast_weaver
	rm -f hypergraph.o parse.o

