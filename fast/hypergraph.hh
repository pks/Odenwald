#pragma once

#include "grammar.hh"
#include "semiring.hh"
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

#include "dummyvector.h"
#include <msgpack.hpp>

using namespace std;

typedef double score_t;
typedef double weight_t;

namespace Hg {


struct Node;

struct Edge {
             Node* head;
     vector<Node*> tails;
           score_t score;
            string rule; //FIXME
       DummyVector f; //FIXME
      unsigned int arity;
      unsigned int mark;

  bool is_marked();
  friend std::ostream& operator<<(std::ostream& os, const Edge& s);

          size_t head_id_;
  vector<size_t> tails_ids_; // node ids

  MSGPACK_DEFINE(head_id_, tails_ids_, score, f, arity);
};

struct Node {
          size_t id;
          string symbol;
  unsigned short left;
  unsigned short right;
         score_t score;
   vector<Edge*> incoming;
   vector<Edge*> outgoing;
    unsigned int mark;

  bool is_marked();
  friend std::ostream& operator<<(std::ostream& os, const Node& n);

  vector<size_t> incoming_ids_; // edge ids
  vector<size_t> outgoing_ids_; // edge ids
  MSGPACK_DEFINE(id, symbol, left, right, score, incoming_ids_, outgoing_ids_);
};

struct Hypergraph {
                   list<Node*> nodes;
                 vector<Edge*> edges;
  unordered_map<size_t, Node*> nodes_by_id;
                  unsigned int arity;

  void add_node(Node* n) { nodes.push_back(n); nodes_by_id[n->id] = n; }
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
read(Hypergraph& hg, string fn);

void
write(Hypergraph& hg, string fn);

void
manual(Hypergraph& hg);

} // namespace


} // namespace

