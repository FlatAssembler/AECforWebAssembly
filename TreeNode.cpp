#include "AssemblyCode.cpp"
#include "bitManipulations.cpp"
#include "compilingContext.cpp"
#include <cmath>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

#pragma once

class TreeNode {
  enum Associativity { left, right };
  static std::vector<TreeNode>
  applyBinaryOperators(std::vector<TreeNode> input,
                       std::vector<std::string> operators,
                       Associativity associativity);

protected:
  std::set<std::string> getStringsInSubnodes() const {
    auto setToBeReturned = std::set<std::string>();
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
  std::map<std::string, int> basicDataTypeSizes;
  std::map<std::string, AssemblyCode::AssemblyType>
      mappingOfAECTypesToWebAssemblyTypes;
  std::map<AssemblyCode::AssemblyType, std::string>
      stringRepresentationOfWebAssemblyType;
  std::set<std::string> AECkeywords;
  std::vector<TreeNode> children;
  std::string text;
  int lineNumber, columnNumber;
  TreeNode() {
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
  int interpretAsACompileTimeIntegerConstant() const {
    if (std::regex_match(text, std::regex("(^\\d+$)|(^0x(\\d|[a-f]|[A-F])+$)")))
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
    if (text == "/" and children.size() == 0) {
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
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Interpreter error: \"" << text
              << "\" isn't a valid token in a compile-time integer constant."
              << std::endl;
    return 0;
  }
  double interpretAsACompileTimeDecimalConstant() const {
    if (std::regex_match(text, std::regex("(^\\d+$)|(^0x(\\d|[a-f]|[A-F])+$)")))
      return std::stoi(text, 0, 0);
    if (std::regex_match(text, std::regex("\\d+\\.\\d*")))
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
      return M_PI;
    if (text == "e")
      return exp(1);
    if (text == "<" and children.size() == 2)
      return children[0].interpretAsACompileTimeDecimalConstant() <
             children[1].interpretAsACompileTimeDecimalConstant();
    if (text == ">" and children.size() == 2)
      return children[0].interpretAsACompileTimeDecimalConstant() >
             children[1].interpretAsACompileTimeDecimalConstant();
    if (text == "=" and children.size() == 2)
      return children[0].interpretAsACompileTimeDecimalConstant() ==
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
  AssemblyCode compile(CompilationContext context) const;
  AssemblyCode compileAPointer(CompilationContext context) const;
  std::string getType(CompilationContext context) const; // Integer32...
};
