#ifndef SEMIRING_HH
#define SEMIRING_HH


template<typename T>
class ViterbiSemiring {
  public:
    T one = 1.0;
    T null = 0.0;

    T add(T x, T y);
    T multiply(T x, T y);
    T convert(T x);
};

template<typename T> T
ViterbiSemiring<T>::add(T x, T y)
{
  return max(x, y);
}

template<typename T> T
ViterbiSemiring<T>::multiply(T x, T y)
{
  return x * y;
}

template<typename T> T
ViterbiSemiring<T>::convert(T x)
{
  return (T)x;
}


#endif

