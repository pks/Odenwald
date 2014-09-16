#pragma once

#include <string>

#include "types.hh"

using namespace std;


namespace util {

inline string
json_escape(const string& s)
{
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

inline vector<symbol_t>
tokenize(string s)
{
  istringstream ss(s);
  vector<symbol_t> r;
  while (ss.good()) {
    string buf;
    ss >> buf;
    r.push_back(buf);
  }

  return r;
}

} // namespace util

