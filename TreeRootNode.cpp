/*
 * So, this is the class for the root of the AST. It behaves significantly
 * differently from other nodes in the AST, so I thought it would make sense
 * to make a special class for it.
 */

#include "TreeNode.cpp"
#include <ciso646> // Necessary for Microsoft C++ Compiler.

AssemblyCode
instantiateGlobalStructure(const structure str, const int offset,
                           const std::vector<structure> structures) {
  std::string assemblyCode = ";;Instantiating a structure of the type \"" +
                             str.name + "\" at memory address " +
                             std::to_string(offset) + ".\n";
  for (auto memberName : str.memberNames) {
    if (!TreeNode().basicDataTypeSizes.count(str.memberTypes.at(memberName)) &&
        !isPointerType(
            str.memberTypes.at(memberName))) { // Nested global structures.
      auto iteratorPointingToTheStructure = std::find_if(
          structures.begin(), structures.end(), [=](structure innerStructure) {
            return str.memberTypes.at(memberName) == innerStructure.name;
          });
      if (iteratorPointingToTheStructure == structures.end()) {
        std::cerr << "Internal compiler error: Structure declaration in the "
                     "compilation context is corrupt. The structure \""
                  << str.name
                  << "\" is supposed to contain a nested structure named \""
                  << str.memberTypes.at(memberName)
                  << "\", but that structure is not present in the compilation "
                     "context. Quitting now before segfaulting!"
                  << std::endl;
        exit(1);
      }
      for (unsigned arrayIndex = 0; arrayIndex < str.arraySize.at(memberName);
           arrayIndex++)
        assemblyCode +=
            instantiateGlobalStructure(
                *iteratorPointingToTheStructure,
                offset +
                    arrayIndex * iteratorPointingToTheStructure->sizeInBytes +
                    str.memberOffsetInBytes.at(memberName),
                structures)
                .indentBy(1);
      continue;
    }
    if (str.defaultValuesOfMembers.count(memberName)) {
      int address = offset + str.memberOffsetInBytes.at(memberName);
      if (str.memberTypes.at(memberName) == "Character")
        assemblyCode +=
            "(data 0 (i32.const " + std::to_string(address) + ") " +
            getCharVectorRepresentationOfCharacter(
                str.defaultValuesOfMembers.at(memberName)) +
            ") ;;Hex of " +
            std::to_string(str.defaultValuesOfMembers.at(memberName)) +
            ", the default value of \"" + str.name + "." + memberName + "\"\n";
      else if (str.memberTypes.at(memberName) == "Integer16")
        assemblyCode +=
            "(data 0 (i32.const " + std::to_string(address) + ") " +
            getCharVectorRepresentationOfInteger16(
                str.defaultValuesOfMembers.at(memberName)) +
            ") ;;Hex of " +
            std::to_string(str.defaultValuesOfMembers.at(memberName)) +
            ", the default value of \"" + str.name + "." + memberName + "\"\n";
      else if (str.memberTypes.at(memberName) == "Integer32" or
               isPointerType(str.memberTypes.at(memberName)))
        assemblyCode +=
            "(data 0 (i32.const " + std::to_string(address) + ") " +
            getCharVectorRepresentationOfInteger32(
                str.defaultValuesOfMembers.at(memberName)) +
            ") ;;Hex of " +
            std::to_string(str.defaultValuesOfMembers.at(memberName)) +
            ", the default value of \"" + str.name + "." + memberName + "\"\n";
      else if (str.memberTypes.at(memberName) == "Integer64")
        assemblyCode +=
            "(data 0 (i32.const " + std::to_string(address) + ") " +
            getCharVectorRepresentationOfInteger64(
                str.defaultValuesOfMembers.at(memberName)) +
            ") ;;Hex of " +
            std::to_string(str.defaultValuesOfMembers.at(memberName)) +
            ", the default value of \"" + str.name + "." + memberName + "\"\n";
      else if (str.memberTypes.at(memberName) == "Decimal32")
        assemblyCode +=
            "(data 0 (i32.const " + std::to_string(address) + ") " +
            getCharVectorRepresentationOfDecimal32(
                str.defaultValuesOfMembers.at(memberName)) +
            ") ;;Hex of " +
            std::to_string(str.defaultValuesOfMembers.at(memberName)) +
            ", the default value of \"" + str.name + "." + memberName + "\"\n";
      else if (str.memberTypes.at(memberName) == "Decimal64")
        assemblyCode +=
            "(data 0 (i32.const " + std::to_string(address) + ") " +
            getCharVectorRepresentationOfInteger16(
                str.defaultValuesOfMembers.at(memberName)) +
            ") ;;Hex of " +
            std::to_string(str.defaultValuesOfMembers.at(memberName)) +
            ", the default value of \"" + str.name + "." + memberName + "\"\n";
      else
        std::cerr << "Compiler error: Can't assign an initial value to the "
                     "structure member \""
                  << str.name << '.' << memberName
                  << "\", because the compiler doesn't know how to convert the "
                     "type \""
                  << str.memberTypes.at(memberName)
                  << "\" to hexadecimal. The compilation will continue, but be "
                     "warned it might produce wrong code because of this."
                  << std::endl;
    }
  }
  assemblyCode +=
      ";;Finished instantiating the structure of type \"" + str.name + "\"\n";
  return AssemblyCode(assemblyCode);
}

