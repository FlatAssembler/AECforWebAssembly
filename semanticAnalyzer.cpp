/*
 * Here we put the parts of the compiler that analyze the syntax tree,
 * but do no assembly-related things. Stuff related to typing belongs
 * here.
 */

#include "NotImplementedException.cpp"
#include "TreeNode.cpp"
#include <ciso646> // Necessary for Microsoft C++ Compiler.

std::string getStrongerType(const int lineNumber, const int columnNumber,
                            const std::string firstType,
                            const std::string secondType) {
  if (firstType == "Nothing" or secondType == "Nothing") {
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Compiler error: Can't add, subtract, multiply or divide "
                 "with something of the type \"Nothing\"!"
              << std::endl;
    exit(1);
  }
  if (isPointerType(firstType) and !isPointerType(secondType))
    return firstType;
  if (isPointerType(secondType) and !isPointerType(firstType))
    return secondType;
  if (isPointerType(firstType) and isPointerType(secondType)) {
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Compiler error: Can't add, multiply or divide two pointers!"
              << std::endl;
    exit(1);
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

std::string TreeNode::getType(const CompilationContext &context) const {
  if (text == "nan") // Not-a-number, to signal a numeric error.
    return "Decimal32";
  if (text == "asm(" and children.size() == 1)
    return "Nothing";
  if ((text == "asm_i32(" or text == "SizeOf(") and children.size() == 1)
    return "Integer32";
  if (text == "asm_i64(" and children.size() == 1)
    return "Integer64";
  if (text == "asm_f32(" and children.size() == 1)
    return "Decimal32";
  if (text == "asm_f64(" and children.size() == 1)
    return "Decimal64";
  if (text == "asm(" or text == "asm_i32(" or text == "asm_i64(" or
      text == "asm_f32(" or text == "asm_f64(" or text == "SizeOf") {
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Compiler error: The inline assembly operator \"" << text
              << "\" has " << children.size()
              << " operands (it should have 1). Its AST is: "
              << getLispExpression() << std::endl;
    std::exit(1);
  }
  if (isInteger(text))
    return "Integer64";
  if (isDecimalNumber(text))
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
    if (!isPointerType(children[0].getType(context))) {
      std::cerr
          << "Line " << lineNumber << ", Column " << columnNumber
          << ", Compiler error: The argument to \"ValueAt\" is not a pointer!"
          << std::endl;
      exit(1);
    }
    return children[0].getType(context).substr(
        0, children[0].getType(context).size() - std::string("Pointer").size());
  }
  if (context.variableTypes.count(text))
    return context.variableTypes.at(text);
  if (text[0] == '"') {
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Internal compiler error: A pointer to the string " << text
              << " is being attempted to compile before the string itself has "
                 "been compiled, aborting the compilation!"
              << std::endl;
    throw std::runtime_error(
        "String constant appears to be an undeclared variable!");
  }
  if (text == "and" or text == "or" or text == "<" or text == ">" or
      text == "=" or text == "not(" or text == "invertBits(") {
    if (children.empty()) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: The operator \"" << text
                << "\" has no operands. Aborting the compilation (or else we "
                   "will segfault)!"
                << std::endl;
      exit(1);
    }
    if (children.size() < 2 and text != "not(" and text != "invertBits(") {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: The binary operator \"" << text
                << "\" has less than two operands. Aborting the compilation "
                   "(or else we will segfault)!"
                << std::endl;
      exit(1);
    }
    return "Integer32"; // Because "if" and "br_if" in WebAssembly expect a
                        // "i32", so let's adapt to that.
  }
  if (text == "mod(") {
    if (children.size() != 2) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: \"mod(\" operator requires two integer "
                   "arguments!"
                << std::endl;
      exit(1);
    }
    if (isDecimalType(children[0].getType(context)) or
        isDecimalType(children[1].getType(context))) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: Unfortunately, WebAssembly (unlike x86 "
                   "assembly) doesn't support computing remaining of division "
                   "of decimal numbers, so we can't support that either "
                   "outside of compile-time constants."
                << std::endl;
      exit(1);
    }
    return getStrongerType(lineNumber, columnNumber,
                           children[0].getType(context),
                           children[1].getType(context));
  }
  if (text == "If" or text == "Then" or text == "Else" or text == "While" or
      text == "Loop" or text == "Does" or
      text == "Return") // Or else the compiler will claim those
                        // tokens are undeclared variables.
    return "Nothing";
  if (isValidVariableName(text)) {
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Compiler error: The variable name \"" << text
              << "\" is not declared!" << std::endl;
    auto most_similar_variable_iterator = std::max_element(
        context.variableTypes.begin(), context.variableTypes.end(),
        [=](const std::pair<std::string, std::string>
                first_potentially_similar_variable,
            const std::pair<std::string, std::string>
                second_potentially_similar_variable) {
          return
#ifndef USING_LEVENSTEIN_DISTANCE
              longest_common_subsequence_length(
                  first_potentially_similar_variable.first, text) <
              longest_common_subsequence_length(
                  second_potentially_similar_variable.first, text);
#else
              Levenstein_distance(first_potentially_similar_variable.first,
                                  text) >
              Levenstein_distance(second_potentially_similar_variable.first,
                                  text);
#endif
        });
    if (most_similar_variable_iterator != context.variableTypes.end() &&
        longest_common_subsequence_length(most_similar_variable_iterator->first,
                                          text)) {
      std::cerr << "By the way, maybe you meant \""
                << most_similar_variable_iterator->first
                << "\", declared at the line "
                << (context.placesOfVariableDeclarations.count(
                        most_similar_variable_iterator->first)
                        ? context.placesOfVariableDeclarations.at(
                              most_similar_variable_iterator->first)
                        : 0)
                << " and of the type \""
                << most_similar_variable_iterator->second << "\""
                << "? " << std::endl;
    }
    exit(1);
  }
  if (text == "+" or text == "*" or text == "/") {
    if (children.size() != 2) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: The binary operator \"" << text
                << "\" doesn't have exactly two operands. Aborting the "
                   "compilation (or else we will segfault)!"
                << std::endl;
      exit(1);
    }
    return getStrongerType(lineNumber, columnNumber,
                           children[0].getType(context),
                           children[1].getType(context));
  }
  if (text == "-") {
    if (children.size() != 2) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: The binary operator \"" << text
                << "\" doesn't have exactly two operands. Aborting the "
                   "compilation (or else we will segfault)!"
                << std::endl;
      exit(1);
    }
    if (isPointerType(children[0].getType(context)) and
        isPointerType(children[1].getType(context)))
      return "Integer32"; // Difference between pointers is an integer of the
                          // same size as the pointers (32-bit).
    return getStrongerType(lineNumber, columnNumber,
                           children[0].getType(context),
                           children[1].getType(context));
  }
  if (text.size() == 2 and text[1] == '=') // Assignment operators
  {
    if (children.size() < 2) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: The assignment operator \"" << text
                << "\" has less "
                   "than two operands. Aborting the compilation, or else the "
                   "compiler will segfault."
                << std::endl;
      exit(1);
    }
    if (children[1].getType(context) == "Nothing") {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: Attempting to assign something of the "
                   "type \"Nothing\" to a variable. Aborting the compilation!"
                << std::endl;
    }
    return children[0].getType(context);
  }
  auto potentialFunction =
      std::find_if(context.functions.begin(), context.functions.end(),
                   [=](function fn) { return fn.name == text; });
  if (potentialFunction != context.functions.end()) {
#define ENABLE_FUNCTION_ARGUMENTS
#ifndef ENABLE_FUNCTION_ARGUMENTS
    for (TreeNode child : children)
      if (child.text == ":=")
        throw NotImplementedException(
            "Line " + std::to_string(child.lineNumber) + ", Column " +
            std::to_string(child.columnNumber) +
            ": Sorry about that, but this compiler does "
            "not yet support named function arguments!"); // TODO: Implement
                                                          // named function
                                                          // arguments.
#endif
    return potentialFunction->returnType;
  }
  if (text.back() == '(' and
      (basicDataTypeSizes.count(text.substr(0, text.size() - 1)) or
       isPointerType(text.substr(0, text.size() - 1)))) // Casting
    return text.substr(0, text.size() - 1);
  if (text.back() == '(') {
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Compiler error: Function \"" << text
              << "\" is not declared!" << std::endl;
    auto most_similar_function_iterator = std::max_element(
        context.functions.begin(), context.functions.end(),
        [=](const function first_potentially_similar_function,
            const function second_potentially_similar_function) {
          return
#ifndef USING_LEVENSTEIN_DISTANCE
              longest_common_subsequence_length(
                  first_potentially_similar_function.name, text) <
              longest_common_subsequence_length(
                  second_potentially_similar_function.name, text);
#else
              Levenstein_distance(first_potentially_similar_function.name,
                                  text) >
              Levenstein_distance(second_potentially_similar_function.name,
                                  text);
#endif
        });
    if (most_similar_function_iterator != context.functions.end() &&
        longest_common_subsequence_length(most_similar_function_iterator->name,
                                          text)) {
      std::cerr << "By the way, maybe you meant \""
                << most_similar_function_iterator->name << "\"?" << std::endl;
    }
    exit(1);
  }
  if (text == "?:") {
    if (isPointerType(children[1].getType(context)) and
        isPointerType(children[2].getType(context)))
      return children[1].getType(context);
    else
      return getStrongerType(lineNumber, columnNumber,
                             children[1].getType(context),
                             children[2].getType(context));
  }
  if (text == ".") { // The dot operator for accessing structure members.
    if (children.size() != 2) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: Corrupt AST, the \".\" node should have "
                   "2 children, but it has "
                << children.size() << ". Quitting now!" << std::endl;
      exit(1);
    }
    std::string structureName = children[0].getType(context);
    auto iteratorPointingToTheStructure =
        std::find_if(context.structures.begin(), context.structures.end(),
                     [=](structure str) { return str.name == structureName; });
    if (iteratorPointingToTheStructure == context.structures.end()) {
      std::cerr
          << "Line " << children[0].lineNumber << ", Column "
          << children[0].columnNumber << ", Compiler error: The instance \""
          << children[0].text << "\" has the type \"" << structureName
          << "\", which doesn't appear to be a structure name. Quitting now!"
          << std::endl;
      exit(1);
    }
    std::string memberName = children[1].text;
    if (!iteratorPointingToTheStructure->memberTypes.count(children[1].text)) {
      std::cerr << "Line " << children[1].lineNumber << ", Column "
                << children[1].columnNumber
                << ", Compiler error: The instance \"" << children[0].text
                << "\", of the structure named \""
                << iteratorPointingToTheStructure->name
                << "\", doesn't have a member named \"" << memberName
                << "\". Quitting now!" << std::endl;
      auto most_similar_variable_iterator = std::max_element(
          iteratorPointingToTheStructure->memberTypes.begin(),
          iteratorPointingToTheStructure->memberTypes.end(),
          [=](const std::pair<std::string, std::string>
                  first_potentially_similar_variable,
              const std::pair<std::string, std::string>
                  second_potentially_similar_variable) {
            return
#ifndef USING_LEVENSTEIN_DISTANCE
                longest_common_subsequence_length(
                    first_potentially_similar_variable.first, memberName) <
                longest_common_subsequence_length(
                    second_potentially_similar_variable.first, memberName);
#else
                Levenstein_distance(first_potentially_similar_variable.first,
                                    memberName) >
                Levenstein_distance(second_potentially_similar_variable.first,
                                    memberName);
#endif
          });
      if (most_similar_variable_iterator !=
              iteratorPointingToTheStructure->memberTypes.end() &&
          longest_common_subsequence_length(
              most_similar_variable_iterator->first, memberName)) {
        std::cerr << "By the way, maybe you meant \""
                  << most_similar_variable_iterator->first << "\"?"
                  << std::endl;
      }
      exit(1);
    }
    return iteratorPointingToTheStructure->memberTypes.at(memberName);
  }
  if (text == "->") {
    TreeNode dotOperator(".", lineNumber, columnNumber);
    TreeNode valueAtOperator("ValueAt(", lineNumber, columnNumber);
    valueAtOperator.children.push_back(children[0]);
    dotOperator.children = <% valueAtOperator, children[1] %>;
    return dotOperator.getType(context);
  }
  return "Nothing";
}
