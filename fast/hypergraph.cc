#include "hypergraph.hh"

namespace Hg {


/*
 * Node
 * methods
 *
 */
string
Node::s()
{
  ostringstream os;
  os << \
    "Node<id=" << this->id << \
    ", symbol=" << this->symbol << \
    ", span=(" << this->left << "," << this->right << ")" \
    ", outgoing:" << this->outgoing.size() << \
    ", incoming:" << this->incoming.size() << ">";
  return os.str();
}

/*
 * Hyperedge
 * methods
 *
 */
unsigned int
Hyperedge::arity()
{
  return this->arity_;
}

bool
Hyperedge::is_marked()
{
  return (this->arity_ == this->mark);
}

string
Hyperedge::s()
{
  ostringstream os;
  vector<unsigned int> tails_ids;
  ostringstream _;
  for_each(this->tails.begin(), this->tails.end(), [&_](Node* n) { _ << n->id; _ << ","; });
  os << \
    "Hyperedge<head=" << this->head->id << \
    ", rule:'" << "TODO" << \
    "', tails=" << _.str() << \
    ", arity=" << this->arity() << \
    ", score=" << this->score << \
    " , f=" << "TODO" << \
    ", mark=" << this->mark << ">";
  return os.str();
}


/*
 * Hypergraph
 * methods
 *
 */
unsigned int
Hypergraph::arity()
{
  // TODO
  return 0;
}

void
Hypergraph::reset()
{
}

string
Hypergraph::s()
{
  // TODO
  return "";
}

string
Hypergraph::json_s()
{
  // TODO
  return "";
}

/*
 * functions
 *
 */
vector<Node*>
topological_sort(vector<Node*>& nodes)
{
  vector<Node*> sorted;
  vector<Node*> tmp;
  for_each(nodes.begin(), nodes.end(), [&tmp](Node* n) -> void { if (n->incoming.size()==0) tmp.push_back(n); });
  while (!tmp.empty()) {
    Node* n = tmp.front();
    cout << n->id << endl;
    tmp.erase(tmp.begin());
    sorted.push_back(n);
    for (auto it = n->outgoing.begin(); it !=  n->outgoing.end(); it++) { // edges
      if ((*it)->is_marked()) {
        continue;
      }
      (*it)->mark++;
      bool add = true;
      for (auto jt = (*it)->head->incoming.begin(); jt != (*it)->head->incoming.end(); jt++) {
        if (!(*jt)->is_marked()) {
          add = false;
          break;
        }
      }
      if (add==true)  {
        tmp.push_back( (*it)->head );
        cout << " " <<  (*it)->head->id<< endl;
      }
    }
  }
  return sorted;
}

void
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
}


} // namespace

