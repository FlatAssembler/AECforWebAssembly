/*
 * Some data-classes used by the compiler. There doesn't appear to be
 * an easy way to make them follow object-oriented principles, and I
 * doubt that would be make the code significantly better.
 * I will, before anything else, aim at making the code easy to debug.
 * Outputting JSON-es seems, in my experience, to be a good way to do that.
 */

#include <ciso646> // Necessary for Microsoft C++ Compiler.
#include <map>
#include <string>
#include <vector>

#pragma once

std::string JSONifyString(const std::string &str) {
  std::string ret = "\"";
  for (size_t i = 0; i < str.length(); i++)
    if (str[i] == '\n')
      ret += "\\n";
    else if (str[i] == '\"')
      ret += "\\\"";
    else if (str[i] == '\'')
      ret += "\\'";
    else if (str[i] == '\t')
      ret += "\\t";
    else if (str[i] == '\\')
      ret += "\\\\";
    else
      ret += str[i];
  ret += "\"";
  return ret;
}

std::string JSONifyVectorOfStrings(const std::vector<std::string> &vector) {
  if (vector.empty())
    return "[]";
  std::string ret = "[";
  for (size_t i = 0; i < vector.size(); i++)
    if (i == vector.size() - 1)
      ret += JSONifyString(vector[i]) + "]";
    else
      ret += JSONifyString(vector[i]) + ",";
  return ret;
}

std::string JSONifyVectorOfDoubles(const std::vector<double> &vector) {
  if (vector.empty())
    return "[]";
  std::string ret = "[";
  for (size_t i = 0; i < vector.size(); i++)
    if (i == vector.size() - 1)
      ret += std::to_string(vector[i]) + "]";
    else
      ret += std::to_string(vector[i]) + ",";
  return ret;
}

struct function {
  std::string name;
  std::string returnType;
  std::vector<std::string> argumentTypes;
  std::vector<double> defaultArgumentValues;
  std::vector<std::string> argumentNames;
  std::string JSONify() const {
    return "{\n\"name\":" + JSONifyString(name) +
           ",\n\"returnType\":" + JSONifyString(returnType) +
           ",\n\"argumentTypes\":" + JSONifyVectorOfStrings(argumentTypes) +
           ",\n\"defaultArgumentValues\":" +
           JSONifyVectorOfDoubles(defaultArgumentValues) +
           ",\n\"argumentNames\":" + JSONifyVectorOfStrings(argumentNames) +
           "\n}";
  }
};

std::string JSONifyMapOfStrings(const std::map<std::string, std::string> &map) {
  std::vector<std::pair<std::string, std::string>> vector(map.size());
  std::copy(map.begin(), map.end(), vector.begin());
  std::string ret = "[\n";
  for (size_t i = 0; i < vector.size(); i++) {
    ret += "{\n\"first\":" + JSONifyString(vector[i].first) +
           ",\n\"second\":" + JSONifyString(vector[i].second) + "\n}";
    if (i != vector.size() - 1)
      ret += ",\n";
    else
      ret += "\n";
  }
  ret += "]";
  return ret;
}

std::string JSONifyMapOfUnsigneds(const std::map<std::string, unsigned> &map) {
  std::vector<std::pair<std::string, unsigned>> vector(map.size());
  std::copy(map.begin(), map.end(), vector.begin());
  std::string ret = "[\n";
  for (size_t i = 0; i < vector.size(); i++) {
    ret += "{\n\"first\":" + JSONifyString(vector[i].first) +
           ",\n\"second\":" + std::to_string(vector[i].second) + "\n}";
    if (i != vector.size() - 1)
      ret += ",\n";
    else
      ret += "\n";
  }
  ret += "]";
  return ret;
}

