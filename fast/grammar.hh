#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <msgpack.hpp>
#include <vector>

#include "sparse_vector.hh"
#include "util.hh"

using namespace std;


namespace G {

struct NT {
  string symbol;
  size_t index;

  NT() {};
  NT(string& s);

  string repr() const;
  string escaped() const;

  friend ostream& operator<<(ostream& os, const NT& t);
};

struct T {
  string word; // use word ids instead?

  T(const string& s);

  string repr() const;
  string escaped() const;

  friend ostream& operator<<(ostream& os, const NT& nt);
};

enum item_type {
  NON_TERMINAL,
  TERMINAL
};

struct Item {
 item_type type;
       NT* nt;
        T* t;

  Item(string& s);

  string repr() const;
  string escaped() const;

  friend ostream& operator<<(ostream& os, const Item& i);
};

struct Rule {
                               NT* lhs;
                     vector<Item*> rhs;
                     vector<Item*> target;
                            size_t arity;
Sv::SparseVector<string, score_t>* f;
               map<size_t, size_t> order;
                            string as_str_; // FIXME

  Rule() {};
  Rule(const string& s);

  string repr() const;
  string escaped() const;

  friend ostream& operator<<(ostream& os, const Rule& r);

  void prep_for_serialization_() { as_str_ = escaped(); }; // FIXME

  MSGPACK_DEFINE(as_str_); // TODO
};

struct Grammar {
  vector<Rule*> rules;
  vector<Rule*> flat;
  vector<Rule*> start_nt;
  vector<Rule*> start_t;

  Grammar() {};
  Grammar(const string& fn);

  void add_glue(); // TODO
  void add_pass_through(const string& input); // TODO

  friend ostream& operator<<(ostream& os, const Grammar& g);
};

} // namespace G

