#include <iostream>
#include <fstream>
#include <msgpack.hpp>
#include <msgpack/fbuffer.hpp>
#include <string>

#include "json-cpp/single_include/json-cpp.hpp"
#include "../fast/hypergraph.hh"
#include "../fast/weaver.hh"

using namespace std;


struct DummyNode {
         size_t id;
         string symbol;
  vector<short> span;
};

struct DummyEdge {
          size_t head_id;
          size_t rule_id;
  vector<size_t> tails_ids;
          string f;
         score_t score;
};

struct DummyHg {
     vector<string> rules;
  vector<DummyNode> nodes;
  vector<DummyEdge> edges;
};

template<typename X> inline void
serialize(jsoncpp::Stream<X>& stream, DummyNode& o)
{
  fields(o, stream, "id", o.id, "symbol", o.symbol, "span", o.span);
}

template<typename X> inline void
serialize(jsoncpp::Stream<X>& stream, DummyEdge& o)
{
  fields(o, stream, "head", o.head_id, "rule", o.rule_id, "tails", o.tails_ids, "score", o.score);
}

template<typename X> inline void
serialize(jsoncpp::Stream<X>& stream, DummyHg& o)
{
  fields(o, stream, "rules", o.rules, "nodes", o.nodes, "edges", o.edges);
}

int
main(int argc, char** argv)
{
  // read from json
  ifstream ifs(argv[1]);
  string json_str((istreambuf_iterator<char>(ifs) ),
                   (istreambuf_iterator<char>()));
  DummyHg hg;
  vector<string> rules;
  hg.rules = rules;
  vector<DummyNode> nodes;
  hg.nodes = nodes;
  vector<DummyEdge> edges;
  hg.edges = edges;
  jsoncpp::parse(hg, json_str);

  // convert to proper objects
  vector<Hg::Node*> nodes_conv;
  for (const auto it: hg.nodes) {
    Hg::Node* n = new Hg::Node;
    n->id = it.id;
    n->symbol = it.symbol;
    n->left = it.span[0];
    n->right = it.span[1];
    nodes_conv.push_back(n);
  }
  vector<Hg::Edge*> edges_conv;
  for (const auto it: hg.edges) {
    Hg::Edge* e = new Hg::Edge;
    e->head_id_ = it.head_id;
    e->tails_ids_ = it.tails_ids;
    e->score = it.score;
    e->rule_id_ = it.rule_id;
    edges_conv.push_back(e);
  }

  // write to msgpack
  FILE* file = fopen(argv[2], "wb");
  msgpack::fbuffer fbuf(file);
  msgpack::pack(fbuf, hg.rules.size());
  msgpack::pack(fbuf, hg.nodes.size());
  msgpack::pack(fbuf, hg.edges.size());
  for (const auto it: hg.rules)
    msgpack::pack(fbuf, it);
  for (const auto it: nodes_conv)
    msgpack::pack(fbuf, *it);
  for (const auto it: edges_conv)
    msgpack::pack(fbuf, *it);
  fclose(file);

  return 0;
}

