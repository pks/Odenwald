COMPILER=clang
CFLAGS=-std=c++11 -O3 -Wall
TCMALLOC=$(shell pwd)/external/gperftools-2.1/lib/libtcmalloc_minimal.a -pthread
SRC=src

PRINT = @echo -e "\e[1;34mBuilding $<\e[0m"

all: $(SRC)/hypergraph.o $(SRC)/fast_weaver.cc
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ -lm -lmsgpack $(TCMALLOC) $(SRC)/hypergraph.o \
		$(SRC)/fast_weaver.cc \
		-o fast_weaver

$(SRC)/hypergraph.o: $(SRC)/hypergraph.cc $(SRC)/hypergraph.hh \
											$(SRC)/grammar.hh \
											$(SRC)/semiring.hh $(SRC)/sparse_vector.hh \
											$(SRC)/types.hh
	$(PRINT)
	$(COMPILER) $(CFLAGS) -g -c $(TCMALLOC) \
		$(SRC)/hypergraph.cc \
		-o $(SRC)/hypergraph.o

util: make_pak read_pak

make_pak: $(SRC)/make_pak.cc external/json-cpp/single_include/json-cpp.hpp \
					$(SRC)/hypergraph.hh $(SRC)/types.hh
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ -lm -lmsgpack -I./external \
		$(SRC)/make_pak.cc \
		-o make_pak

read_pak: $(SRC)/read_pak.cc
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ -lmsgpack \
		$(SRC)/read_pak.cc \
		-o read_pak

test: test_grammar test_hypergraph test_parse test_sparse_vector

test_grammar: $(SRC)/test_grammar.cc $(SRC)/grammar.hh
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ -lm $(TCMALLOC) \
		$(SRC)/test_grammar.cc \
		-o test_grammar

test_hypergraph: $(SRC)/test_hypergraph.cc $(SRC)/hypergraph.o $(SRC)/grammar.hh
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ -lm $(TCMALLOC) -lmsgpack $(SRC)/hypergraph.o \
		$(SRC)/test_hypergraph.cc \
		-o test_hypergraph

test_parse: $(SRC)/test_parse.cc $(SRC)/parse.hh \
						$(SRC)/grammar.hh $(SRC)/util.hh
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ -lm $(TCMALLOC) \
		$(SRC)/test_parse.cc \
		-o test_parse

test_sparse_vector: $(SRC)/test_sparse_vector.cc $(SRC)/sparse_vector.hh
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ -lm $(TCMALLOC) \
		$(SRC)/test_sparse_vector.cc \
		-o test_sparse_vector

clean:
	rm -f fast_weaver hypergraph.o
	rm -f make_pak read_pak
	rm -f test_grammar test_sparse_vector test_parse

