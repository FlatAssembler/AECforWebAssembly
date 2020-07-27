#define NDEBUG
#include "compiler.cpp"
#include "parser.cpp"
#include "tests.cpp"
#include "tokenizer.cpp"
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
  cout << "Running the tests..." << endl;
  runTests();
  cout << "All the tests passed." << endl;
  return 0;
}
