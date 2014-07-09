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

using namespace std;

typedef double score_t;
typedef double weight_t;

namespace Hg {


class Node;

class Hyperedge {
  public:
    Node*            head;
    vector<Node*>    tails;
    score_t          score;
    vector<weight_t> f;
    unsigned int     mark;
  //Grammar::Rule    rule; FIXME
    unsigned int     arity_;

    unsigned int arity();
    bool is_marked();
    string s();

};


class Node {
  public:
    unsigned int id;
    string       symbol;
    unsigned int left;
    unsigned int right;
    score_t      score;
    vector<Hyperedge*> outgoing;
    vector<Hyperedge*> incoming;

    string s();
};



class Hypergraph {
  public:
    vector<Node*>      nodes;
    vector<Hyperedge*> edges;
    unsigned int       arity_;
    map<unsigned int, Node*> nodes_by_id;

    unsigned int arity();
    void reset();
    string s();
    string json_s();
};

vector<Node*> topological_sort(vector<Node*>& nodes);
void viterbi(vector<Node*>& nodes, map<unsigned int, Hg::Node*> nodes_by_id, Node* root);


} // namespace

#endif

