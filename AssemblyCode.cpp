#include <iterator>
#include <sstream>
#include <string>
#include <vector>

struct AssemblyCode {
  enum AssemblyType {
    i32,
    i64,
    f32,
    f64,
    null
  } assemblyType =
      null; // Those are actual types from WebAssembly (they obviously don't
            // correspond to the JavaScript types, for some reason).
  std::string code;
  AssemblyCode &indentBy(int numberOfTabs) {
    std::vector<std::string> tokenized;
    for (auto i = code.begin(); i != code.end(); i++)
      if (tokenized.empty()) {
        tokenized.push_back(std::string());
        tokenized.back().push_back(*i);
      } else if (*i == '\n') {
        tokenized.back().push_back(*i);
        tokenized.push_back(std::string());
      } else
        tokenized.back().push_back(*i);
    for (auto i = tokenized.begin(); i != tokenized.end(); i++)
      for (int j = 0; j < numberOfTabs; j++)
        *i = "\t" + *i;
    std::stringstream stream;
    std::ostream_iterator<std::string> streamIterator(stream);
    std::copy(tokenized.begin(), tokenized.end(), streamIterator);
    code = stream.str();
    return *this;
  }
  AssemblyCode(std::string code, AssemblyType type = null) {
    this->code = code;
    assemblyType = type;
  }
  operator std::string() { return code; }
  AssemblyCode operator+(AssemblyCode anotherAssemblyCode) {
    return AssemblyCode(code + anotherAssemblyCode.code, assemblyType);
  }
  AssemblyCode operator+(std::string newCode) {
    return AssemblyCode(code + newCode, assemblyType);
  }
  AssemblyCode operator+(char character) {
    std::string newCode = code;
    newCode += character;
    return AssemblyCode(newCode, assemblyType);
  }
  AssemblyCode &operator+=(char character) {
    code += character;
    return *this;
  }
  AssemblyCode &operator+=(AssemblyCode anotherAssemblyCode) {
    return *this = *this + anotherAssemblyCode;
  }
  AssemblyCode &operator+=(std::string newCode) {
    return *this = *this + newCode;
  }
  AssemblyCode &operator=(std::string newCode) {
    code = newCode;
    return *this;
  }
};
