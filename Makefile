COMPILER=clang
CFLAGS=-std=c++11 -O3 -Wall

TCMALLOC=$(shell pwd)/external/gperftools-2.1/lib/libtcmalloc_minimal.a -pthread
MSGPACK_C=$(shell pwd)/external/msgpack-c/lib/libmsgpack.a -I./external/msgpack-c/include
JSON_CPP=-I$(shell pwd)/external/json-cpp/include

SRC=src
BIN=bin

################################################################################
# fast_weaver
PRINT = @echo -e "\e[1;34mBuilding $@\e[0m"

all: $(BIN)/fast_weaver util test

$(BIN)/fast_weaver: $(BIN) $(SRC)/hypergraph.o $(SRC)/fast_weaver.cc
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ -lm $(MSGPACK_C) $(TCMALLOC) $(SRC)/hypergraph.o \
		$(SRC)/fast_weaver.cc \
		-o $(BIN)/fast_weaver

$(BIN):
	mkdir -p $(BIN)

$(SRC)/hypergraph.o: $(SRC)/hypergraph.cc $(SRC)/hypergraph.hh \
											$(SRC)/grammar.hh \
											$(SRC)/semiring.hh $(SRC)/sparse_vector.hh \
											$(SRC)/types.hh
	$(PRINT)
	$(COMPILER) $(CFLAGS) -g -c $(TCMALLOC) $(MSGPACK_C) \
		$(SRC)/hypergraph.cc \
		-o $(SRC)/hypergraph.o

################################################################################
# util
util: $(BIN)/make_pak $(BIN)/read_pak

$(BIN)/make_pak: $(BIN) $(SRC)/make_pak.cc external/json-cpp/single_include/json-cpp.hpp \
					$(SRC)/hypergraph.hh $(SRC)/types.hh
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ -lm $(MSGPACK_C) $(JSON_CPP) \
		$(SRC)/make_pak.cc \
		-o $(BIN)/make_pak

$(BIN)/read_pak: $(SRC)/read_pak.cc
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ $(MSGPACK_C) \
		$(SRC)/read_pak.cc \
		-o $(BIN)/read_pak

################################################################################
# test
test: $(BIN)/test_grammar $(BIN)/test_hypergraph $(BIN)/test_parse $(BIN)/test_sparse_vector

$(BIN)/test_grammar: $(BIN) $(SRC)/test_grammar.cc $(SRC)/grammar.hh
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ -lm $(TCMALLOC) $(MSGPACK_C) \
		$(SRC)/test_grammar.cc \
		-o $(BIN)/test_grammar

$(BIN)/test_hypergraph: $(BIN) $(SRC)/test_hypergraph.cc $(SRC)/hypergraph.o $(SRC)/grammar.hh
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ -lm $(TCMALLOC) $(MSGPACK_C) $(SRC)/hypergraph.o \
		$(SRC)/test_hypergraph.cc \
		-o $(BIN)/test_hypergraph

$(BIN)/test_parse: $(BIN) $(SRC)/test_parse.cc $(SRC)/parse.hh \
						$(SRC)/grammar.hh $(SRC)/util.hh
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ -lm $(TCMALLOC) $(MSGPACK_C) \
		$(SRC)/test_parse.cc \
		-o $(BIN)/test_parse

$(BIN)/test_sparse_vector: $(BIN) $(SRC)/test_sparse_vector.cc $(SRC)/sparse_vector.hh
	$(PRINT)
	$(COMPILER) $(CFLAGS) -lstdc++ -lm $(TCMALLOC) \
		$(SRC)/test_sparse_vector.cc \
		-o $(BIN)/test_sparse_vector

################################################################################
# clean
clean:
	rm -f $(SRC)/*.o
	rm -f $(BIN)/*

