#define NDEBUG
#include "TreeRootNode.cpp"
#include "compiler.cpp"
#include "parser.cpp"
#include "tests.cpp"
#include "tokenizer.cpp"
#include <chrono>
#include <fstream>
#include <iostream>
using namespace std;

bool ends_with(string first, string second) {
  if (second.size() > first.size())
    return false;
  return first.substr(first.size() - second.size()) == second;
}

int main(int argc, char **argv) {
  using namespace std::regex_constants;
  using namespace std::chrono;
  try {
    if (argc < 2 or
        !regex_search(argv[1], regex(R"(\.AEC$)", ECMAScript | icase))) {
      if (argc >= 2 and
          (ends_with(argv[1], ".aec") or
           ends_with(argv[1],
                     ".AEC"))) // Damn, CLANG on Linux is to C++ what Internet
                               // Explorer 6 is to JavaScript. I don't understand
                               // how it manages to compile itself, yet it
                               // miscompiles something every now and then in a
                               // 4'000-lines-of-code program.
        throw regex_error(error_complexity);
      cerr << "Please invoke this program as follows:\n"
           << argv[0] << " name_of_the_program.aec" << endl;
      return -1;
    }
  } catch (regex_error &error) {
    cerr << "Your C++ compiler doesn't appear to support JavaScript-style "
            "regular expressions, "
            "that this program makes heavy use of. Quitting now!"
         << endl;
    exit(-1);
  }
  cout << "Running the tests..." << endl;
  auto beginningOfTests = chrono::high_resolution_clock::now();
  runTests();
  auto endOfTests = chrono::high_resolution_clock::now();
  cout << "All the tests passed in "
       << ((endOfTests - beginningOfTests).count() *
           high_resolution_clock::period::num * 1000 /
           high_resolution_clock::period::den)
       << " milliseconds." << endl;
  ifstream input(argv[1]);
  if (!input) {
    cerr << "Can't open the file \"" << argv[1] << "\" for reading!" << endl;
    return -1;
  }
  cout << "Reading the file..." << endl;
  auto beginningOfReading = chrono::high_resolution_clock::now();
  string rawInput;
  while (true) {
    char currentCharacter;
    input.get(currentCharacter);
    if (input.eof())
      break;
    if (currentCharacter != '\r')
      rawInput += currentCharacter; // Some text editors store new-lines as
                                    // "\r\n" instead of '\n'.
  }
  input.close();
  auto endOfReading = chrono::high_resolution_clock::now();
  cout << "All characters read in "
       << ((endOfReading - beginningOfReading).count() *
           high_resolution_clock::period::num * 1000 /
           high_resolution_clock::period::den)
       << " milliseconds." << endl;
  cout << "Tokenizing the program..." << endl;
  auto beginningOfTokenizing = chrono::high_resolution_clock::now();
  vector<TreeNode> tokenized = TreeNode::tokenize(rawInput);
  auto endOfTokenizing = chrono::high_resolution_clock::now();
  cout << "Finished tokenizing the program in "
       << ((endOfTokenizing - beginningOfTokenizing).count() *
           high_resolution_clock::period::num * 1000 /
           high_resolution_clock::period::den)
       << " milliseconds.\nParsing the program..." << endl;
  auto beginningOfParsing = chrono::high_resolution_clock::now();
  vector<TreeNode> parsed = TreeNode::parse(tokenized);
  auto endOfParsing = chrono::high_resolution_clock::now();
  cout << "Finished parsing the program in "
       << ((endOfParsing - beginningOfParsing).count() *
           high_resolution_clock::period::num * 1000 /
           high_resolution_clock::period::den)
       << " milliseconds." << endl;
  TreeRootNode AST = TreeRootNode();
  AST.children = parsed;
  cout << "Compiling the program..." << endl;
  auto beginningOfCompilation = chrono::high_resolution_clock::now();
  string assembly = AST.compile();
  auto endOfCompilation = chrono::high_resolution_clock::now();
  cout << "Compilation finished in "
       << ((endOfCompilation - beginningOfCompilation).count() *
           high_resolution_clock::period::num * 1000 /
           high_resolution_clock::period::den)
       << " milliseconds!" << endl;
  string assemblyFileName =
      string(argv[1]).substr(0,
                             string(argv[1]).size() - string(".aec").size()) +
      ".wat";
  cout << "Saving the assembly in file \"" << assemblyFileName << "\"..."
       << endl;
  ofstream assemblyFile(assemblyFileName.c_str());
  if (!assemblyFile) {
    cerr << "Can't open \"" << assemblyFileName << "\" for output!" << endl;
    return -1;
  }
  // CLANG 10 (but not GCC 9.3.0) appears to miscompile "regex_replace" on
  // Oracle Linux 7.
  string temporaryString;
  for (unsigned int i = 0; i < assembly.size(); i++)
    if (assembly[i] == '\t')
      temporaryString += "  ";
    else
      temporaryString += assembly[i];
  assembly = temporaryString;
  assemblyFile << assembly << endl;
  assemblyFile.close();
  cout << "Assembly successfully saved, quitting now." << endl;
  return 0;
}