class TreeRootNode : public TreeNode // Sets up the compilation context, rather
                                     // than doing the compilation itself.
{
public:
  TreeRootNode() {
    text = "module"; // That's how the root node of the AST is called in
                     // WebAssembly and JavaScript.
  }
  AssemblyCode compile() const {
    CompilationContext context;
    bool hasStackPointerBeenDeclared = false;
    std::string globalDeclarations =
        R"(;;Generated by AECforWebAssembly ( https://github.com/FlatAssembler/AECforWebAssembly ).
(module )";
    if (compilation_target == "browser") {
      globalDeclarations +=
          R"(
  (import "JavaScript" "memory" (memory 1)) ;;Assume that, in the JavaScript program linked to this
  ;;AEC program, there is an "extern"-ed bytearray of size 1 page (64KB) called "memory".
  ;;Have a better idea?
  (global $stack_pointer (import "JavaScript" "stack_pointer") (mut i32)) ;;The stack pointer being
  ;;visible from JavaScript will be useful when debugging the compiler. This breaks compatibility with
  ;;WebAssembly environments which do not support "WebAssembly.Global", such as Firefox 52 (the last
  ;;version of Firefox to run on Windows XP, and the first one supporting WebAssembly).
)";
      hasStackPointerBeenDeclared = true;
    } else if (compilation_target == "WASI")
      globalDeclarations +=
          R"(
  ;;Since we are targeting WASI, we must not declare anything (memory nor stack pointer) before all imports are over. 
)";
    else {
      std::cerr << "Internal Compiler Error: The current compilation target is "
                   "set to \""
                << compilation_target
                << "\", which is invalid. This should be impossible. Quitting "
                   "now, or else we will produce invalid assembly (or, perhaps "
                   "even more likely, crash)."
                << std::endl;
      std::exit(1);
    }
    auto allTheStrings = getStringsInSubnodes();
    for (auto string : allTheStrings) {
      context.globalVariables[string] = context.globalVariablePointer;
      context.variableTypes[string] = "CharacterPointer";
      if (string.back() != '"')
        string += '"';
      globalDeclarations += "\t(data 0 (i32.const " +
                            std::to_string(context.globalVariablePointer) +
                            ") " + string + ")\n";
      context.globalVariablePointer += string.size() - 1;
    }
    for (auto childNode : children) {
      if (basicDataTypeSizes.count(childNode.text) ||
          isPointerType(childNode.text)) { // Global variable declaration
        for (auto variableName : childNode.children) {
          if (!isValidVariableName(variableName.text) or
              AECkeywords.count(variableName.text)) {
            std::cerr
                << "Line " << variableName.lineNumber << ", Column "
                << variableName.columnNumber << ", Compiler error: \""
                << variableName.text
                << "\" is not a valid variable name! Aborting the compilation!"
                << std::endl;
            exit(1);
          }
          if (context.variableTypes.count(variableName.text))
            std::cerr << "Line " << childNode.lineNumber << ", Column "
                      << childNode.columnNumber
                      << ", Compiler warning: Variable named \""
                      << variableName.text
                      << "\" is already visible in this scope, this "
                         "declaration shadows it."
                      << (context.placesOfVariableDeclarations.count(
                              variableName.text)
                              ? " The previous declaration was at the line " +
                                    std::to_string(
                                        context.placesOfVariableDeclarations.at(
                                            variableName.text)) +
                                    "."
                              : "")
                      << std::endl;
          context.globalVariables[variableName.text] =
              context.globalVariablePointer;
          context.variableTypes[variableName.text] = childNode.text;
          context.placesOfVariableDeclarations[variableName.text] =
              variableName.lineNumber;
          if (variableName.text.back() == '[')
            context.globalVariablePointer +=
                (isPointerType(childNode.text)
                     ? 4
                     : basicDataTypeSizes.at(childNode.text)) *
                variableName.children[0]
                    .interpretAsACompileTimeIntegerConstant();
          else
            context.globalVariablePointer +=
                isPointerType(childNode.text)
                    ? 4
                    : basicDataTypeSizes.at(childNode.text);
          globalDeclarations +=
              "\t;;\"" + variableName.text +
              "\" is declared to be at the memory address " +
              std::to_string(context.globalVariables[variableName.text]) + "\n";
          auto iteratorOfTheAssignment = std::find_if(
              variableName.children.begin(), variableName.children.end(),
              [](TreeNode node) { return node.text == ":="; });
          if (iteratorOfTheAssignment !=
              variableName.children.end()) { // If there is an initial value
                                             // assigned to the global variable.
            auto assignment = iteratorOfTheAssignment->children[0];
            if (assignment.text == "{}") { // Array initializers.
              if (variableName.text.back() != '[') {
                std::cerr
                    << "Line " << assignment.lineNumber << ", Column "
                    << assignment.columnNumber
                    << ", Compiler error: Can't assign an array to a variable "
                       "that's not an array. Aborting the compilation!"
                    << std::endl;
                exit(1);
              }
              int address = context.globalVariables[variableName.text];
              for (auto field : assignment.children) {
                if (childNode.text == "Character")
                  globalDeclarations +=
                      "\t(data 0 (i32.const " + std::to_string(address) + ") " +
                      getCharVectorRepresentationOfCharacter(
                          field.interpretAsACompileTimeIntegerConstant()) +
                      ") ;;Hex of " +
                      std::to_string(
                          field.interpretAsACompileTimeIntegerConstant()) +
                      "\n";
                else if (childNode.text == "Integer16")
                  globalDeclarations +=
                      "\t(data 0 (i32.const " + std::to_string(address) + ") " +
                      getCharVectorRepresentationOfInteger16(
                          field.interpretAsACompileTimeIntegerConstant()) +
                      ") ;;Hex of " +
                      std::to_string(
                          field.interpretAsACompileTimeIntegerConstant()) +
                      "\n";
                else if (childNode.text == "Integer32")
                  globalDeclarations +=
                      "\t(data 0 (i32.const " + std::to_string(address) + ") " +
                      getCharVectorRepresentationOfInteger32(
                          field.interpretAsACompileTimeIntegerConstant()) +
                      ") ;;Hex of " +
                      std::to_string(
                          field.interpretAsACompileTimeIntegerConstant()) +
                      "\n";
                else if (childNode.text == "Integer64")
                  globalDeclarations +=
                      "\t(data 0 (i32.const " + std::to_string(address) + ") " +
                      getCharVectorRepresentationOfInteger64(
                          field.interpretAsACompileTimeDecimalConstant()) +
                      ") ;;Hex of " +
                      std::to_string(
                          field.interpretAsACompileTimeIntegerConstant()) +
                      "\n";
                else if (childNode.text == "Decimal32")
                  globalDeclarations +=
                      "\t(data 0 (i32.const " + std::to_string(address) + ") " +
                      getCharVectorRepresentationOfDecimal32(
                          field.interpretAsACompileTimeDecimalConstant()) +
                      ") ;;IEEE 754 hex of " +
                      std::to_string(
                          field.interpretAsACompileTimeDecimalConstant()) +
                      "\n";
                else if (childNode.text == "Decimal64")
                  globalDeclarations +=
                      "\t(data 0 (i32.const " + std::to_string(address) + ") " +
                      getCharVectorRepresentationOfDecimal64(
                          field.interpretAsACompileTimeDecimalConstant()) +
                      ") ;;IEEE 754 hex of " +
                      std::to_string(
                          field.interpretAsACompileTimeDecimalConstant()) +
                      "\n";
                else {
                  std::cerr << "Line " << field.lineNumber << ", Column "
                            << field.columnNumber
                            << ", Internal compiler error: Compiler got into a "
                               "forbidden state!"
                            << std::endl;
                  exit(1);
                }
                address += basicDataTypeSizes.at(childNode.text);
              }
            } else if (assignment.text.front() == '"') // String.
            {
              if (childNode.text != "CharacterPointer") {
                std::cerr << "Line " << iteratorOfTheAssignment->lineNumber
                          << ", Column "
                          << iteratorOfTheAssignment->columnNumber
                          << ", Compiler error: Strings can only be assigned "
                             "to variables of the type \"CharacterPointer\", I "
                             "hope it's clear why. Aborting the compilation!"
                          << std::endl;
                exit(1);
              }
              if (!context.globalVariables.count(assignment.text)) {
                std::cerr << "Line " << assignment.lineNumber << ", Column "
                          << assignment.columnNumber
                          << ", Internal compiler error: Memory for the string "
                          << assignment.text
                          << " hasn't been allocated before a pointer to it is "
                             "being compiled. Aborting the compilation before "
                             "std::map throws exception."
                          << std::endl;
                exit(1);
              }
              globalDeclarations +=
                  "\t(data 0 (i32.const " +
                  std::to_string(context.globalVariables[variableName.text]) +
                  ") " +
                  getCharVectorRepresentationOfInteger32(
                      context.globalVariables.at(assignment.text)) +
                  ") ;;Hex of " +
                  std::to_string(context.globalVariables.at(assignment.text)) +
                  "\n";
            } else { // Simple assignment.
              if (childNode.text == "Character")
                globalDeclarations +=
                    "\t(data 0 (i32.const " +
                    std::to_string(context.globalVariables[variableName.text]) +
                    ") " +
                    getCharVectorRepresentationOfCharacter(
                        assignment.interpretAsACompileTimeIntegerConstant()) +
                    ")\n";
              else if (childNode.text == "Integer16")
                globalDeclarations +=
                    "\t(data 0 (i32.const " +
                    std::to_string(context.globalVariables[variableName.text]) +
                    ") " +
                    getCharVectorRepresentationOfInteger16(
                        assignment.interpretAsACompileTimeIntegerConstant()) +
                    ")\n";
              else if (childNode.text == "Integer32")
                globalDeclarations +=
                    "\t(data 0 (i32.const " +
                    std::to_string(context.globalVariables[variableName.text]) +
                    ") " +
                    getCharVectorRepresentationOfInteger32(
                        assignment.interpretAsACompileTimeIntegerConstant()) +
                    ")\n";
              else if (childNode.text == "Integer64")
                globalDeclarations +=
                    "\t(data 0 (i32.const " +
                    std::to_string(context.globalVariables[variableName.text]) +
                    ") " +
                    getCharVectorRepresentationOfInteger64(
                        assignment.interpretAsACompileTimeIntegerConstant()) +
                    ")\n";
              else if (childNode.text == "Decimal32")
                globalDeclarations +=
                    "\t(data 0 (i32.const " +
                    std::to_string(context.globalVariables[variableName.text]) +
                    ") " +
                    getCharVectorRepresentationOfDecimal32(
                        assignment.interpretAsACompileTimeDecimalConstant()) +
                    ")\n";
              else if (childNode.text == "Decimal64")
                globalDeclarations +=
                    "\t(data 0 (i32.const " +
                    std::to_string(context.globalVariables[variableName.text]) +
                    ") " +
                    getCharVectorRepresentationOfDecimal64(
                        assignment.interpretAsACompileTimeDecimalConstant()) +
                    ")\n";
              else {
                std::cerr << "Line " << assignment.lineNumber << ", Column "
                          << assignment.columnNumber
                          << ", Internal compiler error: Compiler got into a "
                             "forbidden state!"
                          << std::endl;
                exit(1);
              }
            }
          }
        }
      } else if (childNode.text == "Function") {
        function functionDeclaration;
        CompilationContext contextOfThatFunction = context;
        if (childNode.children.size() != 3) {
          std::cerr << "Line " << childNode.lineNumber << ", Column "
                    << childNode.columnNumber
                    << ", Compiler error: The AST is corrupt, the \"Function\" "
                       "node has less than 3 children, quitting now (or else "
                       "the compiler will segfault)!"
                    << std::endl;
          exit(1);
        }
        functionDeclaration.name = childNode.children[0].text;
        functionDeclaration.returnType = childNode.children[1].children[0].text;
        for (TreeNode argument : childNode.children[0].children) {
          if (not(isPointerType(argument.text)) and
              not(basicDataTypeSizes.count(argument.text)) and
              not(context.structureSizes.count(argument.text))) {
            std::cerr << "Line " << argument.lineNumber << ", Column "
                      << argument.columnNumber << ", Compiler error: \""
                      << argument.text
                      << "\" does not appear to be a name of a type. Aborting "
                         "the compilation!"
                      << std::endl;
            exit(1);
          }
          functionDeclaration.argumentNames.push_back(
              argument.children.at(0).text);
          functionDeclaration.argumentTypes.push_back(argument.text);
          contextOfThatFunction.variableTypes[argument.children.at(0).text] =
              argument.text;
          contextOfThatFunction
              .placesOfVariableDeclarations[argument.children.at(0).text] =
              argument.children.at(0).lineNumber;
          contextOfThatFunction.localVariables[argument.children.at(0).text] =
              0;
          for (auto &pair : contextOfThatFunction
                                .localVariables) // The reference operator '&'
                                                 // is needed because... C++.
            pair.second +=
                isPointerType(argument.text) ? 4
                : basicDataTypeSizes.count(argument.text)
                    ? basicDataTypeSizes.at(argument.text)
                    : context.structureSizes.at(
                          argument.text); // Push all the variables
                                          // further back on the stack.
          contextOfThatFunction.stackSizeOfThisFunction +=
              isPointerType(argument.text) ? 4
              : basicDataTypeSizes.count(argument.text)
                  ? basicDataTypeSizes.at(argument.text)
                  : context.structureSizes.at(argument.text);
          contextOfThatFunction.stackSizeOfThisScope +=
              isPointerType(argument.text) ? 4
              : basicDataTypeSizes.count(argument.text)
                  ? basicDataTypeSizes.at(argument.text)
                  : context.structureSizes.at(argument.text);
          if (argument.children.at(0)
                  .children.size()) // If there is a default value.
            functionDeclaration.defaultArgumentValues.push_back(
                argument.children.at(0)
                    .children.at(0)
                    .children.at(0)
                    .interpretAsACompileTimeDecimalConstant());
          else
            functionDeclaration.defaultArgumentValues.push_back(0);
        }
        context.functions.push_back(functionDeclaration);
        contextOfThatFunction.functions.push_back(functionDeclaration);
        contextOfThatFunction.currentFunctionName = functionDeclaration.name;
        if (childNode.children.at(2).text == "External") {
          globalDeclarations +=
              "\t(import \"" +
              (compilation_target == "browser" ? std::string("JavaScript")
                                               : std::string("wasi_unstable")) +
              "\" \"" +
              functionDeclaration.name.substr(
                  0, functionDeclaration.name.size() - 1) +
              "\" (func $" +
              functionDeclaration.name.substr(
                  0, functionDeclaration.name.size() - 1) +
              " ";
          for (std::string argumentType : functionDeclaration.argumentTypes)
            globalDeclarations +=
                "(param " +
                (isPointerType(argumentType)
                     ? "i32"
                     : stringRepresentationOfWebAssemblyType.at(
                           mappingOfAECTypesToWebAssemblyTypes.at(
                               argumentType))) +
                ") ";
          if (functionDeclaration.returnType != "Nothing") {
            if (isPointerType(functionDeclaration.returnType))
              globalDeclarations += "(result i32)";
            else {
              if (!mappingOfAECTypesToWebAssemblyTypes.count(
                      functionDeclaration.returnType) and
                  !std::count_if(context.structures.begin(),
                                 context.structures.end(), [=](structure str) {
                                   return str.name ==
                                          functionDeclaration.returnType;
                                 })) {
                std::cerr << "Line " << childNode.children.at(1).lineNumber
                          << ", Column " << childNode.children[1].columnNumber
                          << ", Compiler error: Unknown type name \""
                          << functionDeclaration.returnType << "\"!"
                          << std::endl;
                exit(1);
              }
              globalDeclarations += "(result " +
                                    stringRepresentationOfWebAssemblyType.at(
                                        mappingOfAECTypesToWebAssemblyTypes.at(
                                            functionDeclaration.returnType)) +
                                    ")";
            }
          }
          globalDeclarations += "))\n";
        } else if (childNode.children.at(2).text == "Does") {
          if (!hasStackPointerBeenDeclared) {
            hasStackPointerBeenDeclared = true;
            globalDeclarations +=
                R"(
  (global $stack_pointer (mut i32) (i32.const 0)) ;;We can declare stack pointer only here because,
                                                  ;;in WebAssembly, "imports must occur before all
                                                  ;;non-import definitions".
  (memory 1)
  (export "memory" (memory 0)) ;;Copied from there, I am not sure how it actually works:
;;https://github.com/bytecodealliance/wasmtime/blob/main/docs/WASI-tutorial.md#web-assembly-text-example
)";
          }
          globalDeclarations += "\t(func $" +
                                functionDeclaration.name.substr(
                                    0, functionDeclaration.name.size() - 1) +
                                " ";
          for (std::string argumentType : functionDeclaration.argumentTypes)
            if (not(isPointerType(argumentType)) and
                mappingOfAECTypesToWebAssemblyTypes.count(argumentType) == 0) {
              std::cerr
                  << "Line " << childNode.children[1].lineNumber << ", Column "
                  << childNode.children[1].columnNumber
                  << R"(, Internal compiler error: Sorry about that, custom argument types (such as ")"
                  << argumentType
                  << R"(") other than pointers to structures are not yet supported. I have made a forum thread about that, if you would like to help me with that:
https://www.forum.hr/showthread.php?t=1243515
In the meantime, you can try modifying your program to use ")"
                  << argumentType << R"(Pointer" instead.)" << std::endl;
              exit(1);
            } else
              globalDeclarations +=
                  "(param " +
                  stringRepresentationOfWebAssemblyType.at(
                      mappingOfAECTypesToWebAssemblyTypes.at(
                          isPointerType(argumentType) ? "Integer32"
                                                      : argumentType)) +
                  ") ";
          if (functionDeclaration.returnType != "Nothing") {
            if (isPointerType(functionDeclaration.returnType))
              globalDeclarations +=
                  "(result i32) (local $return_value i32)"; // Will, of course,
                                                            // crash if somebody
                                                            // switches the
                                                            // JavaScript
                                                            // Virtual Machine
                                                            // to the 64-bit
                                                            // mode (so that
                                                            // pointers are no
                                                            // longer 32-bit
                                                            // integers), but
                                                            // let's hope that
                                                            // isn't going to
                                                            // happen.
            else {
              if (!mappingOfAECTypesToWebAssemblyTypes.count(
                      functionDeclaration.returnType) and
                  !std::count_if(context.structures.begin(),
                                 context.structures.end(), [=](structure str) {
                                   return str.name ==
                                          functionDeclaration.returnType;
                                 })) {
                std::cerr << "Line " << childNode.children[1].lineNumber
                          << ", Column " << childNode.children[1].columnNumber
                          << ", Compiler error: Unknown type name \""
                          << functionDeclaration.returnType << "\"!"
                          << std::endl;
                exit(1);
              }
              if (not(isPointerType(functionDeclaration.returnType)) and
                  mappingOfAECTypesToWebAssemblyTypes.count(
                      functionDeclaration.returnType) == 0) {
                std::cerr
                    << "Line " << childNode.children[1].lineNumber
                    << ", Column " << childNode.children[1].columnNumber
                    << R"(, Internal compiler error: Sorry about that, custom return types (such as ")"
                    << functionDeclaration.returnType
                    << R"(") other than pointers to structures are not yet supported. I have made a Reddit thread about that, if you would like to help me with that:
https://www.reddit.com/r/WebAssembly/comments/j24p4z/how_to_compile_returning_a_structure_from_a/
In the meantime, you can try modifying your program to use ")"
                    << functionDeclaration.returnType << R"(Pointer" instead.)"
                    << std::endl;
                exit(1);
              }
              globalDeclarations += "(result " +
                                    stringRepresentationOfWebAssemblyType.at(
                                        mappingOfAECTypesToWebAssemblyTypes.at(
                                            functionDeclaration.returnType)) +
                                    ") (local $return_value " +
                                    stringRepresentationOfWebAssemblyType.at(
                                        mappingOfAECTypesToWebAssemblyTypes.at(
                                            functionDeclaration.returnType)) +
                                    ")";
            }
          }
          globalDeclarations += "\n";
          globalDeclarations +=
              "\t\t(global.set $stack_pointer ;;Allocate the space for the "
              "arguments of that function on the system "
              "stack.\n\t\t\t(i32.add\n"
              "\t\t\t\t(global.get $stack_pointer)\n\t\t\t\t(i32.const " +
              std::to_string(contextOfThatFunction.stackSizeOfThisScope) +
              ")\n\t\t\t)\n\t\t)\n";
          for (unsigned int i = 0; i < functionDeclaration.argumentNames.size();
               i++) {
            if (functionDeclaration.argumentTypes[i] == "Character")
              globalDeclarations += "\t\t(i32.store8 ";
            else if (functionDeclaration.argumentTypes[i] == "Integer16")
              globalDeclarations += "\t\t(i32.store16 ";
            else if (functionDeclaration.argumentTypes[i] == "Integer32" or
                     isPointerType(functionDeclaration.argumentTypes[i]))
              globalDeclarations += "\t\t(i32.store ";
            else if (functionDeclaration.argumentTypes[i] == "Integer64")
              globalDeclarations += "\t\t(i64.store ";
            else if (functionDeclaration.argumentTypes[i] == "Decimal32")
              globalDeclarations += "\t\t(f32.store ";
            else if (functionDeclaration.argumentTypes[i] == "Decimal64")
              globalDeclarations += "\t\t(f64.store ";
            else {
              std::cerr << "Line " << childNode.children[2].lineNumber
                        << ", Column " << childNode.children[2].columnNumber
                        << ", Internal compiler error: The compiler got into a "
                           "forbidden state, quitting now so that the compiler "
                           "doesn't segfault!"
                        << std::endl;
              exit(1);
            }
            globalDeclarations +=
                "\n" +
                std::string(TreeNode(functionDeclaration.argumentNames[i],
                                     childNode.children[2].lineNumber,
                                     childNode.children[2].columnNumber)
                                .compileAPointer(contextOfThatFunction)
                                .indentBy(3)) +
                "\n\t\t\t(local.get " + std::to_string(i) + ")\n\t\t)\n";
          }
          globalDeclarations +=
              childNode.children[2].compile(contextOfThatFunction).indentBy(2) +
              "\n";
          if (functionDeclaration.returnType != "Nothing") {
            if (isPointerType(functionDeclaration.returnType))
              globalDeclarations += "\t\t(i32.const 0)\n";
            else
              globalDeclarations += "\t\t(" +
                                    stringRepresentationOfWebAssemblyType.at(
                                        mappingOfAECTypesToWebAssemblyTypes.at(
                                            functionDeclaration.returnType)) +
                                    ".const 0)\n";
          }
          globalDeclarations += "\t)\n\t(export \"" +
                                functionDeclaration.name.substr(
                                    0, functionDeclaration.name.size() - 1) +
                                "\" (func $" +
                                functionDeclaration.name.substr(
                                    0, functionDeclaration.name.size() - 1) +
                                "))\n";
        } else if (childNode.children[2].text != "Declared") {
          std::cerr << "Line " << childNode.lineNumber << ", Column "
                    << childNode.columnNumber
                    << ", Internal compiler error: The compiler got into a "
                       "forbidden state, quitting now before segfaulting!"
                    << std::endl;
          exit(1);
        }
      } else if (childNode.text == "asm(") {
        if (childNode.children.size() != 1) {
          std::cerr << "Line " << childNode.lineNumber << ", Column "
                    << childNode.columnNumber
                    << ", Compiler error: The \"asm(\" keyword doesn't"
                       " have exactly one argument, quitting now before"
                       " segfaulting!"
                    << std::endl;
          exit(1);
        }
        globalDeclarations +=
            AssemblyCode(convertInlineAssemblyToAssembly(childNode.children[0]))
                .indentBy(1) +
            "\n";
      } else if (childNode.text == "Structure") {
        if (childNode.children.size() != 2) {
          std::cerr
              << "Line " << childNode.lineNumber << ", Column "
              << childNode.columnNumber
              << ", Compiler error: Corrupt AST, the \"Structure\" node has "
              << childNode.children.size()
              << " children instead of 2. Quitting now!" << std::endl;
          exit(1);
        }
        if (!isValidVariableName(childNode.children[0].text) or
            childNode.children[0].text.back() == '[' or
            AECkeywords.count(childNode.children[0].text)) {
          std::cerr << "Line " << childNode.lineNumber << ", Column "
                    << childNode.columnNumber << ", Compiler error: The name \""
                    << childNode.children[0].text
                    << "\" is not a valid structure name." << std::endl;
          exit(1);
        }
        if (childNode.children[1].text != "Of") {
          std::cerr << "Line " << childNode.children[1].lineNumber
                    << ", Column " << childNode.children[1].columnNumber
                    << ", Compiler error: Expected \"Of\" instead of \""
                    << childNode.children[1].text << "\"." << std::endl;
          exit(1);
        }
        structure currentStructure;
        currentStructure.name = childNode.children[0].text;
        globalDeclarations += "\t;;Declaring a structure named \"" +
                              currentStructure.name + "\"...\n";
        for (TreeNode typeName : childNode.children[1].children) {
          if (!isPointerType(typeName.text) and
              !basicDataTypeSizes.count(typeName.text) and
              !std::count_if(
                  context.structures.begin(), context.structures.end(),
                  [=](structure str) { return str.name == typeName.text; })) {
            std::cerr << "Line " << typeName.lineNumber << ", Column "
                      << typeName.columnNumber
                      << ", Compiler error: The type named \"" << typeName.text
                      << "\" is not visible in this scope." << std::endl;
            exit(1);
          }
          for (TreeNode memberName : typeName.children) {
            if (!isValidVariableName(memberName.text) or
                AECkeywords.count(memberName.text)) {
              std::cerr << "Line " << memberName.lineNumber << ", Column "
                        << memberName.columnNumber << ", Compiler error: \""
                        << memberName.text
                        << "\" is not a valid structure member name."
                        << std::endl;
              exit(1);
            }
            currentStructure.memberNames.push_back(memberName.text);
            currentStructure.memberTypes[memberName.text] = typeName.text;
            currentStructure.memberOffsetInBytes[memberName.text] =
                currentStructure.sizeInBytes;
            currentStructure.arraySize[memberName.text] = 1;
            globalDeclarations +=
                "\t;;Declaring a structure member named \"" +
                currentStructure.name + "." + memberName.text +
                "\" of type \"" + typeName.text + "\" to be at offset " +
                std::to_string(currentStructure.sizeInBytes) + "...\n";
            if (memberName.text.back() == '[') // An array inside a structure...
            {
              if (memberName.children.size() != 1) {
                std::cerr << "Line " << memberName.lineNumber << ", Column "
                          << memberName.columnNumber
                          << ", Compiler error: Corrupt AST, the node \""
                          << memberName.text
                          << "\" should have 1 child, but it has "
                          << memberName.children.size()
                          << ". Aborting the compilation." << std::endl;
                exit(1);
              }
              currentStructure.sizeInBytes +=
                  memberName.children[0]
                      .interpretAsACompileTimeIntegerConstant() *
                  (isPointerType(typeName.text) ? 4
                   : basicDataTypeSizes.count(typeName.text)
                       ? basicDataTypeSizes.at(typeName.text)
                       : context.structureSizes.at(typeName.text));
              currentStructure.arraySize[memberName.text] =
                  memberName.children[0]
                      .interpretAsACompileTimeIntegerConstant();
            } else { // A member that's not an array...
              if (memberName.children.size() == 1 and
                  memberName.children[0].text == ":=")
                currentStructure.defaultValuesOfMembers[memberName.text] =
                    memberName.children[0]
                        .children[0]
                        .interpretAsACompileTimeDecimalConstant();
              else if (memberName.children.size())
                std::cerr << "Line " << memberName.children[0].lineNumber
                          << ", Column " << memberName.children[0].columnNumber
                          << ", Compiler error: Unexpected AST node \""
                          << memberName.children[0].text
                          << "\". The compilation will continue, but be warned "
                             "it might produce wrong code because of this."
                          << std::endl;
              currentStructure.sizeInBytes +=
                  isPointerType(typeName.text) ? 4
                  : basicDataTypeSizes.count(typeName.text)
                      ? basicDataTypeSizes.at(typeName.text)
                      : context.structureSizes.at(typeName.text);
            }
          }
        }
        context.structures.push_back(currentStructure);
        context.structureSizes[currentStructure.name] =
            currentStructure
                .sizeInBytes; // Will make the compiler significantly shorter.
        globalDeclarations += "\t;;Declaring the structure \"" +
                              currentStructure.name + "\" finished.\n";
      } else if (childNode.text == "InstantiateStructure") {
        if (childNode.children.size() != 1) {
          std::cerr << "Line " << childNode.lineNumber << ", Column "
                    << childNode.columnNumber
                    << ", Compiler error: Corrupt AST, the node "
                       "\"InstantiateStructure\" should have 1 child node, but "
                       "it has "
                    << childNode.children.size()
                    << ". Aborting the compilation!" << std::endl;
          exit(1);
        }
        TreeNode structureName = childNode.children.front();
        if (!std::count_if(context.structures.begin(), context.structures.end(),
                           [=](structure str) {
                             return str.name == structureName.text;
                           })) {
          std::cerr << "Line " << structureName.lineNumber << ", Column "
                    << structureName.columnNumber
                    << ", Compiler error: The structure named \""
                    << structureName.text
                    << "\" hasn't been declared in this scope. Aborting the "
                       "compilation!"
                    << std::endl;
          auto most_similar_structure_iterator = std::max_element(
              context.structures.begin(), context.structures.end(),
              [=](structure first_potentially_similar_structure,
                  structure second_potentially_similar_structure) {
                return longest_common_subsequence_length(
                           first_potentially_similar_structure.name,
                           structureName.text) <
                       longest_common_subsequence_length(
                           second_potentially_similar_structure.name,
                           structureName.text);
              });
          if (most_similar_structure_iterator != context.structures.end() &&
              longest_common_subsequence_length(
                  most_similar_structure_iterator->name, structureName.text)) {
            std::cerr << "By the way, maybe you meant \""
                      << most_similar_structure_iterator->name << "\"?"
                      << std::endl;
          }
          exit(1);
        }
        auto iteratorPointingToTheStructure = std::find_if(
            context.structures.begin(), context.structures.end(),
            [=](structure str) { return str.name == structureName.text; });
        for (TreeNode instanceName : structureName.children) {
          context.variableTypes[instanceName.text] = structureName.text;
          context.placesOfVariableDeclarations[instanceName.text] =
              instanceName.lineNumber;
          context.globalVariables[instanceName.text] =
              context.globalVariablePointer;
          globalDeclarations +=
              "\t;;Instance named \"" + instanceName.text +
              "\" of the structure type \"" + structureName.text +
              "\" is declared to be at the address " +
              std::to_string(context.globalVariablePointer) + "\n";
          if (instanceName.text.back() == '[') // Global array of structures...
          {
            if (instanceName.children.size() != 1) {
              std::cerr << "Line " << instanceName.lineNumber << ", Column "
                        << instanceName.columnNumber
                        << ", Compiler error: Corrupt AST, the node \""
                        << instanceName.text
                        << "\" should have 1 child, but it has "
                        << instanceName.children.size()
                        << ". Aborting the compilation." << std::endl;
              exit(1);
            }
            int arraySize = instanceName.children[0]
                                .interpretAsACompileTimeIntegerConstant();
            for (int i = 0; i < arraySize; i++) {
              AssemblyCode initialization = instantiateGlobalStructure(
                  *iteratorPointingToTheStructure,
                  context.globalVariablePointer, context.structures);
              initialization.indentBy(1);
              globalDeclarations += initialization;
              context.globalVariablePointer +=
                  iteratorPointingToTheStructure->sizeInBytes;
            }
          } else // Global structure...
          {
            AssemblyCode initialization = instantiateGlobalStructure(
                *iteratorPointingToTheStructure, context.globalVariablePointer,
                context.structures);
            initialization.indentBy(1);
            globalDeclarations += initialization;
            context.globalVariablePointer +=
                iteratorPointingToTheStructure->sizeInBytes;
          }
        }
      } else {
        std::cerr << "Line " << childNode.lineNumber << ", Column "
                  << childNode.columnNumber
                  << ", Compiler error: No rule for compiling the token \""
                  << childNode.text
                  << "\" in the global scope, aborting the compilation!"
                  << std::endl;
        exit(1);
      }
    }
    globalDeclarations += ")"; // End of the "module" S-expression.
    return AssemblyCode(globalDeclarations);
  }
  AssemblyCode compile(CompilationContext context) const override {
    context.stackSizeOfThisFunction = 0; // So that CLANG doesn't warn about not
                                         // using "context".
    std::cerr << "Internal compiler error: Some part of the compiler attempted "
                 "to recursively call the compiler from the beginning (leading "
                 "to an infinite loop), quitting now!"
              << std::endl;
    exit(1);
    return AssemblyCode(
        "()"); // So that the compiler doesn't throw a bunch of warnings about
               // the control reaching the end of a non-void function.
  }
  AssemblyCode compileAPointer(CompilationContext context) const override {
    context.stackSizeOfThisFunction = 0;
    std::cerr << "Internal compiler error: Some part of the compiler attempted "
                 "to get the assembly of the pointer of a module, which "
                 "doesn't make sense. Quitting now!"
              << std::endl;
    exit(1);
    return AssemblyCode("()");
  }
  int interpretAsACompileTimeIntegerConstant() const override {
    std::cerr
        << "Some part of the compiler attempted to convert a module to the "
           "compile time constant, which makes no sense. Quitting now!"
        << std::endl;
    exit(1);
    return 0;
  }
  double interpretAsACompileTimeDecimalConstant() const override {
    return interpretAsACompileTimeIntegerConstant();
  }
};
