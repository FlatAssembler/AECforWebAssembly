#include "TreeNode.cpp"
#include "bitManipulations.cpp"

AssemblyCode convertToInteger32(TreeNode node, CompilationContext context) {
  auto originalCode = node.compile(context);
  const AssemblyCode::AssemblyType i32 = AssemblyCode::AssemblyType::i32,
                                   i64 = AssemblyCode::AssemblyType::i64,
                                   f32 = AssemblyCode::AssemblyType::f32,
                                   f64 = AssemblyCode::AssemblyType::f64,
                                   null = AssemblyCode::AssemblyType::null;
  if (originalCode.assemblyType == null) {
    std::cerr
        << "Line " << node.lineNumber << ", Column " << node.columnNumber
        << ", Compiler error: Some part of the compiler attempted to convert \""
        << node.text
        << "\" to \"Integer32\", which makes no sense. This could be an "
           "internal compiler error, or there could be something semantically "
           "(though not grammatically) very wrong with your program."
        << std::endl;
    exit(1);
  }
  if (originalCode.assemblyType == i32)
    return originalCode;
  if (originalCode.assemblyType == i64)
    return AssemblyCode(
        "(i32.wrap_64\n" + std::string(originalCode.indentBy(1)) + "\n)", i32);
  if (originalCode.assemblyType == f32)
    return AssemblyCode(
        "(i32.trunc_f32_s\n" + std::string(originalCode.indentBy(1)) + "\n)",
        i32); // Makes little sense to me (that, when converting to an integer,
              // the decimal part of the number is simply truncated), but that's
              // how it is done in the vast majority of programming languages.
  if (originalCode.assemblyType == f64)
    return AssemblyCode("(i32.trunc_f64_s\n" +
                            std::string(originalCode.indentBy(1)) + "\n)",
                        i32);
  std::cerr << "Line " << node.lineNumber << ", Column " << node.columnNumber
            << ", Compiler error: Internal compiler error, control reached the "
               "end of the \"convertToInteger32\" function!"
            << std::endl;
  exit(-1);
  return AssemblyCode("()");
}

AssemblyCode convertToInteger64(TreeNode node, CompilationContext context) {
  auto originalCode = node.compile(context);
  const AssemblyCode::AssemblyType i32 = AssemblyCode::AssemblyType::i32,
                                   i64 = AssemblyCode::AssemblyType::i64,
                                   f32 = AssemblyCode::AssemblyType::f32,
                                   f64 = AssemblyCode::AssemblyType::f64,
                                   null = AssemblyCode::AssemblyType::null;
  if (originalCode.assemblyType == null) {
    std::cerr
        << "Line " << node.lineNumber << ", Column " << node.columnNumber
        << ", Compiler error: Some part of the compiler attempted to convert \""
        << node.text
        << "\" to \"Integer64\", which makes no sense. This could be an "
           "internal compiler error, or there could be something semantically "
           "(though not grammatically) very wrong with your program."
        << std::endl;
    exit(1);
  }
  if (originalCode.assemblyType == i32)
    return AssemblyCode(
        "(i64.extend_i32_s\n" + // If you don't put "_s", JavaScript Virtual
                                // Machine is going to interpret the argument as
                                // unsigned, leading to huge positive numbers
                                // instead of negative ones.
            std::string(originalCode.indentBy(1)) + "\n)",
        i64);
  if (originalCode.assemblyType == i64)
    return originalCode;
  if (originalCode.assemblyType == f32)
    return AssemblyCode("(i64.trunc_f32_s\n" +
                            std::string(originalCode.indentBy(1)) + "\n)",
                        i64);
  if (originalCode.assemblyType == f64)
    return AssemblyCode("(i64.trunc_f64_s\n" +
                            std::string(originalCode.indentBy(1)) + "\n)",
                        i64);
  std::cerr << "Line " << node.lineNumber << ", Column " << node.columnNumber
            << ", Compiler error: Internal compiler error, control reached the "
               "end of the \"convertToInteger64\" function!"
            << std::endl;
  exit(-1);
  return AssemblyCode("()");
}

