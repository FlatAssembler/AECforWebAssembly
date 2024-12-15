#include <cctype>
#include <string>
#include <vector>

bool isAllWhitespace(
    const std::string token) // No obvious way to do it in REGEX so that CLANG
                             // on Linux won't miscompile it.
{
  for (unsigned i = 0; i < token.size(); i++)
    if (!std::isspace(token[i]))
      return false;
  return true;
}

bool isAllDigits(const std::string token) {
  if (!token.size())
    return false;
  for (unsigned i = 0; i < token.size(); i++)
    if (!std::isdigit(token[i]))
      return false;
  return true;
}

bool isWordCharacterButNotDigit(
    const char c) // No obvious way to do it in REGEX so that CLANG on Linux
                  // won't miscompile it.
{
  return (std::isalnum(c) or c == '_') and !std::isdigit(c);
}

bool isInteger(std::string str) {
  // std::regex("(^\\d+$)|(^0x(\\d|[a-f]|[A-F])+$)"), so that CLANG on Oracle
  // Linux doesn't miscompile it (as it miscompiles nearly all regexes).
  if (!str.size())
    return false;
  if (str.substr(0, 2) == "0x") { // Hexadecimal numbers...
    if (str.size() == 2)
      return false;
    for (unsigned i = 2; i < str.size(); i++)
      if (!std::isdigit(str[i]) and !((str[i] >= 'A' and str[i] <= 'F') or
                                      (str[i] >= 'a' and str[i] <= 'f')))
        return false;
    return true;
  }
  // Integer decimal numbers...
  for (unsigned i = 0; i < str.size(); i++)
    if (!std::isdigit(str[i]))
      return false;
  return true;
}

bool isDecimalNumber(std::string str) {
  if (!str.size())
    return false;
  bool haveWePassedOverADecimalPoint = false;
  if (str[0] == '.')
    return false;
  for (unsigned i = 0; i < str.size(); i++)
    if (str[i] == '.' and !haveWePassedOverADecimalPoint)
      haveWePassedOverADecimalPoint = true;
    else if (str[i] == '.')
      return false;
    else if (!isdigit(str[i]))
      return false;
  return true;
}

bool isValidVariableName(std::string str) {
  // std::regex("^(_|[a-z]|[A-Z])\\w*\\[?$")
  if (!str.size())
    return false;
  if (std::isdigit(str[0]))
    return false;
  for (unsigned i = 0; i < str.size(); i++)
    if (!std::isalnum(str[i]) and str[i] != '_' and
        !(i == str.size() - 1 and str[i] == '['))
      return false;
  return true;
}

bool isPointerType(std::string str) {
  if (str.size() < std::string("Pointer").size())
    return false;
  return str.substr(str.size() - std::string("Pointer").size()) == "Pointer";
}

bool isDecimalType(std::string str) {
  return str == "Decimal32" or str == "Decimal64";
}

bool isComposedOfAlnumsAndOneDot(
    std::string token) // Done often in the tokenizer, so it's probably better
                       // (and more portable) to do it this way than in REGEX.
{
  bool passedOverADot = false;
  for (unsigned i = 0; i < token.size(); i++)
    if (token[i] == '.' and !passedOverADot and i != 0)
      passedOverADot = true;
    else if (token[i] == '.')
      return false;
    else if (!std::isalnum(token[i]) and token[i] != '_')
      return false;
  return true;
}

int longest_common_subsequence_length(std::string first, std::string second) {
  std::map<int, std::map<int, int>>
      DP; // There are, of course, faster ways to make multi-dimensional arrays
          // in C++, but let's not worry about performance of a function that
          // will be run only upon an error (to suggest a true name of a
          // misspelled variable name). Still, they are probably all easier than
          // dealing with multi-dimensional arrays in JavaScript.
  for (size_t i = 0; i < first.size();
       i++) // Microsoft C++ Compiler issues a warning if you put "unsigned"
            // instead of "size_t" here, I am not sure why.
    for (size_t j = 0; j < second.size(); j++)
      if (first[i] == second[j])
        DP[i][j] = DP[i - 1][j - 1] +
                   1; // Had we used vectors instead of maps, we could not do
                      // this so simply (What if 'i' or 'j' are zero?).
      else
        DP[i][j] = std::max(DP[i - 1][j], DP[i][j - 1]);
  return DP[first.size() - 1][second.size() - 1];
}

int Levenstein_distance(std::string A, std::string B) {
  // https://discord.com/channels/530598289813536771/847014270922391563/867319320485167115
  //  |
  //  V
  // https://github.com/royalpranjal/Interview-Bit/blob/master/DynamicProgramming/EditDistance.cpp
  using std::min;
  using std::vector;

  int row = A.size();
  int col = B.size();

  vector<vector<int>> temp(row + 1, vector<int>(col + 1));

  for (size_t i = 0; i < temp.size();
       i++) { // Apparently, GCC issues a warning under "-Wall" unless you
              // replace "int" with "size_t" here. I do not know why.
    for (size_t j = 0; j < temp[0].size(); j++) {
      if (j == 0) {
        temp[i][j] = i;
      } else if (i == 0) {
        temp[i][j] = j;
      } else if (A[i - 1] == B[j - 1]) {
        temp[i][j] = temp[i - 1][j - 1];
      } else {
        temp[i][j] = min(temp[i - 1][j - 1], temp[i - 1][j]);
        temp[i][j] = min(temp[i][j - 1], temp[i][j]);
        temp[i][j] = temp[i][j] + 1;
      }
    }
  }

  return temp[row][col];
}

#define USING_LEVENSTEIN_DISTANCE

bool ends_with(
    const std::string &first,
    const std::string &
        second) { // https://stackoverflow.com/questions/67451951/how-to-use-pointers-to-figure-out-if-a-c-string-ends-with-another-c-string/67452057?noredirect=1#comment119223072_67452057
  if (second.size() > first.size())
    return false;
  return first.substr(first.size() - second.size()) == second;
}

std::string demanglePointerType(std::string pointerType) {
  if (pointerType.substr(0, std::string("PointerTo").size()) == "PointerTo")
    return demanglePointerType(
        pointerType.substr(std::string("PointerTo").size()) + "Pointer");
  return pointerType;
}
