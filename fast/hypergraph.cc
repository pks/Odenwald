#include "hypergraph.hh"

namespace Hg {


/*
 * Node
 *
 */
bool
Node::is_marked()
{
  return mark >= incoming.size();
}

std::ostream&
operator<<(std::ostream& os, const Node& n)
{
  os << \
    "Node<id=" << n.id << \
    ", symbol=" << n.symbol << \
    ", span=(" << n.left << "," << n.right << ")" \
    ", score=" << n.score << \
    ", incoming:" << n.incoming.size() << \
    ", outgoing:" << n.outgoing.size() << \
    ", mark=" << n.mark << ">";
}

/*
 * Edge
 *
 */
bool
Edge::is_marked()
{
  return mark >= arity;
}

std::ostream&
operator<<(std::ostream& os, const Edge& e)
{
  ostringstream _;
  for (auto it = e.tails.begin(); it != e.tails.end(); ++it) {
    _ << (*it)->id; if (*it != e.tails.back()) _ << ",";
  }
  os << \
    "Edge<head=" << e.head->id << \
    "', tails=[" << _.str() << "]" \
    ", score=" << e.score << \
    ", rule:'" << "TODO" << \
    " , f=" << "TODO" << \
    ", arity=" << e.arity << \
    ", mark=" << e.mark << ">";
  return os;
}

/*
 * Hypergraph
 * methods
 *
 */
void
Hypergraph::reset()
{
}

/*
 * functions
 *
 */
void
topological_sort(list<Node*>& nodes, list<Node*>::iterator root)
{
  auto p = root;
  (**p).mark = 0; // is_marked()==true
  auto to = nodes.begin();
  while (to != nodes.end()) {
    if ((**p).is_marked()) {
      // explore edges
      for (auto e = (**p).outgoing.begin(); e!=(**p).outgoing.end(); ++e) {
        (**e).mark++;
        if ((**e).is_marked()) {
          (**e).head->mark++;
        }
      }
    }
    if ((**p).is_marked()) {
      nodes.splice(to, nodes, p);
      to = next(p);
      p = to;
    } else {
      ++p;
    }
  }
}

/*void
init(vector<Node*>& nodes, ViterbiSemiring<double>& semiring, Node* root)
{
  for (auto it = nodes.begin(); it != nodes.end(); ++it)
    (*it)->score = semiring.null;
  root->score = semiring.one;
}

void
viterbi(vector<Node*>& nodes, map<unsigned int, Hg::Node*> nodes_by_id, Node* root)
{
  vector<Node*> sorted = topological_sort(nodes);
  ViterbiSemiring<double> semiring;

  init(sorted, semiring, root);

  for (auto n_it = sorted.begin(); n_it != sorted.end(); ++n_it) {
    for (auto e_it = (*n_it)->incoming.begin(); e_it != (*n_it)->incoming.end(); ++e_it) {
      cout << (*e_it)->s() << endl;
      double s = semiring.one;
      for (auto m_it = (*e_it)->tails.begin(); m_it != (*e_it)->tails.end(); m_it++) {
        s = semiring.multiply(s, (*m_it)->score);
      }
      (*n_it)->score = semiring.add((*n_it)->score, semiring.multiply(s, (*e_it)->score));
    }
  }

  for (auto it = sorted.begin(); it != sorted.end(); ++it) {
    cout << (*it)->id << " " <<  (*it)->score << endl;
  }
}*/


} // namespace

