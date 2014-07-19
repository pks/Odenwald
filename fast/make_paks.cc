#include <iostream>
#include <fstream>
#include <string>
#include <msgpack.hpp>
#include <msgpack/fbuffer.h>
#include <msgpack/fbuffer.hpp>
#include <unordered_map>

#include "json-cpp.hpp"
#include "hypergraph.hh"
#include "dummyvector.h"

using namespace std;


struct DummyNode {
  int id;
  string cat;
  vector<int> span;
};

struct DummyEdge {
  int head;
  string rule;
  vector<size_t> tails;
  DummyVector f;
  score_t weight;
};

struct DummyHg {
  vector<DummyNode> nodes;
  vector<DummyEdge> edges;
  DummyVector weights;
};

template<typename X> inline void
serialize(jsoncpp::Stream<X>& stream, DummyNode& o)
{
  fields(o, stream, "id", o.id, "cat", o.cat, "span", o.span);
}

template<typename X> inline void
serialize(jsoncpp::Stream<X>& stream, DummyEdge& o)
{
  fields(o, stream, "head", o.head, "rule", o.rule, "tails", o.tails, "f", o.f, "weight", o.weight);
}

template<typename X> inline void
serialize(jsoncpp::Stream<X>& stream, DummyHg& o)
{
  fields(o, stream, "nodes", o.nodes, "edges", o.edges, "weights", o.weights);
}

template<typename X> inline void
serialize(jsoncpp::Stream<X>& stream, DummyVector& o)
{
  fields(o, stream, "EgivenFCoherent", o.EgivenFCoherent, "SampleCountF", o.SampleCountF, "CountEF", o.CountEF, "MaxLexFgivenE", o.MaxLexFgivenE, "MaxLexEgivenF", o.MaxLexEgivenF, "IsSingletonF", o.IsSingletonF, "IsSingletonFE", o.IsSingletonFE, "LanguageModel", o.LanguageModel, "LanguageModel_OOV", o.LanguageModel_OOV, "PassThrough", o.PassThrough, "PassThrough_1", o.PassThrough_1, "PassThrough_2", o.PassThrough_2, "PassThrough_3", o.PassThrough_3, "PassThrough_4", o.PassThrough_4, "PassThrough_5", o.PassThrough_5, "PassThrough_6", o.PassThrough_6, "WordPenalty", o.WordPenalty, "Glue", o.Glue);
}



int
main(int argc, char** argv)
{
  ifstream ifs(argv[1]);
  string json_str((istreambuf_iterator<char>(ifs) ),
                   (istreambuf_iterator<char>()));

  DummyHg hg;
  vector<DummyNode> nodes;
  hg.nodes = nodes;
  vector<DummyEdge> edges;
  hg.edges = edges;
  DummyVector w;
  hg.weights = w;
  jsoncpp::parse(hg, json_str);

  vector<Hg::Node*> nodes_;
  for (auto it = hg.nodes.begin(); it != hg.nodes.end(); ++it) {
    Hg::Node* n = new Hg::Node; 
    n->id = it->id; 
    n->symbol = it->cat;
    n->left = it->span[0];
    n->right = it->span[1];
    nodes_.push_back(n);
  }

  vector<Hg::Edge*> edges_;
  for (auto it = hg.edges.begin(); it != hg.edges.end(); ++it) {
    Hg::Edge* e = new Hg::Edge; 
    e->head_id_ = it->head;
    e->tails_ids_ = it->tails;
    e->score = it->weight;
    e->rule = it->rule;
    e->f = it->f;
    edges_.push_back(e);
  }

  FILE* file = fopen(argv[2], "wb");
  msgpack::fbuffer fbuf(file);
  msgpack::pack(fbuf, hg.nodes.size());
  msgpack::pack(fbuf, hg.edges.size());
  for (auto it = nodes_.begin(); it != nodes_.end(); ++it)
    msgpack::pack(fbuf, **it);
  for (auto it = edges_.begin(); it != edges_.end(); ++it)
    msgpack::pack(fbuf, **it);

  fclose(file);

  return 0;
}

