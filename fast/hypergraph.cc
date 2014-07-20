#include "hypergraph.hh"


namespace Hg {

/*
 * Node
 *
 */
std::ostream&
operator<<(std::ostream& os, const Node& n)
{
  os << \
    "Node<id=" << n.id << \
    ", symbol='" << n.symbol << "'" << \
    ", span=(" << n.left << "," << n.right << ")" \
    ", score=" << n.score << \
    ", incoming:" << n.incoming.size() << \
    ", outgoing:" << n.outgoing.size() << \
    ", mark=" << n.mark << ">";
  return os;
}

/*
 * Edge
 *
 */
std::ostream&
operator<<(std::ostream& os, const Edge& e)
{
  ostringstream _;
  for (auto it = e.tails.begin(); it != e.tails.end(); ++it) {
    _ << (**it).id; if (*it != e.tails.back()) _ << ",";
  }
  os << \
    "Edge<head=" << e.head->id << \
    ", tails=[" << _.str() << "]" \
    ", score=" << e.score << \
    ", rule:'" << "TODO" << "'" <<  \
    ", f=" << "TODO" << \
    ", arity=" << e.arity << \
    ", mark=" << e.mark << ">";
  return os;
}

/*
 * functions
 *
 */
void
reset(list<Node*> nodes, vector<Edge*> edges)
{
  for (auto it = nodes.begin(); it != nodes.end(); ++it)
    (**it).mark = 0;
  for (auto it = edges.begin(); it != edges.end(); ++it)
    (**it).mark = 0;
}

template<typename Semiring>  void
init(list<Node*>& nodes, list<Node*>::iterator root, Semiring& semiring)
{
  for (auto it = nodes.begin(); it != nodes.end(); ++it)
    (**it).score = semiring.null;
  (**root).score = semiring.one;
}

void
topological_sort(list<Node*>& nodes, list<Node*>::iterator root)
{
  cout << "root " << **root << endl;
  for (auto it = nodes.begin(); it != nodes.end(); it++)
    cout << (**it).id << endl;
  auto p = root;
  auto to = nodes.begin();
  while (to != nodes.end()) {
    if ((**p).is_marked()) {
      cout << **p<< endl;
      // explore edges
      for (auto e = (**p).outgoing.begin(); e!=(**p).outgoing.end(); ++e) {
        (**e).mark++;
        cout << " " << **e << endl;
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
      /*if (p == nodes.end()) {
        for (auto e = (**to).outgoing.begin(); e!=(**to).outgoing.end(); ++e) {
          // explore edges
          (**e).mark++;
          if ((**e).is_marked()) {
            (**e).head->mark++;
          }
        }
        to = next(to);
        p = to;
      }*/
    }
  }
  cout << "---" << endl;
  for (auto it = nodes.begin(); it != nodes.end(); it++)
    cout << (**it).id << endl;
}

void
viterbi(Hypergraph& hg)
{
  list<Node*>::iterator root = \
    find_if(hg.nodes.begin(), hg.nodes.end(), [](Node* n) { return n->incoming.size() == 0; });
  Hg::topological_sort(hg.nodes, root);
  Semiring::Viterbi<double> semiring;
  Hg::init(hg.nodes, root, semiring);
  for (auto n = hg.nodes.begin(); n != hg.nodes.end(); ++n) {
    for (auto e = (**n).incoming.begin(); e != (**n).incoming.end(); ++e) {
      double s = semiring.one;
      for (auto m = (**e).tails.begin(); m != (**e).tails.end(); ++m) {
        s = semiring.multiply(s, (**m).score);
      }
      (**n).score = semiring.add((**n).score, semiring.multiply(s, (**e).score));
    }
  }
}

namespace io {

void
read(Hypergraph& hg, string fn)
{
  ifstream ifs(fn);
  size_t i = 0, nn, ne;
  msgpack::unpacker pac;
  while(true) {
    pac.reserve_buffer(32*1024);
    size_t bytes = ifs.readsome(pac.buffer(), pac.buffer_capacity());
    pac.buffer_consumed(bytes);
    msgpack::unpacked result;
    while(pac.next(&result)) {
      msgpack::object o = result.get();
      if (i ==  0) {
        o.convert(&nn);
        nn += 1;
      } else  if (i == 1) {
        o.convert(&ne);
        ne += 1;
      } else if (i > 1 && i <= nn) {
        Node* n = new Node;
        o.convert(n);
        hg.nodes.push_front(n); // FIXME
        hg.nodes_by_id[n->id] = n;
      } else if (i > nn && i <= nn+ne+1) {
        Edge* e = new Edge;
        e->arity = 0;
        o.convert(e);
        e->head = hg.nodes_by_id[e->head_id_];
        hg.edges.push_back(e);
        hg.nodes_by_id[e->head_id_]->incoming.push_back(e);
        e->arity = 0;
        for (auto it = e->tails_ids_.begin(); it != e->tails_ids_.end(); ++it) {
          hg.nodes_by_id[*it]->outgoing.push_back(e);
          e->tails.push_back(hg.nodes_by_id[*it]);
          e->arity++;
        }
      }
      i++;
    }
    if (!bytes) break;
  }
}

void
write(Hypergraph& hg, string fn)
{
  /*FILE* file = fopen(argv[2], "wb");
  msgpack::fbuffer fbuf(file);
  msgpack::pack(fbuf, hg.nodes.size());
  msgpack::pack(fbuf, hg.edges.size());
  msgpack::pack(fbuf, hg.weights);
  for (auto it = hg.nodes.begin(); it != hg.nodes.end(); it++)
    msgpack::pack(fbuf, *it);
  for (auto it = hg.edges.begin(); it != hg.edges.end(); it++)
    msgpack::pack(fbuf, *it);

  fclose(file);*/
}

void
manual(Hypergraph& hg)
{
  // nodes
  Node* a = new Node; a->id = 0; a->symbol = "root"; a->left = -1;    a->right = -1; a->mark = 0;
  Node* b = new Node; b->id = 1; b->symbol = "NP";   b->left = 0;     b->right = 1;  b->mark = 0;
  Node* c = new Node; c->id = 2; c->symbol = "V";    c->left = 1;     c->right = 2;  c->mark = 0;
  Node* d = new Node; d->id = 3; d->symbol = "JJ";   d->left = 3;     d->right = 4;  d->mark = 0;
  Node* e = new Node; e->id = 4; e->symbol = "NN";   e->left = 3;     e->right = 5;  e->mark = 0;
  Node* f = new Node; f->id = 5; f->symbol = "NP";   f->left = 2;     f->right = 5;  f->mark = 0;
  Node* g = new Node; g->id = 6; g->symbol = "NP";   g->left = 1;     g->right = 5;  g->mark = 0;
  Node* h = new Node; h->id = 7; h->symbol = "S";    h->left = 0;     h->right = 6;  h->mark = 0;

  hg.add_node(b);
  hg.add_node(c);
  hg.add_node(d);
  hg.add_node(e);
  hg.add_node(f);
  hg.add_node(g);
  hg.add_node(h);
  hg.add_node(a);

  // edges
  Edge* q = new Edge; q->head = hg.nodes_by_id[1]; q->tails.push_back(hg.nodes_by_id[0]); q->score = 0.367879441171;
  q->arity = 1; q->mark = 0;
  hg.edges.push_back(q);
  hg.nodes_by_id[1]->incoming.push_back(q);
  hg.nodes_by_id[0]->outgoing.push_back(q);

  Edge* p = new Edge; p->head = hg.nodes_by_id[2]; p->tails.push_back(hg.nodes_by_id[0]); p->score = 0.606530659713;
  p->arity = 1; p->mark = 0;
  hg.edges.push_back(p);
  hg.nodes_by_id[2]->incoming.push_back(p);
  hg.nodes_by_id[0]->outgoing.push_back(p);

  Edge* r = new Edge; r->head = hg.nodes_by_id[3]; r->tails.push_back(hg.nodes_by_id[0]); r->score = 1.0;
  r->arity = 1; r->mark = 0;
  hg.edges.push_back(r);
  hg.nodes_by_id[3]->incoming.push_back(r);
  hg.nodes_by_id[0]->outgoing.push_back(r);

  Edge* s = new Edge; s->head = hg.nodes_by_id[3]; s->tails.push_back(hg.nodes_by_id[0]); s->score = 1.0;
  s->arity = 1; s->mark = 0;
  hg.edges.push_back(s);
  hg.nodes_by_id[3]->incoming.push_back(s);
  hg.nodes_by_id[0]->outgoing.push_back(s);

  Edge* t = new Edge; t->head = hg.nodes_by_id[4]; t->tails.push_back(hg.nodes_by_id[0]); t->score = 1.0;
  t->arity = 1; t->mark = 0;
  hg.edges.push_back(t);
  hg.nodes_by_id[4]->incoming.push_back(t);
  hg.nodes_by_id[0]->outgoing.push_back(t);

  Edge* u = new Edge; u->head = hg.nodes_by_id[4]; u->tails.push_back(hg.nodes_by_id[0]); u->score = 1.0;
  u->arity = 1; u->mark = 0;
  hg.edges.push_back(u);
  hg.nodes_by_id[4]->incoming.push_back(u);
  hg.nodes_by_id[0]->outgoing.push_back(u);

  Edge* v = new Edge; v->head = hg.nodes_by_id[4]; v->tails.push_back(hg.nodes_by_id[3]); v->score = 1.0;
  v->arity = 1; v->mark = 0;
  hg.edges.push_back(v);
  hg.nodes_by_id[4]->incoming.push_back(v);
  hg.nodes_by_id[3]->outgoing.push_back(v);

  Edge* w = new Edge; w->head = hg.nodes_by_id[4]; w->tails.push_back(hg.nodes_by_id[3]); w->score = 2.71828182846;
  w->arity = 1; w->mark = 0;
  hg.edges.push_back(w);
  hg.nodes_by_id[4]->incoming.push_back(w);
  hg.nodes_by_id[3]->outgoing.push_back(w);

  Edge* x = new Edge; x->head = hg.nodes_by_id[5]; x->tails.push_back(hg.nodes_by_id[4]); x->score = 1.0;
  x->arity = 1; x->mark = 0;
  hg.edges.push_back(x);
  hg.nodes_by_id[5]->incoming.push_back(x);
  hg.nodes_by_id[4]->outgoing.push_back(x);

  Edge* y = new Edge; y->head = hg.nodes_by_id[6]; y->tails.push_back(hg.nodes_by_id[2]); y->tails.push_back(hg.nodes_by_id[5]); y->score = 1.0;
  y->arity = 2; y->mark = 0;
  hg.edges.push_back(y);
  hg.nodes_by_id[6]->incoming.push_back(y);
  hg.nodes_by_id[2]->outgoing.push_back(y);
  hg.nodes_by_id[5]->outgoing.push_back(y);

  Edge* z = new Edge; z->head = hg.nodes_by_id[7]; z->tails.push_back(hg.nodes_by_id[1]); z->tails.push_back(hg.nodes_by_id[6]); z->score = 1.0;
  z->arity = 2; z->mark = 0;
  hg.edges.push_back(z);
  hg.nodes_by_id[7]->incoming.push_back(z);
  hg.nodes_by_id[1]->outgoing.push_back(z);
  hg.nodes_by_id[6]->outgoing.push_back(z);
}

} // namespace

} // namespace

