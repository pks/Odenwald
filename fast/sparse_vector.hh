#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "util.hh"
#include "weaver.hh"

using namespace std;


namespace Sv {

template<typename K, typename V>
struct SparseVector {
  unordered_map<K,V> m_;
                   V zero = 0.f;

  SparseVector() {};
  SparseVector(string& s)
  {
    stringstream ss(s);
    while (!ss.eof()) {
      string t;
      ss >> t;
      size_t eq = t.find_first_of("=");
      t.replace(eq, 1, " ");
      stringstream tt(t);
      K k; V v;
      tt >> k >> v;
      m_.emplace(k.substr(k.find_first_of("\"")+1, k.find_last_of("\"")-1), v);
    }
  };

  void
  insert(K k, V v) { m_[k] = v; };

  V
  dot(SparseVector& other)
  {
    V r;
    unordered_map<K,V>* o = &m_;
    auto b = m_.cbegin();
    auto e = m_.cend();
    if (other.size() < size()) {
      b = other.m_.cbegin();
      e = other.m_.cend();
      o = &other.m_;
    }
    for (auto it = b; it != e; it++)
      r += it->second * o->at(it->first);

    return r;
  };

  size_t
  size()
  {
    return m_.size();
  }

  V&
  operator[](const K& k)
  {
    return at(k);
  };

  const V&
  at(const K& k) const
  {
     if (m_.find(k) == m_.end())
      return zero;
    else
      return m_.at(k);
  }

  SparseVector
  operator+(const SparseVector& other) const
  {
    SparseVector<K,V> v;
    v.m_.insert(m_.cbegin(), m_.cend());
    v.m_.insert(other.m_.cbegin(), other.m_.cend());
    for (const auto it: v.m_)
      v.m_[it.first] = this->at(it.first) + other.at(it.first);

    return v;
  };

  SparseVector&
  operator+=(const SparseVector& other)
  {
    for (const auto it: other.m_)
      m_[it.first] += it.second;

    return *this;
  };

  SparseVector
  operator-(const SparseVector& other) const
  {
    SparseVector<K,V> v;
    v.m_.insert(m_.cbegin(), m_.cend());
    v.m_.insert(other.m_.cbegin(), other.m_.cend());
    for (const auto it: v.m_)
      v.m_[it.first] = this->at(it.first) - other.at(it.first);

    return v;
  };

  SparseVector&
  operator-=(const SparseVector& other)
  {
    for (const auto it: other.m_)
      m_[it.first] -= it.second;

    return *this;
  };

  SparseVector
  operator*(V f) const
  {
    SparseVector<K,V> v;
    for (const auto it: m_)
      v.m_[it.first] = this->at(it.first) * f;

    return v;
  };

  SparseVector&
  operator*=(V f)
  {
    for (const auto it: m_)
      m_[it.first] *= f;

    return *this;
  };

  string
  repr() const
  {
    ostringstream os;
    os << "SparseVector<{";
    for (auto it = m_.cbegin(); it != m_.cend(); it++) {
      os << "'" << it->first << "'=" << it->second;
      if (next(it) != m_.end())
        os << ", ";
    }
    os << "}>";

    return os.str();
  };

  string
  escaped() const {
    ostringstream os;
    for (auto it = m_.cbegin(); it != m_.cend(); it++) {
      os << '"' <<  util::json_escape(it->first) << '"' << "=" << it->second;
      if (next(it) != m_.cend()) os << " ";
    }

    return os.str();
  };

  friend ostream& operator<<(ostream& os, const SparseVector& v) { return os << v.repr(); }
};

} // namespace

