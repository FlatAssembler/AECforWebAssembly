#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

#pragma once

class TreeNode {
  enum Associativity { left, right };
  static std::vector<TreeNode>
  applyBinaryOperators(std::vector<TreeNode> input,
                       std::vector<std::string> operators,
                       Associativity associativity);

public:
  std::map<std::string, int> basicDataTypeSizes;
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
            4; // WebAssembly is 32-bit (pointers being 32 bits or 4 bytes
               // long), unless somebody switches to the 64-bit mode (which is
               // rarely done).
    lineNumber = columnNumber = 0;
  }
  TreeNode(std::string newText, int newLine, int newColumn) {
    TreeNode();
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
  std::string
  getLispExpression() { // Again, for debugging purposes (and maybe, some day, I
                        // will want to compile my language to Lisp).
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
  int interpretAsACompileTimeConstant() {
    if (std::regex_match(text, std::regex("^\\d+$")))
      return std::stoi(text);
    if (text == "+")
      return children[0].interpretAsACompileTimeConstant() +
             children[1].interpretAsACompileTimeConstant();
    if (text == "-")
      return children[0].interpretAsACompileTimeConstant() -
             children[1].interpretAsACompileTimeConstant();
    if (text == "*")
      return children[0].interpretAsACompileTimeConstant() *
             children[1].interpretAsACompileTimeConstant();
    if (text == "/")
      return children[0].interpretAsACompileTimeConstant() /
             children[1].interpretAsACompileTimeConstant();
    if (text == "?:")
      return children[0].interpretAsACompileTimeConstant()
                 ? children[1].interpretAsACompileTimeConstant()
                 : children[2].interpretAsACompileTimeConstant();
    if (text == "mod(")
      return children[0].interpretAsACompileTimeConstant() %
             children[1].interpretAsACompileTimeConstant();
    if (text == "<")
      return children[0].interpretAsACompileTimeConstant() <
             children[1].interpretAsACompileTimeConstant();
    if (text == ">")
      return children[0].interpretAsACompileTimeConstant() >
             children[1].interpretAsACompileTimeConstant();
    if (text == "=")
      return children[0].interpretAsACompileTimeConstant() ==
             children[1].interpretAsACompileTimeConstant();
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Interpreter error: \"" << text
              << "\" isn't a valid token in a compile-time integer constant."
              << std::endl;
    return 0;
  }
  static std::vector<TreeNode>
  parseVariableDeclaration(std::vector<TreeNode> input);
};
