#include "ackerman.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

Allocator alloc;

int main(int argc, char ** argv) {

  // input parameters (basic block size, memory length)

  srand(std::time(nullptr));

  int b = 128;
	int M = 512*1024;

  for (int i=1; i<argc; i+=2) {
		if (strncmp(argv[i], "-b", 2) == 0) {
			b = stoi(argv[i+1]);
		} else if (strncmp(argv[i], "-s", 2) == 0) {
			M = stoi(argv[i+1]);
		} else if (strncmp(argv[i], "-h", 2) == 0) {
			cout << "memtest [-b <blocksize>] [-s <memsize>]\n"
           << "-b <blocksize> defines the block size, in bytes. Default is 128 bytes.\n"
           << "-s <memsize> defines the size of the memory to be allocated, in bytes. Default is 512kB.\n"
           << endl;
      return 0;
		}
	}

  cout << "b: " << b << " m: " << M << endl;

  alloc.init_allocator(b, M);

  ackerman_main();

  // release_allocator()
  // called in the destructor
}
