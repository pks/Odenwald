#include "grammar.hh"


string
esc_str(const string& s) { // FIXME
  ostringstream os;
  for (auto it = s.cbegin(); it != s.cend(); it++) {
    switch (*it) {
      case '"':  os << "\\\""; break;
      case '\\': os << "\\\\"; break;
      case '\b': os << "\\b";  break;
      case '\f': os << "\\f";  break;
      case '\n': os << "\\n";  break;
      case '\r': os << "\\r";  break;
      case '\t': os << "\\t";  break;
      default:   os << *it;    break;
    }
  }

  return os.str();
}

namespace G {

NT::NT(string& s)
{
  s.erase(0, 1);
  s.pop_back();
  stringstream ss(s);
  string buf;
  size_t c = 0;
  index = 0;
  while (ss.good() && getline(ss, buf, ',')) {
    if (c == 0) {
      symbol = buf;
    } else {
      index = stoi(buf);
    }
    c++;
  }
}

T::T(string& s)
{
  word = s;
}

Item::Item(string& s)
{
  if (s.front() == '[' && s.back() == ']') {
    type = NON_TERMINAL;
    nt = new NT(s);
  } else {
    type = TERMINAL;
    t = new T(s);
  }
}

Rule::Rule(string& s)
{
  stringstream ss(s);
  size_t c = 0;
  string buf;
  while (ss >> buf) {
    if (buf == "|||") { c++; continue; }
    if (c == 0) {        // LHS
      lhs = new NT(buf);
    } else if (c == 1) { // RHS
      rhs.push_back(new Item(buf));
      if (rhs.back()->type == NON_TERMINAL) arity++;
    } else if (c == 2) { // TARGET
      target.push_back(new Item(buf));
    } else if (c == 3) { // F TODO
    } else if (c == 4) { // A TODO
    } else {             // ERROR FIXME
    }
    if (c == 4) break;
  }
  arity = 0;
}

Grammar::Grammar(string fn)
{
  ifstream ifs(fn);
  string line;
  while (getline(ifs, line)) {
    G::Rule* r = new G::Rule(line);
    rules.push_back(r);
    if (r->arity == 0)
      flat.push_back(r);
    else if (r->rhs.front()->type == NON_TERMINAL)
      start_nt.push_back(r);
    else
      start_t.push_back(r);
  }
}

string
Item::repr() const
{
  ostringstream os;
  if (type == TERMINAL)
    os << t->repr();
  else
    os << nt->repr();

  return os.str();
}

string
Item::escaped() const
{
  ostringstream os;
  if (type == TERMINAL)
    os << t->escaped();
  else
    os << nt->escaped();

  return os.str();
}

ostream&
operator<<(ostream& os, const Item& i)
{
  return os << i.repr();
}

string
NT::repr() const
{
  ostringstream os;
  os << "NT<" << symbol << "," << index << ">";

  return os.str();
}

string
NT::escaped() const
{
  ostringstream os;
  os << "[" << symbol;
  if (index > 0)
    os << "," << index;
  os << "]";

  return os.str();
}

ostream&
operator<<(ostream& os, const NT& nt)
{
  return os << nt.repr();
}

string
T::repr() const
{
  ostringstream os;
  os << "T<" << word << ">";

  return os.str();
}

ostream&
operator<<(ostream& os, const T& t)
{
  return os << t.repr();
}

string
Rule::repr() const
{
  ostringstream os;
  os << "Rule<lhs=" << lhs->repr() << \
   ", rhs:{";
  for (auto it = rhs.begin(); it != rhs.end(); it++) {
    os << (**it).repr();
    if (next(it) != rhs.end()) os << " ";
  }
  os << "}, target:{";
  for (auto it = target.begin(); it != target.end(); it++) {
    os << (**it).repr();
    if (next(it) != target.end()) os << " ";
  }
  os << "}" \
   ", f:" << "TODO" << \
   ", arity=" << arity << \
   ", map:" << "TODO" << \
   ">";

  return os.str();
}

ostream&
operator<<(ostream& os, const Rule& r)
{
  return os << r.repr();
}

string
Rule::escaped() const
{
  ostringstream os;
  os << lhs->escaped() << " ||| ";
  for (auto it = rhs.begin(); it != rhs.end(); it++) {
    os << (**it).escaped();
    if (next(it) != rhs.end()) os << " ";
  }
  os << " ||| ";
  for (auto it = target.begin(); it != target.end(); it++) {
    os << (**it).escaped();
    if (next(it) != target.end()) os << " ";
  }
  os << " ||| ";
  os << "TODO";
  os << " ||| ";
  os << "TODO";

  return os.str();
}

ostream&
operator<<(ostream& os, const Grammar& g)
{
  for (auto it = g.rules.begin(); it != g.rules.end(); it++)
    os << (**it).repr() << endl;

  return os;
}

} // namespace G

