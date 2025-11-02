/*
 * A tokenizer is a part of the compiler that, basically, tells other
 * parts of the compiler where the "word boundaries" in the programming
 * language are. There are some tools to produce them automatically, but
 * I haven't bothered learning to use them and have written a simple
 * tokenizer by myself.
 */

#include "TreeNode.cpp"
#include <algorithm>
#include <ciso646> // Necessary for Microsoft C++ Compiler.

#define NDEBUG_for_multiline_strings

std::vector<TreeNode> TreeNode::tokenize(const std::string input) {
#ifndef NDEBUG
  std::cerr << "DEBUG: Tokenizing the string \"" << input << "\"..."
            << std::endl;
#endif
  using std::string;
  using std::to_string;
  auto tokenizedExpression = std::vector<TreeNode>();
  int currentLine = 1, currentColumn = 1;
  bool areWeInAString = false, areWeInAComment = false;
  string stringDelimiter, commentDelimiter;
  int stringStartLine = 0,
      stringStartColumn = 0; // Track where the string starts
  for (unsigned int i = 0; i < input.size(); i++) {
    if (input[i] == '\r')
      continue;
#ifndef NDEBUG
    std::cerr << "DEBUG: Now we are tokenizing the character #" << i << ": '"
              << input[i] << "'." << std::endl;
    if (areWeInAString)
      std::cerr << "DEBUG: We are in a string." << std::endl;
    if (areWeInAComment)
      std::cerr << "DEBUG: We are in a comment." << std::endl;
#endif
    if (not(areWeInAString) and not(areWeInAComment) and input[i] == 'R' and
        i < input.length() - 1 and input[i + 1] == '"') { // Multi-line string
      areWeInAString = true;
      currentColumn++;
      stringStartLine = currentLine;
      stringStartColumn = currentColumn;
      stringDelimiter = ")";
      size_t where_is_the_open_paranthesis = i + 2;
      while (where_is_the_open_paranthesis < input.length() and
             input[where_is_the_open_paranthesis] != '(') {
        stringDelimiter += input[where_is_the_open_paranthesis];
        where_is_the_open_paranthesis++;
      }
      stringDelimiter += '"';
#ifndef NDEBUG_for_multiline_strings
      std::cerr << "DEBUG: String delimiter of this multi-line string is: "
                << stringDelimiter << std::endl;
#endif
      tokenizedExpression.push_back(
          TreeNode(input.substr(i, 1), currentLine, currentColumn));
    } else if (tokenizedExpression.size() == 0) {
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
    } else if ((input[i] == '"' or input[i] == '\'' or
                (not(stringDelimiter.empty()) and
                 i < input.length() - stringDelimiter.length() and
                 input.substr(i, stringDelimiter.length()) ==
                     stringDelimiter)) and
               !areWeInAComment) {
      if (areWeInAString and
          stringDelimiter == input.substr(i, stringDelimiter.length()) and
          ((stringDelimiter.length() == 1)
               ? ((i > 0 and input[i - 1] != '\\') or
                  ((i > 1 and input[i - 2] == '\\') and
                   (i > 2 and (input[i - 3] != '\\'))))
               : true)) {
        tokenizedExpression.back().text += stringDelimiter;
        if (stringDelimiter.length() > 1) // If it was a multi-line string
        {
          tokenizedExpression.back().text =
              JSONifyString(tokenizedExpression.back().text.substr(
                  stringDelimiter.length() + 1,
                  tokenizedExpression.back().text.length() -
                      2 * stringDelimiter.length() - 1));
          i += stringDelimiter.length() - 1;
          currentColumn += stringDelimiter.length() - 1;
        }
        tokenizedExpression.push_back(
            TreeNode(string(), currentLine, currentColumn));
        currentColumn++;
        areWeInAString = false;
      } else if (not(areWeInAString)) {
        currentColumn++;
        areWeInAString = true;
        stringStartLine = currentLine;
        stringStartColumn = currentColumn;
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
      if (areWeInAString &&
          stringDelimiter.length() // The length of a string delimiter is always
                                   // equal to 1 for single-line strings.
              < 2) {
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
      } else if (areWeInAString) // Multi-line string
      {
        currentLine++;
        currentColumn = 1;
        tokenizedExpression.back().text.push_back(input[i]);
        continue;
      }
      currentLine++;
      currentColumn = 1;
      tokenizedExpression.push_back(
          TreeNode(string(), currentLine, currentColumn));
    } else if (input[i] > 0 // So that Microsoft Visual C++ does not complain
                            // when a unicode character is passed to "isspace".
               and std::isspace(input[i]) and !areWeInAString and
               !areWeInAComment) { // If we came to some other whitespace.
      currentColumn++;
      tokenizedExpression.push_back(
          TreeNode(string(), currentLine, currentColumn));
    } else if (((input[i] > 0 // Again, Microsoft Visual Studio...
                 and std::isalnum(input[i])) or
                input[i] == '_') and
               isComposedOfAlnumsAndOneDot(tokenizedExpression.back().text) and
               !areWeInAString and !areWeInAComment) // Names and numbers
    {
      currentColumn++;
      tokenizedExpression.back().text += input[i];
    } else if (input[i] == '.' and
               isAllDigits(tokenizedExpression.back().text) and
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
      std::exit(1);
    }
  }
  if (areWeInAString)
    std::cerr
        << "Line " << stringStartLine << ", Column " << stringStartColumn
        << ", Tokenizer error: String "
        << (tokenizedExpression.empty()
                ? ""
                : tokenizedExpression.back().text.substr(
                      0, std::min(12,
                                  int(tokenizedExpression.back().text.size()))))
        << "... not terminated before the end of the program!" << std::endl;
  for (auto bitand treeNode :
       tokenizedExpression) // Convert "PointerToCharacter" to
                            // "CharacterPointer", "PointerToInteger32" to
                            // "Integer32Pointer", and so on...
    while (treeNode.text.substr(0, string("PointerTo").length()) == "PointerTo")
      treeNode.text =
          treeNode.text.substr(string("PointerTo").length()) + "Pointer";
  for (auto iterator = tokenizedExpression.begin();
       iterator < tokenizedExpression.end(); iterator++)
    if (std::isdigit(iterator->text[0]) and
        iterator->text.find('_') !=
            std::string::npos) //'_'-es used as thousand separators or the
                               // like...
      iterator->text.erase(
          std::remove(iterator->text.begin(), iterator->text.end(), '_'),
          iterator->text.end());
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
  // https://discord.com/channels/530598289813536771/847014270922391563/847580726811557998
  tokenizedExpression.erase(
      std::remove_if(
          tokenizedExpression.begin(), tokenizedExpression.end(),
          [](TreeNode treeNode) { return isAllWhitespace(treeNode.text); }),
      tokenizedExpression.end());
  for (unsigned int i = 1; i < tokenizedExpression.size(); i++)
    if ((tokenizedExpression[i].text == "(" or // Mark the names of functions...
         tokenizedExpression[i].text ==
             "[") and //...and arrays with ending '(' or '['...
        isWordCharacterButNotDigit(
            tokenizedExpression[i - 1].text[0]) //...for the parser.
        and tokenizedExpression[i - 1].text != "If" and
        tokenizedExpression[i - 1].text != "ElseIf" and
        tokenizedExpression[i - 1].text != "While" and
        tokenizedExpression[i - 1].text != "Return" and
        tokenizedExpression[i - 1].text != "and" and
        tokenizedExpression[i - 1].text != "or" and
        // If an left-hand-side ternary conditional operator is used right after
        // "Does", "Loop", "Then" or "Else"...
        tokenizedExpression[i - 1].text != "Does" and
        tokenizedExpression[i - 1].text != "Loop" and
        tokenizedExpression[i - 1].text != "Then" and
        tokenizedExpression[i - 1].text != "Else") {
      tokenizedExpression[i - 1].text += tokenizedExpression[i].text;
      tokenizedExpression.erase(tokenizedExpression.begin() + i);
    }
  for (unsigned int i = 1; i < tokenizedExpression.size(); i++)
    if (tokenizedExpression[i].text == "=" and
        (tokenizedExpression[i - 1].text == ":" // The ":=" assignment operator.
         or
         tokenizedExpression[i - 1].text == "+" // The "+=" assignment operator.
         or tokenizedExpression[i - 1].text == "-" or
         tokenizedExpression[i - 1].text == "*" or
         tokenizedExpression[i - 1].text == "/" or
         tokenizedExpression[i - 1].text == "<" or
         tokenizedExpression[i - 1].text == ">")) {
      tokenizedExpression[i - 1].text = tokenizedExpression[i - 1].text + "=";
      tokenizedExpression.erase(tokenizedExpression.begin() + i);
    }
  for (unsigned int i = 1; i < tokenizedExpression.size(); i++)
    if (tokenizedExpression[i].text == ">" &&
        tokenizedExpression[i - 1].text == "-") {
      tokenizedExpression[i - 1].text = tokenizedExpression[i - 1].text + ">";
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
      i--;
    }
  for (unsigned int i = 1; i < tokenizedExpression.size(); i++)
    if (tokenizedExpression[i].text == "{" // Hints to the code formatter,
                                           // will make parsing harder
                                           // if passed on to it.
        and (tokenizedExpression[i - 1].text == "Does" or
             tokenizedExpression[i - 1].text == "Then" or
             tokenizedExpression[i - 1].text == "Loop" or
             tokenizedExpression[i - 1].text == "Of" or
             tokenizedExpression[i - 1].text == "Else")) {
      tokenizedExpression.erase(tokenizedExpression.begin() + i);
      i--;
    }
  if (!tokenizedExpression.empty()) {
    for (unsigned int i = 0; i < tokenizedExpression.size() - 1; i++)
      if (tokenizedExpression[i].text == "}" and
          (tokenizedExpression[i + 1].text == "EndFunction" or
           tokenizedExpression[i + 1].text == "Else" or
           tokenizedExpression[i + 1].text == "ElseIf" or
           tokenizedExpression[i + 1].text == "EndIf" or
           tokenizedExpression[i + 1].text == "EndWhile" or
           tokenizedExpression[i + 1].text == "EndStructure")) {
        tokenizedExpression.erase(tokenizedExpression.begin() + i);
        i--;
      }
  }
#ifndef NDEBUG
  std::cerr << "DEBUG: Finished tokenizing the string \"" << input << "\"."
            << std::endl;
#endif
  return tokenizedExpression;
}
