#define NDEBUG
#include "TreeRootNode.cpp"
#include "compiler.cpp"
#include "parser.cpp"
#include "tests.cpp"
#include "tokenizer.cpp"
#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
  using namespace std::regex_constants;
  try {
    if (argc < 2 or
        !regex_match(argv[1], regex("^\\w*\\.AEC$", ECMAScript | icase))) {
      cerr << "Please invoke this program as follows:\n"
           << argv[0] << " name_of_the_program.aec" << endl;
      return -1;
    }
  } catch (regex_error &e) {
    cerr << "Your C++ compiler doesn't appear to support regular expressions, "
            "that this program makes heavy use of. Quitting now!"
         << endl;
    exit(-1);
  }
  cout << "Running the tests..." << endl;
  runTests();
  cout << "All the tests passed." << endl;
  ifstream input(argv[1]);
  if (!input) {
    cerr << "Can't open the file \"" << argv[1] << "\" for reading!" << endl;
    return -1;
  }
  cout << "Reading the file..." << endl;
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
  cout << "All characters read!" << endl;
  cout << "Tokenizing the program..." << endl;
  vector<TreeNode> tokenized = TreeNode::tokenize(rawInput);
  cout << "Finished tokenizing the program!\nParsing the program..." << endl;
  vector<TreeNode> parsed = TreeNode::parse(tokenized);
  cout << "Finished parsing the program!" << endl;
  TreeRootNode AST = TreeRootNode();
  AST.children = parsed;
  cout << "Compiling the program..." << endl;
  string assembly = AST.compile();
  cout << "Compilation finished!" << endl;
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
  assemblyFile << assembly << endl;
  assemblyFile.close();
  cout << "Assembly successfully saved, quitting now." << endl;
  return 0;
}
