#include "TreeNode.cpp"
#include <algorithm>

std::vector<TreeNode>
TreeNode::applyBinaryOperators(std::vector<TreeNode> input,
                               std::vector<std::string> operators,
                               Associativity associativity) {
  using std::regex;
  using std::regex_match;
  for (int i = associativity == left ? 0 : int(input.size()) - 1;
       associativity == left ? i < int(input.size()) : i >= 0;
       i += associativity == left ? 1 : -1) {
    if (std::count(operators.begin(), operators.end(), input[i].text) and
        !input[i].children.size()) {
      if (!i or i == int(input.size()) - 1) {
        std::cerr << "Line " << input[i].lineNumber << ", Column "
                  << input[i].columnNumber
                  << ", Parser error: The binary operator \"" << input[i].text
                  << "\" has less than two operands." << std::endl;
        return input;
      }
      if (!regex_match(
              input[i - 1].text,
              regex("(^(\\d|_|[a-z]|[A-Z])*$)|(^(\\d|_|[a-z]|[A-Z])+\\.("
                    "\\d|_|[a-z]|[A-Z])*$)")) and
          !input[i - 1].children.size() and input[i - 1].text.back() != '(') {
        std::cerr << "Line " << input[i - 1].lineNumber << ", Column "
                  << input[i - 1].columnNumber
                  << ", Parser error: Unexpected token \"" << input[i - 1].text
                  << "\"." << std::endl;
      }
      if (!regex_match(
              input[i + 1].text,
              regex("(^(\\d|_|[a-z]|[A-Z])*$)|(^(\\d|_|[a-z]|[A-Z])+\\.("
                    "\\d|_|[a-z]|[A-Z])*$)")) and
          !input[i + 1].children.size() and input[i - 1].text.back() != '(') {
        std::cerr << "Line " << input[i + 1].lineNumber << ", Column "
                  << input[i + 1].columnNumber
                  << ", Parser error: Unexpected token \"" << input[i - 1].text
                  << "\"." << std::endl;
      }
      input[i].children.push_back(input[i - 1]);
      input[i].children.push_back(input[i + 1]);
      input.erase(input.begin() + i + 1);
      input.erase(input.begin() + i - 1);
      i += associativity == left ? -1 : 1;
    }
  }
  return input;
}

