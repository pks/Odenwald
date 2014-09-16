#include "sparse_vector.hh"

int
main(void)
{
  Sv::SparseVector<string, score_t> a;
  a.insert("1", 1);
  a.insert("2", 2);
  cout << "a:" << a << endl;

  Sv::SparseVector<string, score_t> b;
  b.insert("2", 2);
  cout << "b:" << b << endl;

  Sv::SparseVector<string, score_t> c = a + b;
  cout << "a+b:" << c << endl;

  a += b;
  cout << "a+=b:" << a << endl;

  a -= b;
  cout << "a-=b:" << a << endl;

  cout << "a*2:" << a*2 << endl;

  a *= 2;
  cout << "a*=2:" << a << endl;

  string s("\"a\"=2 \"b\"=3");
  Sv::SparseVector<string, score_t>* sv = new Sv::SparseVector<string, score_t>(s);
  cout << *sv << endl;
  cout << sv->dot(*sv) << endl;

  return 0;
}

