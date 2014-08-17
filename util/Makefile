COMPILER=clang


all: make_pak read_pak

make_pak: make_pak.cc json-cpp/single_include/json-cpp.hpp ../fast/hypergraph.hh ../fast/weaver.hh
	$(COMPILER) -std=c++11 -lstdc++ -lm -lmsgpack make_pak.cc -o make_pak

read_pak: read_pak.cc
	$(COMPILER) -std=c++11 -lstdc++ -lmsgpack read_pak.cc -o read_pak

clean:
	rm -f make_pak read_pak

