#pragma once

#include <unordered_map>
#include <vector>
#include <sstream>

typedef double score_t; // FIXME
typedef double weight_t;

using namespace std;


namespace Sv {

template<typename K, typename V>
struct SparseVector {
  unordered_map<K, V> m_;
  V zero = 0.0;

  void
  insert(K k, V v) { m_[k] = v; };

  weight_t 
  dot(SparseVector& other)
  {
  };

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
    v.m_.insert(m_.begin(), m_.end());
    v.m_.insert(other.m_.begin(), other.m_.end());
    for (auto it = v.m_.begin(); it != v.m_.end(); it++)
      v.m_[it->first] = this->at(it->first) + other.at(it->first);
    return v;
  };

  SparseVector&
  operator+=(const SparseVector& other)
  {
    for (auto it = other.m_.begin(); it != other.m_.end(); it++)
      m_[it->first] += it->second;
    return *this;
  };

  SparseVector
  operator-(const SparseVector& other) const
  {
    SparseVector<K,V> v;
    v.m_.insert(m_.begin(), m_.end());
    v.m_.insert(other.m_.begin(), other.m_.end());
    for (auto it = v.m_.begin(); it != v.m_.end(); it++)
      v.m_[it->first] = this->at(it->first) - other.at(it->first);
    return v;
  };

  SparseVector&
  operator-=(const SparseVector& other)
  {
    for (auto it = other.m_.begin(); it != other.m_.end(); it++)
      m_[it->first] -= it->second;
    return *this;
  };

  SparseVector
  operator*(V f) const
  {
    SparseVector<K,V> v;
    for (auto it = m_.begin(); it != m_.end(); it++)
      v.m_[it->first] = this->at(it->first) * f;
    return v;
  };

  SparseVector&
  operator*=(V f)
  {
    for (auto it = m_.begin(); it != m_.end(); it++)
      m_[it->first] *= f;
    return *this;
  };

  string
  repr() const
  {
    ostringstream os; 
    os << "SparseVector<{";
    for (auto it = m_.begin(); it != m_.end(); it ++) {
      os << "'" << it->first << "'=" << it->second;
      if (next(it) != m_.end())
        os << ", ";
    }
    os << "}>";
    return os.str();
  };

  friend ostream&
  operator<<(ostream& os, const SparseVector& v) { return os << v.repr(); }
};

} // namespace