AssemblyCode convertToDecimal32(TreeNode node, CompilationContext context) {
  auto originalCode = node.compile(context);
  const AssemblyCode::AssemblyType i32 = AssemblyCode::AssemblyType::i32,
                                   i64 = AssemblyCode::AssemblyType::i64,
                                   f32 = AssemblyCode::AssemblyType::f32,
                                   f64 = AssemblyCode::AssemblyType::f64,
                                   null = AssemblyCode::AssemblyType::null;
  if (originalCode.assemblyType == null) {
    std::cerr
        << "Line " << node.lineNumber << ", Column " << node.columnNumber
        << ", Compiler error: Some part of the compiler attempted to convert \""
        << node.text
        << "\" to \"Decimal32\", which makes no sense. This could be an "
           "internal compiler error, or there could be something semantically "
           "(though not grammatically) very wrong with your program."
        << std::endl;
    exit(1);
  }
  if (originalCode.assemblyType == i32)
    return AssemblyCode(
        "(f32.convert_i32_s\n" + // Again, those who designed JavaScript Virtual
                                 // Machine had a weird idea that integers
                                 // should be unsigned unless somebody makes
                                 // them explicitly signed via "_s".
            std::string(originalCode.indentBy(1)) + "\n)",
        f32);
  if (originalCode.assemblyType == i64)
    return AssemblyCode("(f32.convert_i64_s\n" +
                            std::string(originalCode.indentBy(1)) + "\n)",
                        f32);
  if (originalCode.assemblyType == f32)
    return originalCode;
  if (originalCode.assemblyType == f64)
    return AssemblyCode("(f32.demote_f64\n" +
                            std::string(originalCode.indentBy(1)) + "\n)",
                        f32);
  std::cerr << "Line " << node.lineNumber << ", Column " << node.columnNumber
            << ", Compiler error: Internal compiler error, control reached the "
               "end of the \"convertToDecimal32\" function!"
            << std::endl;
  exit(-1);
  return AssemblyCode("()");
}

AssemblyCode convertToDecimal64(TreeNode node, CompilationContext context) {
  auto originalCode = node.compile(context);
  const AssemblyCode::AssemblyType i32 = AssemblyCode::AssemblyType::i32,
                                   i64 = AssemblyCode::AssemblyType::i64,
                                   f32 = AssemblyCode::AssemblyType::f32,
                                   f64 = AssemblyCode::AssemblyType::f64,
                                   null = AssemblyCode::AssemblyType::null;
  if (originalCode.assemblyType == null) {
    std::cerr
        << "Line " << node.lineNumber << ", Column " << node.columnNumber
        << ", Compiler error: Some part of the compiler attempted to convert \""
        << node.text
        << "\" to \"Decimal64\", which makes no sense. This could be an "
           "internal compiler error, or there could be something semantically "
           "(though not grammatically) very wrong with your program."
        << std::endl;
    exit(1);
  }
  if (originalCode.assemblyType == i32)
    return AssemblyCode("(f64.convert_i32_s\n" +
                            std::string(originalCode.indentBy(1)) + "\n)",
                        f64);
  if (originalCode.assemblyType == i64)
    return AssemblyCode("(f64.convert_i64_s\n" +
                            std::string(originalCode.indentBy(1)) + "\n)",
                        f64);
  if (originalCode.assemblyType == f32)
    return AssemblyCode("(f64.promote_f32\n" +
                            std::string(originalCode.indentBy(1)) + "\n)",
                        f64);
  if (originalCode.assemblyType == f64)
    return originalCode;
  std::cerr << "Line " << node.lineNumber << ", Column " << node.columnNumber
            << ", Compiler error: Internal compiler error, control reached the "
               "end of the \"convertToDecimal64\" function!"
            << std::endl;
  exit(-1);
  return AssemblyCode("()");
}

AssemblyCode convertTo(TreeNode node, std::string type,
                       CompilationContext context) {
  if (type == "Character" or type == "Integer16" or type == "Integer32" or
      std::regex_search(
          type,
          std::regex(
              "Pointer$"))) // When, in JavaScript Virtual Machine, you can't
                            // push types of less than 4 bytes (32 bits) onto
                            // the system stack, you need to convert those to
                            // Integer32 (i32). Well, makes slightly more sense
                            // than the way it is in 64-bit x86 assembly, where
                            // you can put 16-bit values and 64-bit values onto
                            // the system stack, but you can't put 32-bit
                            // values.
    return convertToInteger32(node, context);
  if (type == "Integer64")
    return convertToInteger64(node, context);
  if (type == "Decimal32")
    return convertToDecimal32(node, context);
  if (type == "Decimal64")
    return convertToDecimal64(node, context);
  std::cerr << "Line " << node.lineNumber << ", Column " << node.columnNumber
            << ", Compiler error: Some part of the compiler attempted to get "
               "the assembly code for converting \""
            << node.text << "\" into the type \"" << type
            << "\", which doesn't make sense. This could be an internal "
               "compiler error, or there could be something semantically "
               "(though not grammatically) very wrong with your program."
            << std::endl;
  exit(-1);
  return AssemblyCode("()");
}

