COMPILER=clang
CFLAGS=-std=c++11 -O3


test_grammar: test_grammar.cc ../grammar.o
	$(COMPILER) $(CFLAGS) ../grammar.o test_grammar.cc

test_sparse_vector: test_sparse_vector.cc
	$(COMPILER) $(CFLAGS) -lm test_sparse_vector.cc

clean:
	rm -f test_grammar test_sparse_vector