std::string JSONifyMapOfDoubles(const std::map<std::string, double> &map) {
  std::vector<std::pair<std::string, double>> vector(map.size());
  std::copy(map.begin(), map.end(), vector.begin());
  std::string ret = "[\n";
  for (size_t i = 0; i < vector.size(); i++) {
    ret += "{\n\"first\":" + JSONifyString(vector[i].first) +
           ",\n\"second\":" + std::to_string(vector[i].second) + "\n}";
    if (i != vector.size() - 1)
      ret += ",\n";
    else
      ret += "\n";
  }
  ret += "]";
  return ret;
}

struct structure {
  std::string name;
  unsigned sizeInBytes = 0;
  std::vector<std::string> memberNames;
  std::map<std::string, std::string> memberTypes;
  std::map<std::string, unsigned> memberOffsetInBytes;
  std::map<std::string, double> defaultValuesOfMembers;
  std::map<std::string, unsigned>
      arraySize; // Should contain '1' for members that aren't arrays.
  std::string JSONify() const {
    return "{\n\"name\":" + JSONifyString(name) +
           ",\n\"sizeInBytes\":" + std::to_string(sizeInBytes) +
           ",\n\"memberNames\":" + JSONifyVectorOfStrings(memberNames) +
           ",\n\"memberTypes\":" + JSONifyMapOfStrings(memberTypes) +
           ",\n\"memberOffsetInBytes\":" +
           JSONifyMapOfUnsigneds(memberOffsetInBytes) +
           ",\n\"defaultValuesOfMembers\":" +
           JSONifyMapOfDoubles(defaultValuesOfMembers) +
           ",\n\"arraySize\":" + JSONifyMapOfUnsigneds(arraySize) + "\n}";
  }
};

std::string JSONifyMapOfInts(const std::map<std::string, int> &map) {
  std::vector<std::pair<std::string, int>> vector(map.size());
  std::copy(map.begin(), map.end(), vector.begin());
  std::string ret = "[\n";
  for (size_t i = 0; i < vector.size(); i++) {
    ret += "{\n\"first\":" + JSONifyString(vector[i].first) +
           ",\n\"second\":" + std::to_string(vector[i].second) + "\n}";
    if (i != vector.size() - 1)
      ret += ",\n";
    else
      ret += "\n";
  }
  ret += "]";
  return ret;
}

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
      placesOfVariableDeclarations; // To provide more useful warnings about
                                    // variable shadowing (when a variable is
                                    // declared with the variable of the same
                                    // name already being in scope).
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
  std::string JSONify() const {
    std::string ret =
        "{\n\"stackSize\":" + std::to_string(stackSize) +
        ",\n\"globalVariablePointer\":" +
        std::to_string(globalVariablePointer) +
        ",\n\"variableTypes\":" + JSONifyMapOfStrings(variableTypes) +
        ",\n\"placesOfVariableDeclarations\":" +
        JSONifyMapOfInts(placesOfVariableDeclarations) +
        ",\n\"globalVariables\":" + JSONifyMapOfInts(globalVariables) +
        ",\n\"localVariables\":" + JSONifyMapOfInts(localVariables) +
        ",\n\"functions\":[\n";
    for (size_t i = 0; i < functions.size(); i++)
      if (i == functions.size() - 1)
        ret += functions[i].JSONify() + "\n";
      else
        ret += functions[i].JSONify() + ",\n";
    ret +=
        "],\n\"stackSizeOfThisFunction\":" +
        std::to_string(stackSizeOfThisFunction) +
        ",\n\"stackSizeOfThisScope\":" + std::to_string(stackSizeOfThisScope) +
        ",\n\"currentFunctionName\":" + JSONifyString(currentFunctionName) +
        ",\n\"structures\":[\n";
    for (size_t i = 0; i < structures.size(); i++)
      if (i == structures.size() - 1)
        ret += structures[i].JSONify() + "\n";
      else
        ret += structures[i].JSONify() + ",\n";
    ret += "],\n\"structureSizes\":" + JSONifyMapOfUnsigneds(structureSizes) +
           "\n}\n";
    return ret;
  }
};
