#include "TreeNode.cpp"

AssemblyCode TreeNode::compile(CompilationContext context) {
  throw std::string("Compiler is not yet implemented!");
  return AssemblyCode("()");
}

AssemblyCode TreeNode::compileAPointer(CompilationContext context) {
  throw std::string("Compiler is not yet implemented!");
  return AssemblyCode("()");
}

std::string TreeNode::getType(CompilationContext context) {
  if (std::regex_match(text, std::regex("^\\d+$")))
    return "Integer64";
  if (std::regex_match(text, std::regex("^\\d+\\.\\d*$")))
    return "Decimal64";
  if (text == "AddressOf(")
    if (children.empty()) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: \"AddressOf\" is without the argument!"
                << std::endl;
      exit(1);
    } else
      return children[0].getType(context) + "Pointer";
  if (context.variableTypes.count(text))
    return context.variableTypes[text];
  auto potentialFunction =
      std::find_if(context.functions.begin(), context.functions.end(),
                   [=](function fn) { return fn.name == text; });
  if (potentialFunction != context.functions.end())
    return potentialFunction->returnType;
  throw std::string("Compiler is not yet implemented!");
  return "Nothing";
}
