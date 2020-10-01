/*
 * Some data-classes used by the compiler. There doesn't appear to be
 * an easy way to make them follow object-oriented principles, and I
 * doubt that would be make the code significantly better.
 */

#include <map>
#include <string>
#include <vector>

struct function {
  std::string name;
  std::string returnType;
  std::vector<std::string> argumentTypes;
  std::vector<double> defaultArgumentValues;
  std::vector<std::string> argumentNames;
};

struct structure {
  std::string name;
  unsigned sizeInBytes = 0;
  std::vector<std::string> memberNames;
  std::map<std::string, std::string> memberTypes;
  std::map<std::string, unsigned> memberOffsetInBytes;
  std::map<std::string, double> defaultValuesOfMembers;
  std::map<std::string, unsigned>
      arraySize; // Should contain '1' for members that aren't arrays.
};

struct CompilationContext {
  int stackSize =
      4096; // The JavaScript (WebAssembly) Virtual Machine by default has only
            // 64KB of RAM, and you need to allocate it so that it grows
            // linearly (unlike on real machines, all the memory allocated to
            // some program running on the JavaScript Virtual Machine appears
            // continuous to that program).
  int globalVariablePointer = stackSize;
  std::map<std::string, std::string> variableTypes; // Integer32...
  std::map<std::string, int>
      globalVariables; // Where they are in the heap memory.
  std::map<std::string, int> localVariables; // Where they are on the stack.
  std::vector<function> functions;
  int stackSizeOfThisFunction = 0;
  int stackSizeOfThisScope = 0; // Variables declared inside while-loops...
  std::string currentFunctionName;
  std::vector<structure> structures;
  std::map<std::string, unsigned>
      structureSizes; // A bit redundant, but it can make the code significantly
                      // shorter sometimes.
};
