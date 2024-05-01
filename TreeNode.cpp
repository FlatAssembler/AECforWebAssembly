/*
 * "TreeNode" is supposed to basically be a composite, representing the
 * tree structure (abstract syntax tree, AST) in such a way that other
 * code doesn't have to deal with whether it's interacting with a leaf
 * or some other node in a tree. It isn't a true composite because almost
 * none of the operations that make sense for other nodes make sense for
 * the root node.
 */

#include "AssemblyCode.cpp"
#include "bitManipulations.cpp"
#include "compilingContext.cpp"
#include "stringManipulationHelperFunctions.cpp"
#include <ciso646> // Necessary for Microsoft C++ Compiler.
#include <cmath>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

#pragma once

class TreeNode {
  enum class Associativity { left, right };
  static std::vector<TreeNode>
  applyBinaryOperators(std::vector<TreeNode> input,
                       std::vector<std::string> operators,
                       Associativity associativity);

protected:
  std::set<std::string> getStringsInSubnodes() const {
    auto setToBeReturned = std::set<std::string>();
    if (text == "asm(" or text == "asm_i32(" or text == "asm_i64(" or
        text == "asm_f32(" or
        text == "asm_f64(") // Inline assembly isn't a string that can be
                            // manipulated, and storing it in memory wastes
                            // memory (potentially a lot of it).
      return std::set<std::string>(); // That's why we will return an empty set,
                                      // as if we had no strings in our subnodes
                                      // (even though we have at least one).
    if (text.size() and text[0] == '"') {
      setToBeReturned.insert(text);
      return setToBeReturned;
    }
    for (auto child : children) {
      auto stringsInChild = child.getStringsInSubnodes();
      setToBeReturned.insert(stringsInChild.begin(), stringsInChild.end());
    }
    return setToBeReturned;
  }

public:
  // HappySkeptic suggested me to try to make shared parts of TreeNode static to
  // save on stack memory:
  // https://atheistforums.org/thread-63150-post-2053689.html#pid2053689
  static std::map<std::string, int> basicDataTypeSizes;
  static std::map<std::string, AssemblyCode::AssemblyType>
      mappingOfAECTypesToWebAssemblyTypes;
  static std::map<AssemblyCode::AssemblyType, std::string>
      stringRepresentationOfWebAssemblyType;
  static std::set<std::string> AECkeywords;
  static bool staticPropertiesInitialized;
  std::vector<TreeNode> children;
  std::string text;
  int lineNumber, columnNumber;
  TreeNode() {
    if (!staticPropertiesInitialized) { // https://atheistforums.org/thread-63150-post-2055509.html#pid2055509
      basicDataTypeSizes["Integer32"] = 4;
      basicDataTypeSizes["Character"] = 1;
      basicDataTypeSizes["Decimal32"] = 4;
      basicDataTypeSizes["Integer64"] = 8;
      basicDataTypeSizes["Decimal64"] = 8;
      basicDataTypeSizes["Integer16"] = 2;
      for (auto iterator = basicDataTypeSizes.begin();
           iterator != basicDataTypeSizes.end(); iterator++)
        if (iterator->first.find("Pointer") == std::string::npos)
          basicDataTypeSizes[iterator->first + "Pointer"] =
              4; // JavaScript (WebAssembly) virtual machine is 32-bit (pointers
                 // being 32 bits or 4 bytes long), unless somebody switches to
                 // the 64-bit mode (which is almost never done).
      basicDataTypeSizes["Nothing"] = 0;
      mappingOfAECTypesToWebAssemblyTypes =
          std::map<std::string, AssemblyCode::AssemblyType>(
              {{"Integer32", AssemblyCode::AssemblyType::i32},
               {"Integer64", AssemblyCode::AssemblyType::i64},
               {"Decimal32", AssemblyCode::AssemblyType::f32},
               {"Decimal64", AssemblyCode::AssemblyType::f64},
               {"Nothing", AssemblyCode::AssemblyType::null}});
      for (auto pair : basicDataTypeSizes)
        if (!mappingOfAECTypesToWebAssemblyTypes.count(pair.first))
          mappingOfAECTypesToWebAssemblyTypes[pair.first] =
              AssemblyCode::AssemblyType::i32;
      AECkeywords = std::set<std::string>({"Function",  "Which",
                                           "Returns",   "Nothing",
                                           "Is",        "External",
                                           "Does",      "EndFunction",
                                           "If",        "Then",
                                           "ElseIf",    "Else",
                                           "EndIf",     "While",
                                           "Loop",      "EndWhile",
                                           "Structure", "Consists",
                                           "Of",        "EndStructure",
                                           "Character", "CharacterPointer",
                                           "Integer16", "Integer16Pointer",
                                           "Integer32", "Integer32Pointer",
                                           "Integer64", "Integer64Pointer",
                                           "Decimal32", "Decimal32Pointer",
                                           "Decimal64", "Decimal64Pointer"});
      stringRepresentationOfWebAssemblyType =
          std::map<AssemblyCode::AssemblyType, std::string>(
              {{AssemblyCode::AssemblyType::i32, "i32"},
               {AssemblyCode::AssemblyType::i64, "i64"},
               {AssemblyCode::AssemblyType::f32, "f32"},
               {AssemblyCode::AssemblyType::f64, "f64"},
               {AssemblyCode::AssemblyType::null, "null"}});
      staticPropertiesInitialized = true;
    }
    lineNumber = columnNumber = 0;
  }
  TreeNode(std::string newText, int newLine, int newColumn) {
    *this = TreeNode(); // For some weird reason, just "TreeNode()" won't do the
                        // trick.
    text = newText;
    lineNumber = newLine;
    columnNumber = newColumn;
  }
  static std::vector<TreeNode>
  tokenize(std::string input); // See "tokenizer.cpp" for the implementation.
  static std::string JSONifyArrayOfTokens(
      std::vector<TreeNode> tokenizedString) { // For debugging the tokenizer.
    std::string ret = "[";
    if (!tokenizedString.size())
      return "[]";
    for (unsigned int i = 0; i < tokenizedString.size(); i++)
      if (i != tokenizedString.size() - 1)
        ret += "'" + tokenizedString[i].text + "',";
      else
        ret += "'" + tokenizedString[i].text + "']";
    return ret;
  }
  static std::vector<TreeNode> parse(
      std::vector<TreeNode> input); // See "parser.cpp" for the implementation.
  static std::vector<TreeNode> parseExpression(
      std::vector<TreeNode> input); // Made public for debugging purposes.
  std::string getLispExpression()
      const { // Again, for debugging purposes (and maybe, some day, I will want
              // to compile my language to Lisp).
    if (children.size() == 0)
      return text;
    std::string LispExpression = "(" +
                                 ((text.back() == '(' or text.back() == '[')
                                      ? (text.substr(0, text.size() - 1))
                                      : (text)) +
                                 " ";
    for (unsigned int i = 0; i < children.size(); i++)
      if (i == children.size() - 1)
        LispExpression += children[i].getLispExpression() + ")";
      else
        LispExpression += children[i].getLispExpression() + " ";
    return LispExpression;
  }
  virtual int interpretAsACompileTimeIntegerConstant() const {
    if ((text == "<" or text == ">" or text == "<=" or text == ">=") and
        children.at(0).text == text) { // Chained comparisons (`a < b < c`).
      TreeNode andNode("and", lineNumber, columnNumber),
          secondChild(text, lineNumber, columnNumber);
      secondChild.children = {children.at(0).children.at(1), children.at(1)};
      andNode.children = {children.at(0), secondChild};
      return andNode.interpretAsACompileTimeIntegerConstant();
    }
    if (isInteger(text))
      return std::stoi(text, 0, 0);
    if (text == "+" and children.size() == 2)
      return children[0].interpretAsACompileTimeIntegerConstant() +
             children[1].interpretAsACompileTimeIntegerConstant();
    if (text == "-" and children.size() == 2)
      return children[0].interpretAsACompileTimeIntegerConstant() -
             children[1].interpretAsACompileTimeIntegerConstant();
    if (text == "*" and children.size() == 2)
      return children[0].interpretAsACompileTimeIntegerConstant() *
             children[1].interpretAsACompileTimeIntegerConstant();
    if (text == "/" and children.size() == 2) {
      if (children[1].interpretAsACompileTimeIntegerConstant() == 0) {
        std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                  << ", Interpreter error: Division by zero in a compile-time "
                     "integer constant!"
                  << std::endl;
        std::exit(1);
      }
      return children[0].interpretAsACompileTimeIntegerConstant() /
             children[1].interpretAsACompileTimeIntegerConstant();
    }
    if (text == "and" and children.size() == 2)
      return children[0].interpretAsACompileTimeIntegerConstant() bitand
             children[1].interpretAsACompileTimeIntegerConstant();
    if (text == "or" and children.size() == 2)
      return children[0].interpretAsACompileTimeIntegerConstant() bitor
             children[1].interpretAsACompileTimeIntegerConstant();
    if (text == "?:" and children.size() == 3)
      return children[0].interpretAsACompileTimeIntegerConstant()
                 ? children[1].interpretAsACompileTimeIntegerConstant()
                 : children[2].interpretAsACompileTimeIntegerConstant();
    if (text == "mod(" and children.size() == 2)
      return children[0].interpretAsACompileTimeIntegerConstant() %
             children[1].interpretAsACompileTimeIntegerConstant();
    if (text == "<" and children.size() == 2)
      return children[0].interpretAsACompileTimeIntegerConstant() <
             children[1].interpretAsACompileTimeIntegerConstant();
    if (text == ">" and children.size() == 2)
      return children[0].interpretAsACompileTimeIntegerConstant() >
             children[1].interpretAsACompileTimeIntegerConstant();
    if (text == "=" and children.size() == 2)
      return children[0].interpretAsACompileTimeIntegerConstant() ==
             children[1].interpretAsACompileTimeIntegerConstant();
    if (text == "<=" and children.size() == 2)
      return children[0].interpretAsACompileTimeIntegerConstant() <=
             children[1].interpretAsACompileTimeIntegerConstant();
    if (text == ">=" and children.size() == 2)
      return children[0].interpretAsACompileTimeIntegerConstant() >=
             children[1].interpretAsACompileTimeIntegerConstant();
    if (text == "not(" and children.size() == 1)
      return not(children[0].interpretAsACompileTimeIntegerConstant());
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Interpreter error: \"" << text
              << "\" isn't a valid token in a compile-time integer constant."
              << std::endl;
    return 0;
  }
  virtual double interpretAsACompileTimeDecimalConstant() const {
    if ((text == "<" or text == ">" or text == "<=" or text == ">=") and
        children.at(0).text == text) { // Chained comparisons (`a < b < c`).
      TreeNode andNode("and", lineNumber, columnNumber),
          secondChild(text, lineNumber, columnNumber);
      secondChild.children = {children.at(0).children.at(1), children.at(1)};
      andNode.children = {children.at(0), secondChild};
      return andNode.interpretAsACompileTimeDecimalConstant();
    }
    if (isInteger(text))
      return std::stoi(text, 0, 0);
    if (isDecimalNumber(text))
      return std::stod(text);
    if (text == "+" and children.size() == 2)
      return children[0].interpretAsACompileTimeDecimalConstant() +
             children[1].interpretAsACompileTimeDecimalConstant();
    if (text == "-" and children.size() == 2)
      return children[0].interpretAsACompileTimeDecimalConstant() -
             children[1].interpretAsACompileTimeDecimalConstant();
    if (text == "*" and children.size() == 2)
      return children[0].interpretAsACompileTimeDecimalConstant() *
             children[1].interpretAsACompileTimeDecimalConstant();
    if (text == "/" and children.size() == 2)
      return children[0]
                 .interpretAsACompileTimeDecimalConstant() / // Dividing a real
                                                             // number by zero
                                                             // in C++ won't
                                                             // crash a program.
                                                             // I don't know
                                                             // what the
                                                             // standard says,
                                                             // but, in my
                                                             // experience, it
                                                             // results in
                                                             // putting some
                                                             // special value
                                                             // (NaN or inf)
                                                             // into the result.
             children[1].interpretAsACompileTimeDecimalConstant();
    if (text == "and" and children.size() == 2)
      return children[0].interpretAsACompileTimeDecimalConstant() and
             children[1].interpretAsACompileTimeDecimalConstant();
    if (text == "or" and children.size() == 2)
      return children[0].interpretAsACompileTimeDecimalConstant() or
             children[1].interpretAsACompileTimeDecimalConstant();
    if (text == "?:" and children.size() == 3)
      return children[0].interpretAsACompileTimeDecimalConstant()
                 ? children[1].interpretAsACompileTimeDecimalConstant()
                 : children[2].interpretAsACompileTimeDecimalConstant();
    if (text == "mod(" and
        children.size() ==
            2) // Let's give the users of the language the access to standard
               // library functions when writing compile-time decimal constants.
      return fmod(children[0].interpretAsACompileTimeDecimalConstant(),
                  children[1].interpretAsACompileTimeDecimalConstant());
    if (text == "sin(" and children.size() == 1)
      return sin(children[0].interpretAsACompileTimeDecimalConstant());
    if (text == "cos(" and children.size() == 1)
      return cos(children[0].interpretAsACompileTimeDecimalConstant());
    if (text == "tan(" and children.size() == 1)
      return tan(children[0].interpretAsACompileTimeDecimalConstant());
    if (text == "sqrt(" and children.size() == 1)
      return sqrt(children[0].interpretAsACompileTimeDecimalConstant());
    if (text == "atan(" and children.size() == 1)
      return atan(children[0].interpretAsACompileTimeDecimalConstant());
    if (text == "atan2(" and children.size() == 2)
      return atan2(children[0].interpretAsACompileTimeDecimalConstant(),
                   children[1].interpretAsACompileTimeDecimalConstant());
    if (text == "asin(" and children.size() == 1)
      return asin(children[0].interpretAsACompileTimeDecimalConstant());
    if (text == "acos(" and children.size() == 1)
      return acos(children[0].interpretAsACompileTimeDecimalConstant());
    if (text == "log(" and children.size() == 1)
      return log(children[0].interpretAsACompileTimeDecimalConstant());
    if (text == "log2(" and children.size() == 1)
      return log2(children[0].interpretAsACompileTimeDecimalConstant());
    if (text == "log10(" and children.size() == 1)
      return log10(children[0].interpretAsACompileTimeDecimalConstant());
    if (text == "exp(" and children.size() == 1)
      return exp(children[0].interpretAsACompileTimeDecimalConstant());
    if (text == "pow(" and children.size() == 2)
      return pow(children[0].interpretAsACompileTimeDecimalConstant(),
                 children[1].interpretAsACompileTimeDecimalConstant());
    if (text == "pi")
#ifndef M_PI // Microsoft C++ Compiler (I haven't tested this code in it,
             // but I suppose it will reject my code without this).
#define M_PI (atan(1) * 4)
#endif
      return M_PI;
    if (text == "e")
      return exp(1.); // That seems to be the only way to get 'e' on
                      // GCC 7 on Solaris 11.
    if (text == "<" and children.size() == 2)
      return children[0].interpretAsACompileTimeDecimalConstant() <
             children[1].interpretAsACompileTimeDecimalConstant();
    if (text == ">" and children.size() == 2)
      return children[0].interpretAsACompileTimeDecimalConstant() >
             children[1].interpretAsACompileTimeDecimalConstant();
    if (text == "=" and children.size() == 2)
      return children[0].interpretAsACompileTimeDecimalConstant() ==
             children[1].interpretAsACompileTimeDecimalConstant();
    if (text == "<=" and children.size() == 2)
      return children[0].interpretAsACompileTimeDecimalConstant() <=
             children[1].interpretAsACompileTimeDecimalConstant();
    if (text == ">=" and children.size() == 2)
      return children[0].interpretAsACompileTimeDecimalConstant() >=
             children[1].interpretAsACompileTimeDecimalConstant();
    std::cerr
        << "Line " << lineNumber << ", Column " << columnNumber
        << ", Interpreter error: \"" << text
        << "\" isn't a valid token in a compile-time decimal-number constant."
        << std::endl;
    return 0;
  }
  static std::vector<TreeNode>
  parseVariableDeclaration(std::vector<TreeNode> input);
  virtual AssemblyCode compile(CompilationContext context) const;
  virtual AssemblyCode compileAPointer(const CompilationContext &context) const;
  virtual std::string
  getType(const CompilationContext &context) const; // Integer32...
  virtual ~TreeNode() =
      default; // https://discord.com/channels/172018499005317120/172018499005317120/809830734256406569
};

