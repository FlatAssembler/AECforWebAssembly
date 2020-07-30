#include "TreeNode.cpp"
#include "bitManipulations.cpp"

AssemblyCode TreeNode::compile(CompilationContext context) {
  std::cerr << "Compiler is not yet implemented!" << std::endl;
  exit(1);
  return AssemblyCode("()");
}

AssemblyCode TreeNode::compileAPointer(CompilationContext context) {
  if (text == "ValueAt(")
    return children[0].compile(context);
  std::cerr << "Compiler is not yet implemented!" << std::endl;
  exit(1);
  return AssemblyCode("()");
}

std::string getStrongerType(int lineNumber, int columnNumber,
                            std::string firstType, std::string secondType) {
  if (firstType == "Nothing" or secondType == "Nothing") {
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Compiler error: Can't add, subtract, multiply or divide "
                 "with something of the type \"Nothing\"!";
    exit(1);
  }
  if (std::regex_search(firstType, std::regex("Pointer$")) and
      !std::regex_search(secondType, std::regex("Pointer$")))
    return firstType;
  if (std::regex_search(secondType, std::regex("Pointer$")) and
      !std::regex_search(firstType, std::regex("Pointer$")))
    return secondType;
  if (std::regex_search(firstType, std::regex("Pointer$")) and
      std::regex_search(secondType, std::regex("Pointer$"))) {
    std::cerr
        << "Line " << lineNumber << ", Column " << columnNumber
        << ", Compiler error: Can't add, multiply or divide two pointers!";
  }
  if (firstType == "Decimal64" or secondType == "Decimal64")
    return "Decimal64";
  if (firstType == "Decimal32" or secondType == "Decimal32")
    return "Decimal32";
  if (firstType == "Integer64" or secondType == "Integer64")
    return "Integer64";
  if (firstType == "Integer32" or secondType == "Integer32")
    return "Integer32";
  if (firstType == "Integer16" or secondType == "Integer16")
    return "Integer16";
  return firstType;
}

std::string TreeNode::getType(CompilationContext context) {
  if (std::regex_match(text, std::regex("(^\\d+$)|(^0x(\\d|[a-f]|[A-F])+$)")))
    return "Integer64";
  if (std::regex_match(text, std::regex("^\\d+\\.\\d*$")))
    return "Decimal64";
  if (text == "AddressOf(") {
    if (children.empty()) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: \"AddressOf\" is without the argument!"
                << std::endl;
      exit(1);
    }
    if (children.size() > 1) {
      std::cerr
          << "Line " << lineNumber << ", Column " << columnNumber
          << ", Compiler error: Can't take the address of multiple variables!"
          << std::endl;
      exit(1);
    }
    if (children[0].getType(context) == "Nothing") {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: \"AddressOf\" has an argument of type "
                   "\"Nothing\"!"
                << std::endl;
      exit(1);
    }
    return children[0].getType(context) + "Pointer";
  }
  if (text == "ValueAt(") {
    if (children.empty()) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: \"ValueAt\" is without the argument!"
                << std::endl;
      exit(1);
    }
    if (children.size() > 1) {
      std::cerr
          << "Line " << lineNumber << ", Column " << columnNumber
          << ", Compiler error: Can't dereference multiple variables at once!"
          << std::endl;
      exit(1);
    }
    if (std::regex_search(text, std::regex("Pointer$")) == false) {
      std::cerr
          << "Line " << lineNumber << ", Column " << columnNumber
          << ", Compiler error: The argument to \"ValueAt\" is not a pointer!"
          << std::endl;
      exit(1);
    }
  }
  if (context.variableTypes.count(text))
    return context.variableTypes[text];
  if (text == "and" or text == "or" or text == "<" or text == ">" or
      text == "=" or text == "not(")
    return "Integer32"; // Because "if" and "br_if" in WebAssembly expect a
                        // "i32", so let's adapt to that.
  if (std::regex_match(text, std::regex("^(_|[a-z]|[A-Z])\\w*\\[?"))) {
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Compiler error: The variable name \"" << text
              << "\" is not declared!" << std::endl;
    exit(1);
  }
  if (text == "+" or text == "*" or text == "/")
    return getStrongerType(lineNumber, columnNumber,
                           children[0].getType(context),
                           children[1].getType(context));
  if (text == "-") {
    if (std::regex_search(children[0].getType(context),
                          std::regex("Pointer$")) and
        std::regex_search(children[1].getType(context), std::regex("Pointer$")))
      return "Integer32"; // Difference between pointers is an integer of the
                          // same size as the pointers (32-bit).
    return getStrongerType(lineNumber, columnNumber,
                           children[0].getType(context),
                           children[1].getType(context));
  }
  auto potentialFunction =
      std::find_if(context.functions.begin(), context.functions.end(),
                   [=](function fn) { return fn.name == text; });
  if (potentialFunction != context.functions.end())
    return potentialFunction->returnType;
  if (text.back() == '(' and
      basicDataTypeSizes.count(text.substr(0, text.size() - 1))) // Casting
    return text.substr(0, text.size() - 1);
  if (text.back() == '(') {
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Compiler error: Function \"" << text
              << "\" is not declared!" << std::endl;
    exit(1);
  }
  if (text == "?:")
    return getStrongerType(lineNumber, columnNumber,
                           children[1].getType(context),
                           children[2].getType(context));
  return "Nothing";
}
