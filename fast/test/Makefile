COMPILER=g++
CFLAGS=-std=c++11 -O3 -I../
TCMALLOC=/home/pks/src/weaver/fast/gperftools-2.1/lib/libtcmalloc_minimal.a -pthread


all: test_grammar test_sparse_vector test_parse

test_grammar: test_grammar.cc ../grammar.hh
	$(COMPILER) $(CFLAGS) -lstdc++ -lm $(TCMALLOC) test_grammar.cc -o test_grammar

test_sparse_vector: test_sparse_vector.cc ../sparse_vector.hh
	$(COMPILER) $(CFLAGS) -lstdc++ -lm $(TCMALLOC) test_sparse_vector.cc -o test_sparse_vector

test_parse: test_parse.cc ../parse.hh ../grammar.hh ../util.hh
	$(COMPILER) $(CFLAGS) -lstdc++ -lm $(TCMALLOC) test_parse.cc -o test_parse

clean:
	rm -f test_grammar test_sparse_vector