std::string convertInlineAssemblyToAssembly(TreeNode inlineAssemblyNode) {
  std::string inlineAssembly = inlineAssemblyNode.text;
  if (inlineAssembly.front() != '"' or inlineAssembly.back() != '"') {
    std::cerr << "Line " << inlineAssemblyNode.lineNumber << ", Column "
              << inlineAssemblyNode.columnNumber
              << ", Compiler error: Inline assembly doesn't appear to be a "
                 "string. Aborting the compilation (or we will produce "
                 "syntactically incorrect assembly)."
              << std::endl;
    std::exit(1);
  }
  inlineAssembly = ";;Inline assembly begins.\n" +
                   inlineAssembly.substr(1, inlineAssembly.size() - 2) +
                   "\n;;Inline assembly ends.";
  // CLANG 10 (but not GCC 9.3.0) appears to miscompile "regex_replace" on
  // Oracle Linux 7.
  std::string temporaryString;
  for (unsigned int i = 0; i < inlineAssembly.size(); i++)
    if (inlineAssembly.substr(i, 2) == R"(\\)") {
      temporaryString += '\\';
      i++;
    } else if (inlineAssembly.substr(i, 2) == R"(\n)") {
      temporaryString += '\n';
      i++;
    } else if (inlineAssembly.substr(i, 2) == R"(\t)") {
      temporaryString += '\t';
      i++;
    } else if (inlineAssembly.substr(i, 2) == R"(\")") {
      temporaryString += '"';
      i++;
    } else
      temporaryString += inlineAssembly[i];
  inlineAssembly = temporaryString;
  return inlineAssembly;
}

std::map<std::string, int> TreeNode::basicDataTypeSizes;
std::map<std::string, AssemblyCode::AssemblyType>
    TreeNode::mappingOfAECTypesToWebAssemblyTypes;
std::map<AssemblyCode::AssemblyType, std::string>
    TreeNode::stringRepresentationOfWebAssemblyType;
std::set<std::string> TreeNode::AECkeywords;
bool TreeNode::staticPropertiesInitialized = false;
