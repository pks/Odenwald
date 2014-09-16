#pragma once


namespace Semiring {

template<typename T>
struct Viterbi {
  T one = 1.0;
  T null = 0.0;

  T add(T x, T y);
  T multiply(T x, T y);
  T convert(T x);
};

template<typename T> T
Viterbi<T>::add(T x, T y)
{
  return max(x, y);
}

template<typename T> T
Viterbi<T>::multiply(T x, T y)
{
  return x * y;
}

template<typename T> T
Viterbi<T>::convert(T x)
{
  return (T)x;
}

} // namespace