AssemblyCode TreeNode::compile(CompilationContext context) {
  std::cerr << "Compiler is not yet implemented!" << std::endl;
  exit(1);
  return AssemblyCode("()");
}

AssemblyCode TreeNode::compileAPointer(CompilationContext context) {
  if (text == "ValueAt(")
    return children[0].compile(context);
  if (context.localVariables.count(text) and text.back() != '[')
    return AssemblyCode(
        "(i32.sub\n\t(global.get $stack_pointer)\n\t(i32.const " +
            std::to_string(context.localVariables[text]) + ") ;;" + text +
            "\n)",
        AssemblyCode::AssemblyType::i32);
  if (context.localVariables.count(text) and text.back() == '[') {
    if (children.empty()) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: The array \""
                << text.substr(0, text.size() - 1)
                << "\" has no index in the AST. Aborting the compilation, or "
                   "else the compiler will segfault!"
                << std::endl;
      exit(1);
    }
    return AssemblyCode(
        "(i32.add\n\t(i32.sub\n\t\t(global.get "
        "$stack_pointer)\n\t\t(i32.const " +
            std::to_string(context.localVariables[text]) + ") ;;" + text +
            "\n\t)\n)" +
            std::string(convertToInteger32(children[0], context).indentBy(1)) +
            "\n)",
        AssemblyCode::AssemblyType::i32);
  }
  if (context.globalVariables.count(text) and text.back() != '[')
    return AssemblyCode("(i32.const " +
                            std::to_string(context.globalVariables[text]) +
                            ") ;;" + text,
                        AssemblyCode::AssemblyType::i32);
  if (context.globalVariables.count(text) and text.back() == '[') {
    if (children.empty()) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: The array \""
                << text.substr(0, text.size() - 1)
                << "\" has no index in the AST. Aborting the compilation, or "
                   "else the compiler will segfault!"
                << std::endl;
      exit(1);
    }
    return AssemblyCode(
        "(i32.add\n\t(i32.const " +
            std::to_string(context.globalVariables[text]) + ") ;;" + text +
            "\n" +
            std::string(convertToInteger32(children[0], context).indentBy(1)) +
            "\n)",
        AssemblyCode::AssemblyType::i32);
  }
  std::cerr << "Line " << lineNumber << ", Column " << columnNumber
            << ", Compiler error: Some part of the compiler attempted to get "
               "the assembly of the pointer to \""
            << text
            << "\", which makes no sense. This could be an internal compiler "
               "error, or there could be something semantically (though not "
               "grammatically) very wrong with your program."
            << std::endl;
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
      text == "=" or text == "not(") {
    if (children.empty()) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: The operator \"" << text
                << "\" has no operands. Aborting the compilation (or else we "
                   "will segfault)!"
                << std::endl;
      exit(1);
    }
    if (children.size() < 2 and text != "not(") {
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
    if (std::regex_search(children[0].getType(context),
                          std::regex("^Decimal")) or
        std::regex_search(children[1].getType(context),
                          std::regex("^Decimal"))) {
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
      text == "Loop" or text == "Does") // Or else the compiler will claim those
                                        // tokens are undeclared variables.
    return "Nothing";
  if (std::regex_match(text, std::regex("^(_|[a-z]|[A-Z])\\w*\\[?"))) {
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Compiler error: The variable name \"" << text
              << "\" is not declared!" << std::endl;
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
    if (std::regex_search(children[0].getType(context),
                          std::regex("Pointer$")) and
        std::regex_search(children[1].getType(context), std::regex("Pointer$")))
      return "Integer32"; // Difference between pointers is an integer of the
                          // same size as the pointers (32-bit).
    return getStrongerType(lineNumber, columnNumber,
                           children[0].getType(context),
                           children[1].getType(context));
  }
  if (text == ":=") {
    if (children.size() < 2) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: The assignment operator \":=\" has less "
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
