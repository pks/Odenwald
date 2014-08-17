#include <iostream>
#include <fstream>
#include <msgpack.hpp>

using namespace std;


int
main(int argc, char** argv)
{
  ifstream ifs(argv[1]);
  msgpack::unpacker pac;
  while(true) {
    pac.reserve_buffer(32*1024);
    size_t bytes = ifs.readsome(pac.buffer(), pac.buffer_capacity());
    pac.buffer_consumed(bytes);
    msgpack::unpacked result;
    while(pac.next(&result)) {
      msgpack::object o = result.get();
      cout << o << endl;
    }
    if (!bytes) break;
  }

  return 0;
}

