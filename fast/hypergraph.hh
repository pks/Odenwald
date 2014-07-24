#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <msgpack.hpp>
#include <msgpack/fbuffer.hpp>

#include "grammar.hh"
#include "semiring.hh"
#include "dummyvector.h"
#include "sparse_vector.hh"

using namespace std;

typedef double score_t;
typedef double weight_t;


namespace Hg {

struct Node;

struct Edge {
             Node* head;
     vector<Node*> tails;
           score_t score;
          G::Rule* rule;
      unsigned int arity = 0;
      unsigned int mark = 0;

  inline bool is_marked() { return mark >= arity; }
  friend ostream& operator<<(ostream& os, const Edge& s);

          size_t head_id_;
  vector<size_t> tails_ids_; // node ids
          size_t rule_id_;

  MSGPACK_DEFINE(head_id_, tails_ids_, rule_id_, score, arity);
};

struct Node {
          size_t id;
          string symbol;
           short left;
           short right;
         score_t score;
   vector<Edge*> incoming;
   vector<Edge*> outgoing;
    unsigned int mark;

  inline bool is_marked() { return mark >= incoming.size(); };
  friend ostream& operator<<(ostream& os, const Node& n);

  MSGPACK_DEFINE(id, symbol, left, right, score);
};

struct Hypergraph {
                   list<Node*> nodes;
                 vector<Edge*> edges;
  unordered_map<size_t, Node*> nodes_by_id;
                  unsigned int arity;
};

void
reset();

template<typename Semiring> void
init(list<Node*>& nodes, list<Node*>::iterator root, Semiring& semiring);

void
topological_sort(list<Node*>& nodes, list<Node*>::iterator root);

void
viterbi(Hypergraph& hg);

namespace io {

void
read(Hypergraph& hg, vector<G::Rule*> rules, string fn);

void
write(Hypergraph& hg, vector<G::Rule*> rules, string fn);

void
manual(Hypergraph& hg);

} // namespace

} // namespace

