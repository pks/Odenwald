#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

#include "dummyvector.h"

using namespace std;


namespace G {

struct NT {
        string symbol;
  unsigned int index;

  NT() {};
  NT(string& s);
  string repr() const;
  friend ostream& operator<<(ostream& os, const NT& t);
};

struct T {
  string word;

  T(string& s);
  string repr() const;
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
  friend ostream& operator<<(ostream& os, const Item& i);
};

struct Rule {
                      NT* lhs;
            vector<Item*> rhs;
            vector<Item*> target;
            //map<int,int> map;
                   size_t arity;
              DummyVector f;

  Rule() {};
  Rule(string& s);
  string repr() const;
  friend ostream& operator<<(ostream& os, const Rule& r);
};

struct Grammar {
  vector<Rule*> rules;
  vector<Rule*> flat;
  vector<Rule*> start_nt;
  vector<Rule*> start_t;

  Grammar(string fn);
  void add_glue();
  void add_pass_through();
  friend ostream& operator<<(ostream& os, const Grammar& g);
};

} // namespace G

