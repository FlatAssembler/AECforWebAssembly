/*
 * So, this is the main program, the driver which reads the source code
 * and lets the tokenizer, the parser and the compiler communicate.
 *
 * TODO: What if some part of the compiler (most likely tokenizer) wants
 * to read some file other than the main AEC program? Such a problem
 * isn't easily solvable with this design.
 */

#ifndef NDEBUG // So that CMAKE does not complain about it being redefined...
#define NDEBUG // If deleted, the tokenizer and parser will output verbose
               // output.
#endif
#include <string>
std::string compilation_target; // WASI (WebAssembly System Interface) or
                                // browser (default).
#include "TreeRootNode.cpp"
#include "compiler.cpp"
#include "parser.cpp"
#include "tests.cpp"
#include "tokenizer.cpp"
#include <chrono>
#include <ciso646> // Necessary for Microsoft C++ Compiler.
#include <exception>
#include <fstream>
#include <iostream>
#include <typeinfo>
using namespace std;

bool ends_with(
    const string &first,
    const string &
        second) { // https://stackoverflow.com/questions/67451951/how-to-use-pointers-to-figure-out-if-a-c-string-ends-with-another-c-string/67452057?noredirect=1#comment119223072_67452057
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
                               // Explorer 6 (or, maybe better say, Opera Mini,
                               // because it appears to be fast by disobeying
                               // the standards) is to JavaScript. I don't
                               // understand how it manages to compile itself,
                               // yet it miscompiles something every now and
                               // then in a 5'000-lines-of-code program.
        throw regex_error(error_complexity);
      cerr << "Please invoke this program as follows:\n"
           << argv[0] << " name_of_the_program.aec" << endl;
      return -1;
    }
  } catch (regex_error &error) {
    cerr <<
        R"(The C++ compiler this executable has been compiled
with doesn't appear to support JavaScript-style regular
expressions. Parts of this program that rely on them
will not work.)"
         << endl;
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
  cout << "Reading the file \"" << argv[1] << "\"..." << endl;
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
       << R"( milliseconds.
I have made a forum thread about how to speed up the tokenizer,
in case you are interested:
https://www.forum.hr/showthread.php?t=1243509
Parsing the program...)"
       << endl;
  if (tokenized.size() and
      tokenized[0].text == "#") { // If we are setting the compilation target
    if (tokenized.size() >= 2 and tokenized[1].text != "target") {
      cerr << "Preprocessor error: Line " << tokenized[1].lineNumber
           << ", Column " << tokenized[1].columnNumber
           << ": Unrecognized preprocessor directive \"" << tokenized[1].text
           << "\"!" << endl;
      exit(1);
    } else if (tokenized.size() < 2) {
      cerr << "Preprocessor error: Line " << tokenized[0].lineNumber
           << ", Column " << tokenized[0].columnNumber
           << ": The preprocessor directive seems to be missing!" << endl;
      exit(1);
    }
    if (tokenized.size() >= 3 and tokenized[2].text != "browser" and
        tokenized[2].text != "WASI") {
      cerr << "Preprocessor error: Line " << tokenized[2].lineNumber
           << ", Column " << tokenized[2].columnNumber
           << ": Unrecognized target name \"" << tokenized[2].text
           << "\". Supported targets are \"browser\" and \"WASI\"." << endl;
      exit(1);
    } else if (tokenized.size() < 3) {
      cerr << "Preprocessor error: Line " << tokenized[1].lineNumber
           << ", Column " << tokenized[1].columnNumber
           << ": The compilation target seems not to be specified!" << endl;
      exit(1);
    }
    compilation_target = tokenized[2].text;
    tokenized.erase(tokenized.begin(), tokenized.begin() + 3);
  } else
    compilation_target = "browser";
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
  string assembly;
  try {
    assembly = AST.compile();
  } catch (exception &error) {
    cerr << "Internal compiler error: Uncaught exception in the compiler: "
         << typeid(error).name() << ": " << error.what() << std::endl;
    cerr << R"(If you have time, please report this to me on GitHub as an issue:
https://github.com/FlatAssembler/AECforWebAssembly/issues)"
         << std::endl;
    return 1;
  }
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
