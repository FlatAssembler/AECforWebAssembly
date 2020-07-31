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

std::vector<TreeNode>
TreeNode::parseVariableDeclaration(std::vector<TreeNode> input) {
  auto inputWithParenthesesParsed = parseExpression(input);
  if (input.size() < 2) {
    std::cerr << "Line " << inputWithParenthesesParsed[0].lineNumber
              << ", Column " << inputWithParenthesesParsed[0].columnNumber
              << ", Parser error: Unexpected token \""
              << inputWithParenthesesParsed[0].text << "\"!" << std::endl;
    return inputWithParenthesesParsed;
  }
  for (unsigned i = 0; i < inputWithParenthesesParsed.size(); i++)
    if (inputWithParenthesesParsed[i].text == ":=") // Initial assignments.
    {
      TreeNode nodeWithVariableName =
          inputWithParenthesesParsed[i]
              .children[0]; // Let's assume the parser has done a good job thus
                            // far.
      TreeNode NodeWithAssignment = inputWithParenthesesParsed[i].children[1];
      TreeNode temporaryNode =
          TreeNode(":=", inputWithParenthesesParsed[i].lineNumber,
                   inputWithParenthesesParsed[i].columnNumber);
      temporaryNode.children.push_back(NodeWithAssignment);
      nodeWithVariableName.children.push_back(temporaryNode);
      inputWithParenthesesParsed[i] = nodeWithVariableName;
    }
  inputWithParenthesesParsed.erase(
      std::remove_if(inputWithParenthesesParsed.begin(),
                     inputWithParenthesesParsed.end(),
                     [](TreeNode node) { return node.text == ","; }),
      inputWithParenthesesParsed.end());
  inputWithParenthesesParsed[0].children.insert(
      inputWithParenthesesParsed[0].children.begin(),
      inputWithParenthesesParsed.begin() + 1, inputWithParenthesesParsed.end());
  inputWithParenthesesParsed.erase(inputWithParenthesesParsed.begin() + 1,
                                   inputWithParenthesesParsed.end());
  return inputWithParenthesesParsed;
}

