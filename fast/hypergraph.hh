#ifndef HYPERGRAPH_HH
#define HYPERGRAPH_HH

#include "grammar.hh"
#include "semiring.hh"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>

#include <msgpack.hpp>

using namespace std;

typedef double score_t;
typedef double weight_t;
typedef size_t id_t;

namespace Hg {



class Node;

class Edge {
  public:
    id_t             head;
    vector<id_t>     tails;
    score_t          score;
    vector<weight_t> f;
    unsigned int     mark;
  //Grammar::Rule    rule; FIXME
    unsigned int     arity_;

    unsigned int arity();
    bool is_marked();
    string s();

    MSGPACK_DEFINE(head, tails, score, f, mark, arity_);
};


class Node {
  public:
    unsigned int id;
    string       symbol;
    unsigned int left;
    unsigned int right;
    score_t      score;
    vector<id_t> outgoing;
    vector<id_t> incoming;

    string s();

    MSGPACK_DEFINE(id, symbol, left, right, score, outgoing, incoming);
};



class Hypergraph {
  public:
    vector<Node> nodes;
    vector<Edge> edges;
    unsigned int arity_;

    unsigned int arity();
    void reset();
    string s();
    string json_s();

    MSGPACK_DEFINE(nodes, edges, arity_, nodes_by_id);
};

vector<Node*> topological_sort(vector<Node*>& nodes);
void viterbi(vector<Node*>& nodes, map<unsigned int, Hg::Node*> nodes_by_id, Node* root);


} // namespace

#endif

