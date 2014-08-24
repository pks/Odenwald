#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <msgpack.hpp>

#include "sparse_vector.hh"
#include "util.hh"

using namespace std;


namespace G {

enum item_type {
  UNDEFINED,
  TERMINAL,
  NON_TERMINAL
};

struct Item {
  virtual size_t index() const { return 0; }
  virtual symbol_t symbol() const { return ""; }
  virtual item_type type() const { return UNDEFINED; }

  virtual ostream& repr(ostream& os) const { return os << "Item<>"; }
  virtual ostream& escaped(ostream& os) const { return os << ""; }

  friend ostream&
  operator<<(ostream& os, const Item& i)
  {
    return i.repr(os);
  };
};

struct NT : public Item {
  symbol_t symbol_;
  size_t index_;

  NT() {}

  NT(string const& s)
  {
    index_ = 0; // default
    string t(s);
    t.erase(0, 1); t.pop_back(); // remove '[' and ']'
    istringstream ss(t);
    if (ss >> index_) { // [i]
      symbol_ = "";
      index_ = stoi(s);
      return;
    } else {
      ss.clear();
      string buf;
      size_t j = 0;
      while (ss.good() && getline(ss, buf, ',')) {
        if (j == 0) {
          symbol_ = buf;
        } else {
          index_ = stoi(buf);
        }
        j++;
      }
    }
  }

  virtual size_t index() const { return index_; }
  virtual symbol_t symbol() const { return symbol_; }
  virtual item_type type() const { return NON_TERMINAL; }

  virtual ostream&
  repr(ostream& os) const
  {
    return os << "NT<" << symbol_ << "," << index_ << ">";
  }

  virtual ostream&
  escaped(ostream& os) const
  {
    os << "[" << symbol_;
    if (index_ > 0)
      os << "," << index_;
    os << "]";

    return os;
  }
};

struct T : public Item {
  symbol_t symbol_;

  T(string const& s)
  {
    symbol_ = s;
  }

  virtual symbol_t symbol() const { return symbol_; }
  virtual item_type type() const { return TERMINAL; }

  virtual ostream&
  repr(ostream& os) const
  {
    return os << "T<" << symbol_ << ">";
  }

  virtual ostream&
  escaped(ostream& os) const
  {
    os << util::json_escape(symbol_);
  }
};

struct Vocabulary
{
  unordered_map<symbol_t, size_t> m_;
  vector<Item*> items_;

  bool
  is_non_terminal(string const& s)
  {
    return s.front() == '[' && s.back() == ']';
  }

  Item*
  get(symbol_t const& s)
  {
    if (is_non_terminal(s))
      return new NT(s);
    if (m_.find(s) != m_.end())
      return items_[m_[s]];
    return add(s);
  }

  Item*
  add(symbol_t const& s)
  {
    size_t next_index_ = items_.size();
    T* item = new T(s);
    items_.push_back(item);
    m_[s] = next_index_;

    return item;
  }
};

struct Rule {
                               NT* lhs;
                     vector<Item*> rhs;
                     vector<Item*> target;
                            size_t arity;
Sv::SparseVector<string, score_t>* f;
               map<size_t, size_t> order;
                            string as_str_;

  Rule() {}

  Rule(string const& s, Vocabulary& vocab) { from_s(this, s, vocab); }

  static void
  from_s(Rule* r, string const& s, Vocabulary& vocab)
  {
    istringstream ss(s);
    string buf;
    size_t j = 0, i = 1;
    r->arity = 0;
    vector<NT*> rhs_non_terminals;
    r->f = new Sv::SparseVector<string, score_t>();
    while (ss >> buf) {
      if (buf == "|||") { j++; continue; }
      if (j == 0) {        // left-hand side
        r->lhs = new NT(buf);
      } else if (j == 1) { // right-hand side
        Item* item = vocab.get(buf);
        r->rhs.push_back(item);
        if (item->type() == NON_TERMINAL) {
          r->arity++;
          rhs_non_terminals.push_back(reinterpret_cast<NT*>(item));
        }
      } else if (j == 2) { // target
        Item* item = vocab.get(buf);
        if (item->type() == NON_TERMINAL) {
          r->order.insert(make_pair(i, item->index()));
          i++;
          if (item->symbol() == "") { // only [1], [2] ... on target
            reinterpret_cast<NT*>(item)->symbol_ = \
             rhs_non_terminals[item->index()-1]->symbol();
          }
        }
        r->target.push_back(item);
      } else if (j == 3) { // feature vector
        Sv::SparseVector<string, score_t>::from_s(r->f, buf);
        // FIXME: this is slow!!!          ^^^ 
      } else if (j == 4) { // alignment
      } else {
        // error
      }
      if (j == 4) break;
    }
  }

  ostream&
  repr(ostream& os) const
  {
    os << "Rule<lhs=";
    lhs->repr(os);
    os << ", rhs:{";
    for (auto it = rhs.begin(); it != rhs.end(); it++) {
      (**it).repr(os);
      if (next(it) != rhs.end()) os << " ";
    }
    os << "}, target:{";
    for (auto it = target.begin(); it != target.end(); it++) {
      (**it).repr(os);
      if (next(it) != target.end()) os << " ";
    }
    os << "}, f:";
    f->repr(os);
    os << ", arity=" << arity << \
     ", order:{";
    for (auto it = order.begin(); it != order.end(); it++) {
      os << it->first << "->" << it->second;
      if (next(it) != order.end()) os << ", ";
    }
    os << "}>";

    return os;
  }

  ostream&
  escaped(ostream& os) const
  {
    lhs->escaped(os);
    os << " ||| ";
    for (auto it = rhs.begin(); it != rhs.end(); it++) {
      (**it).escaped(os);
      if (next(it) != rhs.end()) os << " ";
    }
    os << " ||| ";
    for (auto it = target.begin(); it != target.end(); it++) {
      (**it).escaped(os);
      if (next(it) != target.end()) os << " ";
    }
    os << " ||| ";
    f->escaped(os);
    os << " ||| " << \
     "TODO";

    return os;
  };

  friend ostream&
  operator<<(ostream& os, Rule const& r)
  {
    return r.repr(os);
  };

  // --
  void
  prep_for_serialization_()
  {
    ostringstream os;
    escaped(os);
    as_str_ = os.str();
  };
  MSGPACK_DEFINE(as_str_);
  //             ^^^ FIXME
};

struct Grammar {
  vector<Rule*> rules;
  vector<Rule*> flat;
  vector<Rule*> start_non_terminal;
  vector<Rule*> start_terminal;

  Grammar() {}

  Grammar(string const& fn, Vocabulary& vocab)
  {
    ifstream ifs(fn);
    string line;
    while (getline(ifs, line)) {
      G::Rule* r = new G::Rule(line, vocab);
      rules.push_back(r);
      if (r->arity == 0)
        flat.push_back(r);
      else if (r->rhs.front()->type() == NON_TERMINAL)
        start_non_terminal.push_back(r);
      else
        start_terminal.push_back(r);
    }
  }

  void add_glue();
  //   ^^^ TODO
  void add_pass_through(const string& input);
  //   ^^^ TODO

  friend ostream&
  operator<<(ostream& os, Grammar const& g)
  {
    for (const auto it: g.rules) {
      it->repr(os);
      os << endl;
    }

    return os;
  }
};

} // namespace G