std::vector<TreeNode> TreeNode::parse(std::vector<TreeNode> input) {
  typedef std::vector<TreeNode> TreeNodes;
#ifndef NDEBUG // Well, constructors and all those object-oriented things in C++
               // are only useful if you know what you're doing. If you don't,
               // you are shooting yourself in the foot, like I did here.
  if (input.size()) {
    std::cerr << "DEBUG: Basic data type sizes are:\n";
    for (auto i = input[0].basicDataTypeSizes.begin();
         i != input[0].basicDataTypeSizes.end(); i++)
      std::cerr << "DEBUG: " << i->first << "\t" << i->second << '\n';
    std::flush(std::cerr);
  }
#endif
  for (unsigned i = 0; i < input.size(); i++)
    if (input[i].basicDataTypeSizes.count(input[i].text) and
        input[i].children.empty()) { // Declaration of a variable of a basic
                                     // type (Integer32...).
#ifndef NDEBUG
      std::cerr << "DEBUG: Parsing a variable declaration at line "
                << input[i].lineNumber << " of the type " << input[i].text
                << "." << std::endl;
#endif
      unsigned int typeName = i;
      unsigned int semicolon = i + 1;
      while (true) {
        if (semicolon >= input.size()) {
          std::cerr << "Line " << input[typeName].lineNumber << ", Column "
                    << input[typeName].columnNumber
                    << ", Parser error: Expected a semi-colon to end the "
                       "variable declaration of type "
                    << input[typeName].text << "!" << std::endl;
          break;
        }
        if (input[semicolon].text == ";")
          break;
        semicolon++;
      }
      TreeNodes nodesThatTheRecursionDealsWith(input.begin() + typeName,
                                               input.begin() + semicolon);
      nodesThatTheRecursionDealsWith =
          parseVariableDeclaration(nodesThatTheRecursionDealsWith);
      input[typeName] = nodesThatTheRecursionDealsWith[0];
      input.erase(input.begin() + typeName + 1, input.begin() + semicolon);
    } else if (input[i].text == "InstantiateStructure" and
               input[i].children.empty()) {
      if (i == input.size() - 1) {
        std::cerr << "Line " << input[i].lineNumber << ", Column "
                  << input[i].columnNumber
                  << ", Parser error: Unexpected end of file!" << std::endl;
        return input;
      }
      unsigned int typeName = i;
      unsigned int semicolon = i + 1;
      while (true) {
        if (semicolon >= input.size()) {
          std::cerr << "Line " << input[typeName].lineNumber << ", Column "
                    << input[typeName].columnNumber
                    << ", Parser error: Expected a semicolon to end the "
                       "variable declaration of type "
                    << input[typeName + 1].text << "!" << std::endl;
          break;
        }
        if (input[semicolon].text == ";")
          break;
        semicolon++;
      }
      TreeNodes nodesThatTheRecursionDealsWith(input.begin() + typeName + 1,
                                               input.begin() + semicolon);
      nodesThatTheRecursionDealsWith =
          parseVariableDeclaration(nodesThatTheRecursionDealsWith);
      input[typeName].children.push_back(nodesThatTheRecursionDealsWith[0]);
      input.erase(input.begin() + typeName + 1, input.begin() + semicolon);
    } else if (input[i].text == "Function") {
      if (i == input.size() - 1) {
        std::cerr << "Line " << input[i].lineNumber << ", Column "
                  << input[i].columnNumber
                  << ", Parser error: Unexpected end of file!" << std::endl;
        return input;
      }
      if (input[i + 1].text.back() != '(' or input[i + 1].text.size() == 1) {
        std::cerr << "Line " << input[i + 1].lineNumber << ", Column "
                  << input[i + 1].columnNumber
                  << ", Parser error: Expected a function name instead of \""
                  << input[i + 1].text << "\"!" << std::endl;
        return input;
      }
      unsigned int functionName = i + 1;
      unsigned int endOfFunctionSignature = i + 2;
      unsigned int counterOfParentheses = 1;
      while (counterOfParentheses) {
        if (endOfFunctionSignature >= input.size()) {
          std::cerr << "Line " << input[functionName].lineNumber
                    << ", Column: " << input[functionName].columnNumber
                    << ", Parser error: Parenthesis in \""
                    << input[functionName].text << "\" not closed!";
          break;
        }
        if (input[endOfFunctionSignature].text == ")")
          counterOfParentheses--;
        if (input[endOfFunctionSignature].text.back() == '(')
          counterOfParentheses++;
        endOfFunctionSignature++;
      }
      TreeNodes functionArguments(input.begin() + functionName + 1,
                                  input.begin() + endOfFunctionSignature - 1);
      input.erase(input.begin() + functionName + 1,
                  input.begin() + endOfFunctionSignature);
      TreeNodes argument;
      for (unsigned int i = 0; i < functionArguments.size() + 1; i++) {
        if (i == functionArguments.size() or functionArguments[i].text == ",") {
          input[functionName].children.push_back(
              parseVariableDeclaration(argument)[0]);
          argument = TreeNodes();
        } else
          argument.push_back(functionArguments[i]);
      }
      if (functionName > input.size() - 5) {
        std::cerr
            << "Line " << input[functionName].lineNumber << ", Column "
            << input[functionName].columnNumber
            << ", Parser error: Expected a function declaration of the format "
               "\"Function function_name(argument list) Which Returns "
               "type_name Does\" or \"Function function_name(argument list) "
               "Which Returns type_name Is External;\"!"
            << std::endl;
        return input;
      }
      if (input[functionName + 1].text != "Which") {
        std::cerr << "Line " << input[functionName + 1].lineNumber
                  << ", Column " << input[functionName + 1].columnNumber
                  << ", Parser error: Expected \"Which\" instead of \""
                  << input[functionName + 1].text << "\"!" << std::endl;
        return input;
      }
      if (input[functionName + 2].text != "Returns") {
        std::cerr << "Line " << input[functionName + 2].lineNumber
                  << ", Column " << input[functionName + 2].columnNumber
                  << ", Parser error: Expected \"Returns\" instead of \""
                  << input[functionName + 2].text << "\"!" << std::endl;
        return input;
      }
      input[functionName - 1].children.push_back(input[functionName]);
      input[functionName + 2].children.push_back(input[functionName + 3]);
      input[functionName - 1].children.push_back(input[functionName + 2]);
      if (input[functionName + 4].text == "Is") // External function
      {
        if (input[functionName + 5].text != "External") {
          std::cerr << "Line " << input[functionName + 5].lineNumber
                    << ", Column " << input[functionName + 5].columnNumber
                    << ", Parser error: Expected \"External\" instead of \""
                    << input[functionName + 5].text << "\"!" << std::endl;
          return input;
        }
        if (input[functionName + 6].text != ";") {
          std::cerr << "Line " << input[functionName + 6].lineNumber
                    << ", Column " << input[functionName + 6].columnNumber
                    << ", Parser error: Expected \";\" instead of \""
                    << input[functionName + 5].text << "\"!" << std::endl;
          return input;
        }
        input[functionName - 1].children.push_back(input[functionName + 5]);
        input.erase(
            input.begin() + functionName,
            input.begin() + functionName +
                7); // Delete all up to, and including, the semicolon ";".
      } else if (input[functionName + 4].text ==
                 "Does") // Function implemented in this file (right here).
      {
        unsigned int endOfTheFunction = functionName + 5;
        while (true) {
          if (endOfTheFunction >= input.size()) {
            std::cerr << "Line " << input[functionName + 4].lineNumber
                      << ", Column " << input[functionName + 5].columnNumber
                      << ", Parser error: The end of the function is not "
                         "marked with \"EndFunction\"."
                      << std::endl;
            break;
          }
          if (input[endOfTheFunction].text == "EndFunction")
            break;
          endOfTheFunction++;
        }
        TreeNodes nodesThatTheRecursionDealsWith(
            input.begin() + functionName + 5, input.begin() + endOfTheFunction);
        nodesThatTheRecursionDealsWith = parse(nodesThatTheRecursionDealsWith);
        input[functionName + 4].children.insert(
            input[functionName + 4].children.begin(),
            nodesThatTheRecursionDealsWith.begin(),
            nodesThatTheRecursionDealsWith.end());
        input[functionName - 1].children.push_back(input[functionName + 4]);
        input.erase(input.begin() + functionName,
                    input.begin() + endOfTheFunction +
                        1); // Because now it's all supposed to be the children
                            // of the "Function" node.
      } else {
        std::cerr << "Line " << input[functionName + 4].lineNumber
                  << ", Column " << input[functionName + 4].columnNumber
                  << ", Parser error: Expected either \"Is\" or \"Does\" "
                     "instead of \""
                  << input[functionName + 4].text << "\"." << std::endl;
        return input;
      }
    } else if (input[i].text == "While" and input[i].children.empty()) {
      auto iteratorOfTheLoopToken =
          std::find_if(input.begin(), input.end(),
                       [](TreeNode node) { return node.text == "Loop"; });
      if (iteratorOfTheLoopToken == input.end())
        std::cerr << "Line " << input[i].lineNumber << ", Column "
                  << input[i].columnNumber
                  << ", Parser error: There is a \"While\" token without its "
                     "corresponding \"Loop\" token!"
                  << std::endl;
      TreeNodes condition(input.begin() + i + 1, iteratorOfTheLoopToken);
      condition = parseExpression(condition);
      if (condition.size() == 0) {
        std::cerr << "Line " << input[i].lineNumber << ", Column "
                  << input[i].columnNumber
                  << ", Parser error: No expression between \"While\" and "
                     "\"Loop\" tokens!"
                  << std::endl;
        condition.push_back(
            TreeNode("0", input[i].lineNumber, input[i].columnNumber));
      }
      if (condition.size() > 1)
        std::cerr << "Line " << condition[1].lineNumber << ", Column "
                  << condition[1].columnNumber
                  << ", Parser error: Unexpected token \"" << condition[1].text
                  << "\"" << std::endl;
      input[i].children.push_back(condition[0]);
      if (iteratorOfTheLoopToken ==
          input.end()) // If there is no "Loop" token...
      {
        input.erase(input.begin() + i + 1, input.end());
        return input;
      }
      auto iteratorOfTheEndWhileToken = iteratorOfTheLoopToken;
      unsigned int counterOfWhileLoops = 1;
      do {
        iteratorOfTheEndWhileToken++;
        if (iteratorOfTheEndWhileToken == input.end())
          break;
        if (iteratorOfTheEndWhileToken->text == "While")
          counterOfWhileLoops++;
        if (iteratorOfTheEndWhileToken->text == "EndWhile")
          counterOfWhileLoops--;
      } while (counterOfWhileLoops);
      if (iteratorOfTheEndWhileToken == input.end())
        std::cerr << "Line " << iteratorOfTheLoopToken->lineNumber
                  << ", Column " << iteratorOfTheLoopToken->columnNumber
                  << ", Parser error: There is a \"Loop\" token without a "
                     "corresponding \"EndWhile\" token."
                  << std::endl;
      TreeNodes nodesThatTheRecursionDealsWith(iteratorOfTheLoopToken + 1,
                                               iteratorOfTheEndWhileToken);
      nodesThatTheRecursionDealsWith = parse(nodesThatTheRecursionDealsWith);
      iteratorOfTheLoopToken->children.insert(
          iteratorOfTheLoopToken->children.begin(),
          nodesThatTheRecursionDealsWith.begin(),
          nodesThatTheRecursionDealsWith.end());
      input[i].children.push_back(*iteratorOfTheLoopToken);
      input.erase(
          input.begin() + i + 1,
          iteratorOfTheEndWhileToken == input.end()
              ? iteratorOfTheEndWhileToken
              : iteratorOfTheEndWhileToken +
                    1); // If the "EndWhile" token exists, delete it now.
    } else if (input[i].text == "Return" and input[i].children.empty()) {
      auto iteratorOfTheSemicolon =
          std::find_if(input.begin() + i, input.end(),
                       [](TreeNode node) { return node.text == ";"; });
      if (iteratorOfTheSemicolon == input.end())
        std::cerr << "Line " << input[i].lineNumber << ", Column "
                  << input[i].columnNumber
                  << ", Parser error: The return statement is not terminated "
                     "with a semicolon."
                  << std::endl;
      TreeNodes expression(input.begin() + i + 1, iteratorOfTheSemicolon);
      expression = parseExpression(expression);
      if (expression.size() > 1)
        std::cerr << "Line " << expression[1].lineNumber << ", Column "
                  << expression[1].columnNumber
                  << ", Parser error: Unexpected token \"" << expression[1].text
                  << "\"" << std::endl;
      if (expression.size())
        input[i].children.push_back(
            expression[0]); // The function can return nothing at all, the
                            // parser must not segfault then!
      input.erase(input.begin() + i + 1, (iteratorOfTheSemicolon == input.end())
                                             ? iteratorOfTheSemicolon
                                             : iteratorOfTheSemicolon + 1);
    } else if (input[i].text == "If" and input[i].children.empty()) {
      auto iteratorPointingToTheThenToken =
          std::find_if(input.begin() + i, input.end(),
                       [](TreeNode node) { return node.text == "Then"; });
      if (iteratorPointingToTheThenToken == input.end())
        std::cerr << "Line " << input[i].lineNumber << ", Column "
                  << input[i].columnNumber
                  << ", Parser error: There is a \"If\" without a "
                     "corresponding \"Then\"!"
                  << std::endl;
      TreeNodes condition(input.begin() + i + 1,
                          iteratorPointingToTheThenToken);
      condition = parseExpression(condition);
      if (condition.empty()) {
        std::cerr << "Line " << input[i].lineNumber << ", Column "
                  << input[i].columnNumber
                  << ", Parser error: No expression between \"If\" and "
                     "\"Then\" tokens!"
                  << std::endl;
        condition.push_back(
            TreeNode("0", input[i].lineNumber, input[i].columnNumber));
      }
      if (condition.size() > 1)
        std::cerr << "Line " << condition[1].lineNumber << ", Column "
                  << condition[1].columnNumber
                  << ", Parser error: Unexpected token \"" << condition[1].text
                  << "\"!" << std::endl;
      input[i].children.push_back(condition.front());
      if (iteratorPointingToTheThenToken ==
          input.end()) // If there is no "Then"...
      {
        input.erase(input.begin() + i + 1, input.end());
        return input;
      }
      auto iteratorPointingToTheEndIfToken = iteratorPointingToTheThenToken;
      int counterOfIfBranches = 1;
      do {
        iteratorPointingToTheEndIfToken++;
        if (iteratorPointingToTheEndIfToken == input.end()) {
          std::cerr << "Line " << iteratorPointingToTheThenToken->lineNumber
                    << ", Column "
                    << iteratorPointingToTheThenToken->columnNumber
                    << ", Parser error: There is a \"Then\" token without the "
                       "corresponding \"EndIf\" token!"
                    << std::endl;
          break;
        }
        if (iteratorPointingToTheEndIfToken->text == "EndIf")
          counterOfIfBranches--;
        if (iteratorPointingToTheEndIfToken->text == "If")
          counterOfIfBranches++; // Don't look for "Then" tokens, because they
                                 // also come after the "ElseIf" statements.
      } while (counterOfIfBranches);
      auto iteratorPointingToTheElseIfToken = iteratorPointingToTheThenToken;
      counterOfIfBranches = 0;
      while (iteratorPointingToTheElseIfToken <
             iteratorPointingToTheEndIfToken) {
        if (!counterOfIfBranches and
            iteratorPointingToTheElseIfToken->text == "ElseIf")
          break; // If the "ElseIf" is referring to the "If" token at
                 // "input[i]", rather than to some nested "If" (which it does
                 // if "counterOfIfBranches" is non-zero).
        if (iteratorPointingToTheElseIfToken->text == "If")
          counterOfIfBranches++;
        if (iteratorPointingToTheElseIfToken->text == "EndIf")
          counterOfIfBranches--;
        iteratorPointingToTheElseIfToken++;
      }
      if (iteratorPointingToTheElseIfToken <
          iteratorPointingToTheEndIfToken) { // If there is an "ElseIf"
                                             // referring to this "If".
        TreeNodes nodesThatTheRecursionDealsWith(
            iteratorPointingToTheThenToken + 1,
            iteratorPointingToTheElseIfToken);
        nodesThatTheRecursionDealsWith = parse(nodesThatTheRecursionDealsWith);
        iteratorPointingToTheThenToken->children.insert(
            iteratorPointingToTheThenToken->children.begin(),
            nodesThatTheRecursionDealsWith.begin(),
            nodesThatTheRecursionDealsWith.end());
        input[i].children.push_back(*iteratorPointingToTheThenToken);
        iteratorPointingToTheElseIfToken->text = "Else";
        nodesThatTheRecursionDealsWith =
            TreeNodes(iteratorPointingToTheElseIfToken,
                      iteratorPointingToTheEndIfToken == input.end()
                          ? iteratorPointingToTheEndIfToken
                          : iteratorPointingToTheEndIfToken + 1);
        nodesThatTheRecursionDealsWith[0].text = "If";
        nodesThatTheRecursionDealsWith = parse(nodesThatTheRecursionDealsWith);
        iteratorPointingToTheElseIfToken->children.insert(
            iteratorPointingToTheElseIfToken->children.begin(),
            nodesThatTheRecursionDealsWith.begin(),
            nodesThatTheRecursionDealsWith.end());
        input[i].children.push_back(
            *iteratorPointingToTheElseIfToken); // Will appear as the "Else"
                                                // token to the compiler.
        input.erase(
            input.begin() + i + 1,
            iteratorPointingToTheEndIfToken == input.end()
                ? input.end()
                : iteratorPointingToTheEndIfToken +
                      1); // If there is an "EndIf" token, delete it as well.
      } else // No "ElseIf" token, but maybe there is an "Else" token. Let's
             // search for it!
      {
        auto iteratorPointingToTheElseToken = iteratorPointingToTheThenToken;
        counterOfIfBranches = 0;
        while (iteratorPointingToTheElseToken <
               iteratorPointingToTheEndIfToken) {
          if (!counterOfIfBranches and
              iteratorPointingToTheElseToken->text == "Else")
            break;
          if (iteratorPointingToTheElseToken->text == "If")
            counterOfIfBranches++;
          if (iteratorPointingToTheElseToken->text == "EndIf")
            counterOfIfBranches--;
          iteratorPointingToTheElseToken++;
        }
        if (iteratorPointingToTheElseToken <
            iteratorPointingToTheEndIfToken) // If there is an "Else" token...
        {
          TreeNodes nodesThatTheRecursionDealsWith(
              iteratorPointingToTheThenToken + 1,
              iteratorPointingToTheElseToken);
          nodesThatTheRecursionDealsWith =
              parse(nodesThatTheRecursionDealsWith);
          iteratorPointingToTheThenToken->children =
              nodesThatTheRecursionDealsWith;
          input[i].children.push_back(*iteratorPointingToTheThenToken);
          nodesThatTheRecursionDealsWith =
              TreeNodes(iteratorPointingToTheElseToken + 1,
                        iteratorPointingToTheEndIfToken);
          nodesThatTheRecursionDealsWith =
              parse(nodesThatTheRecursionDealsWith);
          iteratorPointingToTheElseToken->children =
              nodesThatTheRecursionDealsWith;
          input[i].children.push_back(*iteratorPointingToTheElseToken);
          input.erase(input.begin() + i + 1,
                      iteratorPointingToTheEndIfToken == input.end()
                          ? input.end()
                          : iteratorPointingToTheEndIfToken + 1);
        } else { // There is neither an "Else" nor an "ElseIf" token, so we can
                 // simply pass the tokens between (but not including) "Then"
                 // and "EndIf" token.
          TreeNodes nodesThatTheRecursionDealsWith(
              iteratorPointingToTheThenToken + 1,
              iteratorPointingToTheEndIfToken);
          nodesThatTheRecursionDealsWith =
              parse(nodesThatTheRecursionDealsWith);
          iteratorPointingToTheThenToken->children =
              nodesThatTheRecursionDealsWith;
          input[i].children.push_back(*iteratorPointingToTheThenToken);
          input.erase(input.begin() + i + 1,
                      iteratorPointingToTheEndIfToken == input.end()
                          ? input.end()
                          : iteratorPointingToTheEndIfToken + 1);
        }
      }
    } else if (input[i].text == "Structure" and input[i].children.empty()) {
      if (i == input.size() - 1) {
        std::cerr << "Line " << input[i].lineNumber << ", Column "
                  << input[i].columnNumber
                  << ", Parser error: Structure name not specified!"
                  << std::endl;
        return input;
      }
      input[i].children.push_back(input[i + 1]);
      input.erase(input.begin() + i +
                  1); // Delete the structure name, because it's been copied to
                      // the "input[i].children".
      if (i == input.size() - 1) {
        std::cerr << "Line " << input[i].lineNumber << ", Column "
                  << input[i].columnNumber
                  << ", Parser error: Unexpected end of file!" << std::endl;
        return input;
      }
      if (input[i + 1].text != "Consists") {
        std::cerr << "Line " << input[i + 1].lineNumber << ", Column "
                  << input[i + 1].columnNumber
                  << ", Parser error: Expected \"Consists\" instead of \""
                  << input[i + 1].text << "\"!" << std::endl;
        return input;
      }
      input.erase(input.begin() + i + 1); // Delete "Consists" from the AST.
      if (i == input.size() - 1) {
        std::cerr << "Line " << input[i].lineNumber << ", Column "
                  << input[i].columnNumber
                  << ", Parser error: Unexpected end of file!" << std::endl;
        return input;
      }
      if (input[i + 1].text != "Of") {
        std::cerr << "Line " << input[i + 1].lineNumber << ", Column "
                  << input[i + 1].columnNumber
                  << ", Parser error: Expected \"Of\" instead of \""
                  << input[i + 1].text << "\"!" << std::endl;
        return input;
      }
      auto iteratorPointingToTheEndStructureToken =
          std::find_if(input.begin() + i + 1, input.end(), [](TreeNode node) {
            return node.text == "EndStructure";
          }); // Let's not deal with nested structures for now.
      if (iteratorPointingToTheEndStructureToken == input.end())
        std::cerr << "Line " << input[i + 1].lineNumber << ", Column "
                  << input[i + 1].columnNumber
                  << ", Parser error: There is an \"Of\" token without a "
                     "corresponding \"EndStructure\" token."
                  << std::endl;
      TreeNodes structureMemberDeclaration;
      auto iteratorPointingToTheNextSemicolon =
          input.begin() + i + 2; // Right after the "Of" token.
      while (iteratorPointingToTheNextSemicolon !=
             iteratorPointingToTheEndStructureToken) {
        if (iteratorPointingToTheNextSemicolon->text == ";") {
          if (structureMemberDeclaration.empty())
            continue; // If somebody has written something like ";;".
          structureMemberDeclaration =
              parseVariableDeclaration(structureMemberDeclaration);
          if (structureMemberDeclaration.size() > 1)
            std::cerr << "Line " << structureMemberDeclaration[1].lineNumber
                      << ", Column "
                      << structureMemberDeclaration[1].columnNumber
                      << ", Parser error: Unexpected token \""
                      << structureMemberDeclaration[1].text << "\"!"
                      << std::endl;
          if (!structureMemberDeclaration
                   .empty()) // I am not sure if that's possible, but let's not
                             // segfault then.
            input[i + 1].children.push_back(
                structureMemberDeclaration[0]); // Make the structure member
                                                // declaration a child of the
                                                // "Of" token.
          structureMemberDeclaration =
              TreeNodes(); // Delete the structure member declaration from the
                           // AST.
        } else
          structureMemberDeclaration.push_back(
              *iteratorPointingToTheNextSemicolon);
        iteratorPointingToTheNextSemicolon++;
      }
      if (!structureMemberDeclaration.empty()) {
        std::cerr << "Line " << structureMemberDeclaration.back().lineNumber
                  << ", Column "
                  << structureMemberDeclaration.back().columnNumber
                  << ", Parser error: Expected a semicolon ';'!" << std::endl;
        structureMemberDeclaration =
            parseVariableDeclaration(structureMemberDeclaration);
        if (structureMemberDeclaration.size() > 1)
          std::cerr << "Line " << structureMemberDeclaration[1].lineNumber
                    << ", Column " << structureMemberDeclaration[1].columnNumber
                    << ", Parser error: Unexpected token \""
                    << structureMemberDeclaration[1].text << "\"!" << std::endl;
        if (!structureMemberDeclaration.empty())
          input[i + 1].children.push_back(structureMemberDeclaration[0]);
      }
      input[i].children.push_back(
          input[i + 1]); // Make the "Of" node a child of the "Structure" node.
      input.erase(input.begin() + i + 1,
                  iteratorPointingToTheEndStructureToken == input.end()
                      ? input.end()
                      : iteratorPointingToTheEndStructureToken +
                            1); // If there is an "EndStructure" node, delete it
                                // from the AST, together with the "Of" node.
    } else { // Assume that what follows is an expression, presumably including
             // a ":=".
      auto iteratorPointingToTheNextSemicolon =
          std::find_if(input.begin() + i, input.end(),
                       [](TreeNode node) { return node.text == ";"; });
      if (iteratorPointingToTheNextSemicolon == input.end())
        std::cerr << "Line " << input[i].lineNumber << ", Column "
                  << input[i].columnNumber
                  << ", Parser error: The expression starting with \""
                  << input[i].text << "\" is not ended in a semicolon!"
                  << std::endl;
      TreeNodes expression(input.begin() + i,
                           iteratorPointingToTheNextSemicolon);
      expression = parseExpression(expression);
      if (expression.size() > 1)
        std::cerr << "Line " << expression[1].lineNumber << ", Column "
                  << expression[1].columnNumber
                  << ", Parser error: Unexpected token \"" << expression[1].text
                  << "\"!" << std::endl;
      input.erase(
          input.begin() + i,
          (iteratorPointingToTheNextSemicolon != input.end())
              ? iteratorPointingToTheNextSemicolon + 1
              : iteratorPointingToTheNextSemicolon); // If there is a semicolon
                                                     // terminating the
                                                     // expression, delete it
                                                     // together with the
                                                     // expression.
      input.insert(input.begin() + i, expression.begin(), expression.end());
      i += expression.size() - 1;
    }
  return input;
}