std::vector<TreeNode> TreeNode::parseExpression(std::vector<TreeNode> input) {
#ifndef NDEBUG
  std::cerr << "DEBUG: Beginning to parse the array of tokens: "
            << JSONifyArrayOfTokens(input) << std::endl;
#endif
  auto parsedExpression = input;
  for (unsigned int i = 0; i < parsedExpression.size(); i++)
    if (parsedExpression[i].text.back() == '(' and
        !parsedExpression[i].children.size()) {
      unsigned int firstParenthesis = i;
      unsigned int nextParenthesis = i + 1;
      unsigned int counterOfOpenParentheses = 1;
      while (counterOfOpenParentheses) {
        if (nextParenthesis >= parsedExpression.size()) {
          std::cerr << "Line " << parsedExpression[i].lineNumber << ", Column "
                    << parsedExpression[i].columnNumber
                    << ", Parser error: The parenthesis is not closed!"
                    << std::endl;
          break;
        }
        if (parsedExpression[nextParenthesis].text.back() == '(')
          counterOfOpenParentheses++;
        if (parsedExpression[nextParenthesis].text == ")")
          counterOfOpenParentheses--;
        nextParenthesis++;
      }
      std::vector<TreeNode> nodesThatTheRecursionDealsWith;
      for (unsigned int i = firstParenthesis + 1; i < nextParenthesis - 1;
           i++) // Don't include the parentheses in the expression passed to the
                // recursion, otherwise you will end up in an infinite loop.
        nodesThatTheRecursionDealsWith.push_back(parsedExpression[i]);
      nodesThatTheRecursionDealsWith =
          parseExpression(nodesThatTheRecursionDealsWith);
      if (nodesThatTheRecursionDealsWith.size() > 1 and
          parsedExpression[i].text == "(") {
        std::cerr << "Line " << nodesThatTheRecursionDealsWith[1].lineNumber
                  << ", Column "
                  << nodesThatTheRecursionDealsWith[1].columnNumber
                  << ", Parser error: Unexpected token \""
                  << nodesThatTheRecursionDealsWith[1].text << "\"."
                  << std::endl;
      }
      for (auto iterator = nodesThatTheRecursionDealsWith.begin();
           iterator < nodesThatTheRecursionDealsWith.end(); iterator++)
        if (iterator->text == ",") { // Multi-argument function
          iterator--;
          nodesThatTheRecursionDealsWith.erase(iterator + 1);
        }
      if (parsedExpression[i].text ==
          "(") { // If it's not a function, but only a parenthesis, delete it.
        parsedExpression.erase(parsedExpression.begin() + firstParenthesis,
                               parsedExpression.begin() + nextParenthesis);
        parsedExpression.insert(parsedExpression.begin() + i,
                                nodesThatTheRecursionDealsWith.begin(),
                                nodesThatTheRecursionDealsWith.end());
      } else { // If it's the name of a function, don't delete it.
        parsedExpression.erase(parsedExpression.begin() + firstParenthesis + 1,
                               parsedExpression.begin() + nextParenthesis);
        parsedExpression[i].children.insert(
            parsedExpression[i].children.begin(),
            nodesThatTheRecursionDealsWith.begin(),
            nodesThatTheRecursionDealsWith.end());
      }
    }
  for (unsigned int i = 0; i < parsedExpression.size(); i++)
    if (parsedExpression[i].text.back() == '[' and
        parsedExpression[i].children.size() == 0) // Array indices
    {
      unsigned int nameOfTheArray = i;
      unsigned int counterOfArrayNames = 1;
      unsigned int closedBracket = nameOfTheArray + 1;
      while (counterOfArrayNames) {
        if (closedBracket >= parsedExpression.size()) {
          std::cerr << "Line " << parsedExpression[i].lineNumber << ", Column "
                    << parsedExpression[i].columnNumber
                    << ", Parser error: The index of the array \""
                    << parsedExpression[i].text.substr(
                           0, parsedExpression[i].text.size() - 1)
                    << "\" is not closed by \"]\"." << std::endl;
          break;
        }
        if (parsedExpression[closedBracket].text == "]")
          counterOfArrayNames--;
        if (parsedExpression[closedBracket].text.back() == '[')
          counterOfArrayNames++;
        closedBracket++;
      }
      closedBracket--;
      std::vector<TreeNode> nodesThatTheRecursionDealsWith;
      for (unsigned int i = nameOfTheArray + 1; i < closedBracket;
           i++) // Again, it's important not to include brackets in the array
                // that's passed to the recursion.
        nodesThatTheRecursionDealsWith.push_back(parsedExpression[i]);
      nodesThatTheRecursionDealsWith =
          parseExpression(nodesThatTheRecursionDealsWith);
      if (nodesThatTheRecursionDealsWith.size() == 0) {
        std::cerr << "Line " << parsedExpression[i].lineNumber << ", Column "
                  << parsedExpression[i].columnNumber
                  << ", Parser error: Array index of the array \""
                  << parsedExpression[i].text.substr(
                         0, parsedExpression[i].text.size() -
                                1) // Don't print the '[' character at the end
                                   // of the array name (as the array name is
                                   // visible to the parser).
                  << "\" is empty!" << std::endl;
        break;
      }
      if (nodesThatTheRecursionDealsWith.size() > 1) {
        std::cerr << "Line " << nodesThatTheRecursionDealsWith[1].lineNumber
                  << ", Column "
                  << nodesThatTheRecursionDealsWith[1].columnNumber
                  << ", Parser error: Unexpected token \""
                  << nodesThatTheRecursionDealsWith[1].text << "\"!"
                  << std::endl;
      }
      parsedExpression[i].children.insert(
          parsedExpression[i].children.begin(),
          nodesThatTheRecursionDealsWith.begin(),
          nodesThatTheRecursionDealsWith.end());
      parsedExpression.erase(
          parsedExpression.begin() + nameOfTheArray +
              1, // It's important to exclude the name of the array from the
                 // portion about to be erased.
          parsedExpression.begin() + closedBracket +
              1 // And it's also important to include the closing bracket into
                // the portion about to be erased.
      );
    }
  for (unsigned int i = 0; i < parsedExpression.size(); i++)
    if (parsedExpression[i].text == "{") // Array initializer
    {
      unsigned int openCurlyBrace = i;
      unsigned int closedCurlyBrace = i + 1;
      unsigned int curlyBracesCounter = 1;
      while (curlyBracesCounter) {
        if (closedCurlyBrace >= parsedExpression.size()) {
          std::cerr << "Line " << parsedExpression[i].lineNumber << ", Column "
                    << parsedExpression[i].columnNumber
                    << ", Parser error: The curly brace \"{\" isn't closed!"
                    << std::endl;
          break;
        }
        if (parsedExpression[closedCurlyBrace].text == "}")
          curlyBracesCounter--;
        if (parsedExpression[closedCurlyBrace].text == "{")
          curlyBracesCounter++;
        closedCurlyBrace++;
      }
      closedCurlyBrace--;
      std::vector<TreeNode> nodesThatTheRecursionDealsWith(
          parsedExpression.begin() + openCurlyBrace + 1,
          parsedExpression.begin() +
              closedCurlyBrace); // We aren't in JavaScript, let's use the
                                 // features of C++, such as the iterator range
                                 // constructors...
      nodesThatTheRecursionDealsWith =
          parseExpression(nodesThatTheRecursionDealsWith);
      nodesThatTheRecursionDealsWith.erase(
          std::remove_if(nodesThatTheRecursionDealsWith.begin(),
                         nodesThatTheRecursionDealsWith.end(),
                         [](TreeNode node) { return node.text == ","; }),
          nodesThatTheRecursionDealsWith.end()); //...and lambda functions.
#ifndef NDEBUG
      std::cerr << "DEBUG: After std::remove_if, we are dealing with: "
                << JSONifyArrayOfTokens(nodesThatTheRecursionDealsWith)
                << std::endl;
#endif
      parsedExpression[openCurlyBrace].children.insert(
          parsedExpression[openCurlyBrace].children.begin(),
          nodesThatTheRecursionDealsWith.begin(),
          nodesThatTheRecursionDealsWith.end());
      parsedExpression.erase(parsedExpression.begin() + openCurlyBrace + 1,
                             parsedExpression.begin() + closedCurlyBrace + 1);
      parsedExpression[openCurlyBrace].text = "{}";
    }
  for (int i = parsedExpression.size() - 1; i >= 0;
       i--) // The unary "-" operator.
    if (parsedExpression[i].text == "-" and
        i != int(parsedExpression.size()) - 1 and
        parsedExpression[i].children.size() == 0 and
        (!i or
         (!std::regex_match(parsedExpression[i - 1].text,
                            std::regex("(^(\\d|_|[a-z]|[A-Z])*$)|(^(\\d|_|[a-z]"
                                       "|[A-Z])+\\.(\\d|_|[a-z]|[A-Z])*$)")) and
          parsedExpression[i - 1].text.back() != '(' and
          parsedExpression[i - 1].children.size() == 0))) {
      parsedExpression[i].children.push_back(
          TreeNode("0", parsedExpression[i].lineNumber,
                   parsedExpression[i].columnNumber));
      parsedExpression[i].children.push_back(parsedExpression[i + 1]);
      parsedExpression.erase(parsedExpression.begin() + i + 1);
    }
  std::vector<std::vector<std::string>> leftAssociativeBinaryOperators(
      {{"*", "/"}, {"-", "+"}, {"<", ">", "="}, {"and"}, {"or"}});
  parsedExpression =
      applyBinaryOperators(parsedExpression, {"."}, Associativity::right);
  for (unsigned int i = 0; i < leftAssociativeBinaryOperators.size(); i++)
    parsedExpression = applyBinaryOperators(parsedExpression,
                                            leftAssociativeBinaryOperators[i],
                                            Associativity::left);
  for (int i = parsedExpression.size() - 1; i >= 0;
       i--) // The ternary conditional "?:" operator (it's right-associative).
    if (parsedExpression[i].text == ":") {
      if (i == int(parsedExpression.size()) - 1) {
        std::cerr << "Line " << parsedExpression[i].lineNumber << ", Column "
                  << parsedExpression[i].columnNumber
                  << ", Parser error: The ternary operator \"?:\" has less "
                     "than three operands."
                  << std::endl;
        break;
      }
      int colon = i;
      int questionMark = i - 1;
      int colonCounter = 1;
      while (colonCounter) {
        if (questionMark < 0) {
          std::cerr << "Line " << parsedExpression[colon].lineNumber
                    << ", Column " << parsedExpression[colon].columnNumber
                    << ", Parser error: Unexpected token \":\"!" << std::endl;
          break;
        }
        if (parsedExpression[questionMark].text == "?")
          colonCounter--;
        if (parsedExpression[questionMark].text == ":")
          colonCounter++;
        questionMark--;
      }
      questionMark++;
      if (!questionMark) {
        std::cerr << "Line " << parsedExpression[questionMark].lineNumber
                  << ", Column " << parsedExpression[questionMark].columnNumber
                  << ", Parser error: The ternary operator \"?:\" has less "
                     "than three operands."
                  << std::endl;
        break;
      }
      std::vector<TreeNode> nodesThatTheRecursionDealsWith;
      for (int i = questionMark + 1; i < colon; i++)
        nodesThatTheRecursionDealsWith.push_back(parsedExpression[i]);
      nodesThatTheRecursionDealsWith =
          parseExpression(nodesThatTheRecursionDealsWith);
      if (nodesThatTheRecursionDealsWith.size() > 1) {
        std::cerr << "Line " << nodesThatTheRecursionDealsWith[1].lineNumber
                  << ", Column "
                  << nodesThatTheRecursionDealsWith[1].columnNumber
                  << ", Parser error: Unexpected token \""
                  << nodesThatTheRecursionDealsWith[1].text << "\"!"
                  << std::endl;
        break;
      }
      if (nodesThatTheRecursionDealsWith.size() == 0) {
        std::cerr << "Line " << parsedExpression[questionMark].lineNumber
                  << ", Column " << parsedExpression[questionMark].columnNumber
                  << ", Parser error: The ternary operator \"?:\" has less "
                     "than three operands!"
                  << std::endl;
        break;
      }
      parsedExpression[questionMark].text = "?:";
      parsedExpression[questionMark].children.push_back(
          parsedExpression[questionMark - 1]); // Condition
      parsedExpression[questionMark].children.push_back(
          nodesThatTheRecursionDealsWith[0]); // Then-clause
      parsedExpression[questionMark].children.push_back(
          parsedExpression[colon + 1]); // Else-clause
      parsedExpression.erase(parsedExpression.begin() + questionMark - 1);
      parsedExpression.erase(parsedExpression.begin() + questionMark,
                             parsedExpression.begin() + colon + 1);
      i = questionMark;
    }
  parsedExpression =
      applyBinaryOperators(parsedExpression, {":="}, Associativity::right);
#ifndef NDEBUG
  std::cerr << "DEBUG: Returning the array: "
            << JSONifyArrayOfTokens(parsedExpression) << std::endl;
#endif
  return parsedExpression;
}

std::vector<TreeNode> TreeNode::parse(std::vector<TreeNode> input) {
  throw std::string("Parser is not yet implemented!");
}
