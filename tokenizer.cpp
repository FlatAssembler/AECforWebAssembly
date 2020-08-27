#include "TreeNode.cpp"

std::vector<TreeNode> TreeNode::tokenize(std::string input) {
#ifndef NDEBUG
  std::cerr << "DEBUG: Tokenizing the string \"" << input << "\"..."
            << std::endl;
#endif
  using std::regex;
  using std::regex_match;
  using std::string;
  using std::to_string;
  auto tokenizedExpression = std::vector<TreeNode>();
  int currentLine = 1, currentColumn = 1;
  bool areWeInAString = false, areWeInAComment = false;
  string stringDelimiter, commentDelimiter;
  try {
    for (unsigned int i = 0; i < input.size(); i++) {
#ifndef NDEBUG
      std::cerr << "DEBUG: Now we are tokenizing the character #" << i << ": '"
                << input[i] << "'." << std::endl;
      if (areWeInAString)
        std::cerr << "DEBUG: We are in a string." << std::endl;
      if (areWeInAComment)
        std::cerr << "DEBUG: We are in a comment." << std::endl;
#endif
      if (tokenizedExpression.size() == 0) {
        if (input.size() - i > 2 and
            (input.substr(i, 2) == "//" or input.substr(i, 2) == "/*")) {
          areWeInAComment = true;
          commentDelimiter = input.substr(i, 2);
          tokenizedExpression.push_back(TreeNode(string(), 1, 1));
          currentColumn++;
          continue;
        }
        tokenizedExpression.push_back(TreeNode(input.substr(i, 1), 1, 1));
        currentColumn++;
        if (input[i] == '"' or input[i] == '\'') {
          areWeInAString = true;
          stringDelimiter = input.substr(i, 1);
        }
      } else if ((input[i] == '"' or input[i] == '\'') and !areWeInAComment) {
        if (areWeInAString and stringDelimiter == input.substr(i, 1) and
            (i == 0 or input[i - 1] != '\\')) {
          tokenizedExpression.back().text.push_back(input[i]);
          tokenizedExpression.push_back(
              TreeNode(string(), currentLine, currentColumn));
          currentColumn++;
          areWeInAString = false;
        } else if (!areWeInAString) {
          currentColumn++;
          areWeInAString = true;
          stringDelimiter = input.substr(i, 1);
          tokenizedExpression.push_back(
              TreeNode(input.substr(i, 1), currentLine, currentColumn));
        } else
          tokenizedExpression.back().text.push_back(input[i]);
      } else if (input.size() - i > 2 and
                 (input.substr(i, 2) == "//" or input.substr(i, 2) == "/*") and
                 !areWeInAString and !areWeInAComment) {
        areWeInAComment = true;
        commentDelimiter = input.substr(i, 2);
      } else if (input[i] == '\n') { // If we came to the end of a line.
        if (areWeInAString) {
          std::cerr << "Line " << currentLine << ", Column "
                    << tokenizedExpression.back().columnNumber
                    << ", Tokenizer error: Unterminated string literal!"
                    << std::endl;
          areWeInAString = false;
        }
        if (areWeInAComment and commentDelimiter == "//")
          areWeInAComment = false;
        else if (areWeInAComment) {
          currentLine++;
          currentColumn = 1;
          continue;
        }
        currentLine++;
        currentColumn = 1;
        tokenizedExpression.push_back(
            TreeNode(string(), currentLine, currentColumn));
      } else if (regex_match(input.substr(i, 1),
                             regex("\\s")) and
                 !areWeInAString and
                 !areWeInAComment) { // If we came to some other whitespace.
        currentColumn++;
        tokenizedExpression.push_back(
            TreeNode(string(), currentLine, currentColumn));
      } else if ((std::isalnum(input[i]) or input[i] == '_') and
                 regex_match(
                     tokenizedExpression.back().text,
                     regex("(^(\\d|_|[a-z]|[A-Z])*$)|(^(\\d|_|[a-z]|[A-Z])+\\.("
                           "\\d|_|[a-z]|[A-Z])*$)")) and
                 !areWeInAString and !areWeInAComment) // Names and numbers
      {
        currentColumn++;
        tokenizedExpression.back().text += input[i];
      } else if (input[i] == '.' and
                 regex_match(tokenizedExpression.back().text,
                             regex("^\\d+$")) and
                 !areWeInAString and
                 !areWeInAComment) // If we are currently
                                   // tokenizing a number, a dot
                                   // character is a decimal point.
      {
        currentColumn++;
        tokenizedExpression.back().text += input[i];
      } else if (!areWeInAString and !areWeInAComment) {
        currentColumn++;
        tokenizedExpression.push_back(
            TreeNode(input.substr(i, 1), currentLine, currentColumn));
      } else if (areWeInAString and !areWeInAComment) {
        currentColumn++;
        tokenizedExpression.back().text += input[i];
      } else if (areWeInAComment and commentDelimiter == "/*" and
                 input.size() - i > 2 and input.substr(i, 2) == "*/" and
                 !areWeInAString) {
        areWeInAComment = false;
        currentColumn += 2;
        tokenizedExpression.push_back(
            TreeNode(string(), currentLine, currentColumn));
        i++;
      } else if (!areWeInAString and areWeInAComment) {
        currentColumn++;
      } else {
        std::cerr << "Line " << currentLine << ", Column " << currentColumn
                  << ", Internal compiler error: Tokenizer is in the "
                     "forbidden state!"
                  << std::endl;
      }
    }
    for (auto iterator = tokenizedExpression.begin();
         iterator < tokenizedExpression.end(); iterator++)
      if (iterator->text.size() == 3 and iterator->text.substr(0, 1) == "'" and
          iterator->text.substr(2, 1) == "'")
        iterator->text = std::to_string(int(iterator->text[1]));
    for (auto iterator = tokenizedExpression.begin();
         iterator < tokenizedExpression.end();
         iterator++) // Turn escape sequences into numbers.
      if (iterator->text == "'\\n'")
        iterator->text = to_string(int('\n'));
      else if (iterator->text == "'\\t'")
        iterator->text = to_string(int('\t'));
      else if (iterator->text == "'\\''")
        iterator->text = to_string(int('\''));
      else if (iterator->text == "'\\\\'")
        iterator->text = to_string(int('\\'));
    for (auto iterator = tokenizedExpression.begin();
         iterator < tokenizedExpression.end(); iterator++)
      if (regex_match(iterator->text, regex("^\\s*$"))) // Delete empty nodes.
      {
        iterator--;
        tokenizedExpression.erase(iterator + 1);
      }
    for (unsigned int i = 1; i < tokenizedExpression.size(); i++)
      if ((tokenizedExpression[i].text ==
               "(" or // Mark the names of functions...
           tokenizedExpression[i].text ==
               "[") and //...and arrays with ending '(' or '['
          regex_match(
              tokenizedExpression[i - 1].text.substr(0, 1), //...for the parser.
              regex("_|[a-z]|[A-Z]")) and
          tokenizedExpression[i - 1].text != "If" and
          tokenizedExpression[i - 1].text != "ElseIf" and
          tokenizedExpression[i - 1].text != "While" and
          tokenizedExpression[i - 1].text != "Return" and
          tokenizedExpression[i - 1].text != "and" and
          tokenizedExpression[i - 1].text != "or") {
        tokenizedExpression[i - 1].text += tokenizedExpression[i].text;
        tokenizedExpression.erase(tokenizedExpression.begin() + i);
      }
    for (unsigned int i = 1; i < tokenizedExpression.size(); i++)
      if (tokenizedExpression[i].text == "=" and
          tokenizedExpression[i - 1].text ==
              ":") // The ":=" assignment operator.
      {
        tokenizedExpression[i - 1].text = ":=";
        tokenizedExpression.erase(tokenizedExpression.begin() + i);
      }
    for (unsigned int i = 1; i < tokenizedExpression.size(); i++)
      if (tokenizedExpression[i].text[0] == '"' and
          tokenizedExpression[i - 1].text[0] ==
              '"') // Concatenate two strings next to each other (as in C and
                   // C++).
      {
        tokenizedExpression[i - 1].text =
            tokenizedExpression[i - 1].text.substr(
                0, tokenizedExpression[i - 1].text.size() - 1) +
            tokenizedExpression[i].text.substr(1);
        tokenizedExpression.erase(tokenizedExpression.begin() + i);
      }
  } catch (std::regex_error &error) {
    std::cerr << "Internal compiler error in tokenizer: " << error.what() << ":"
              << error.code() << std::endl;
    return std::vector<TreeNode>();
  }
#ifndef NDEBUG
  std::cerr << "DEBUG: Finished tokenizing the string \"" << input << "\"."
            << std::endl;
#endif
  return tokenizedExpression;
}
