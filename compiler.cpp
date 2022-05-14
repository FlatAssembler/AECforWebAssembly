/*
 * This is the core of the compiler, a part of the compiler that produces
 * assembly code. The code is somewhat ugly (lots of else-if-s), but
 * there doesn't appear to be an easy way to avoid that. Using inheritance
 * would lead to countless classes and code that's even harder to understand
 * and debug. Allegedly, compilers can be nicely structured using the
 * visitor behavioral pattern, but, honestly, I don't understand it and
 * I doubt it would help significantly.
 */

#include "bitManipulations.cpp"
#include "semanticAnalyzer.cpp"
#include <ciso646> // Necessary for Microsoft C++ Compiler.

AssemblyCode convertToInteger32(
    const TreeNode &
        node, // HappySkeptic suggested me to use constant references to avoid
              // Stack Overflow:
              // https://atheistforums.org/thread-63150-post-2054368.html#pid2054368
    const CompilationContext &context) {
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
        "(i32.wrap_i64\n" + std::string(originalCode.indentBy(1)) + "\n)", i32);
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
  throw std::runtime_error("Logic error in \"convertToInteger32\"");
  return AssemblyCode("()");
}

AssemblyCode convertToInteger64(const TreeNode &node,
                                const CompilationContext &context) {
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
  throw std::runtime_error("Logic error in \"convertToInteger64\"");
  return AssemblyCode("()");
}

AssemblyCode convertToDecimal32(const TreeNode &node,
                                const CompilationContext &context) {
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
  throw std::runtime_error("Logic error in \"convertToDecimal32\"");
  return AssemblyCode("()");
}

AssemblyCode convertToDecimal64(const TreeNode &node,
                                const CompilationContext &context) {
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
  throw std::runtime_error("Logic error in \"convertToDecimal64\"");
  return AssemblyCode("()");
}

AssemblyCode convertTo(const TreeNode &node, const std::string &type,
                       const CompilationContext &context) {
  if (type == "Character" or type == "Integer16" or type == "Integer32" or
      isPointerType(type)) // When, in JavaScript Virtual Machine, you can't
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

AssemblyCode TreeNode::compile(CompilationContext context) const {
  std::string typeOfTheCurrentNode = getType(context);
  AssemblyCode::AssemblyType returnType;
  if (isPointerType(typeOfTheCurrentNode))
    returnType = AssemblyCode::AssemblyType::i32;
  else {
    if (!mappingOfAECTypesToWebAssemblyTypes.count(typeOfTheCurrentNode)) {
      std::cerr
          << "Line " << lineNumber << ", Column " << columnNumber
          << ", Internal compiler error: The function \"getType\" returned \""
          << typeOfTheCurrentNode
          << "\", which is an invalid name of type. It's for the node with AST "
          << getLispExpression() << ". Aborting the compilation!" << std::endl;
      throw InvalidTypenameException();
    }
    returnType = mappingOfAECTypesToWebAssemblyTypes.at(typeOfTheCurrentNode);
  }
  auto iteratorPointingToFunctionBeingCompiled =
      std::find_if(context.functions.begin(), context.functions.end(),
                   [=](function someFunction) {
                     return someFunction.name == context.currentFunctionName;
                   });
  if (iteratorPointingToFunctionBeingCompiled == context.functions.end()) {
    std::cerr
        << "Line " << lineNumber << ", Column " << columnNumber
        << ", Internal compiler error: The \"compile(CompilationContext)\" "
           "function was called without setting the current function name, "
           "aborting compilation (or else the compiler will segfault)!"
        << std::endl;
    throw CorruptCompilationContextException(context);
  }
  function currentFunction = *iteratorPointingToFunctionBeingCompiled;
  std::string assembly;
#ifdef OUTPUT_DEBUG_COMMENTS_IN_ASSEMBLY_COMMENTS
  assembly += ";; Line " + std::to_string(lineNumber) + ", Column " +
              std::to_string(columnNumber) + ", token " + JSONifyString(text) +
              "\n";
#endif
  if (text == "Does" or text == "Then" or text == "Loop" or
      text == "Else") // Blocks of code are stored by the parser as child nodes
                      // of "Does", "Then", "Else" and "Loop".
  {
#ifdef OUTPUT_DEBUG_COMMENTS_IN_ASSEMBLY_COMMENTS
    std::string JSON = context.JSONify();
    std::string commentedJSON = ";;\t";
    for (size_t i = 0; i < JSON.length(); i++)
      if (JSON[i] == '\n')
        commentedJSON += "\n;;\t";
      else
        commentedJSON += JSON[i];
    assembly += ";;The JSON of the current compilation context is:\n" +
                commentedJSON + "\n";
#endif
    if (text != "Does")
      context.stackSizeOfThisScope =
          0; //"TreeRootNode" is supposed to set up the arguments in the scope
             // before passing the recursion onto the "Does" node.
    for (auto childNode : children) {
      if (childNode.text == "Nothing")
        continue;
      else if (basicDataTypeSizes.count(childNode.text) ||
               isPointerType(childNode.text)) {
        // Local variables declaration.
        for (TreeNode variableName : childNode.children) {
          if (context.variableTypes.count(variableName.text))
            std::cerr << "Line " << variableName.lineNumber << ", Column "
                      << variableName.columnNumber
                      << ", Compiler warning: Variable named \""
                      << variableName.text
                      << "\" is already visible in this scope (to be of type \""
                      << context.variableTypes.at(variableName.text) << "\""
                      << (context.placesOfVariableDeclarations.count(
                              variableName.text)
                              ? (", at the line " +
                                 std::to_string(
                                     context.placesOfVariableDeclarations.at(
                                         variableName.text)))
                              : "")
                      << "), this "
                         "declaration shadows it."
                      << std::endl;
          if (variableName.text.back() != '[') { // If it's not an array.
            context.localVariables[variableName.text] = 0;
            for (auto &pair : context.localVariables)
              pair.second += isPointerType(childNode.text)
                                 ? 4
                                 : basicDataTypeSizes.at(childNode.text);
            context.variableTypes[variableName.text] = childNode.text;
            context.placesOfVariableDeclarations[variableName.text] =
                variableName.lineNumber;
            context.stackSizeOfThisFunction +=
                isPointerType(childNode.text)
                    ? 4
                    : basicDataTypeSizes.at(childNode.text);
            context.stackSizeOfThisScope +=
                isPointerType(childNode.text)
                    ? 4
                    : basicDataTypeSizes.at(childNode.text);
            assembly +=
                "(global.set $stack_pointer\n\t(i32.add (global.get "
                "$stack_pointer) (i32.const " +
                std::to_string(isPointerType(childNode.text)
                                   ? 4
                                   : basicDataTypeSizes.at(childNode.text)) +
                ")) ;;Allocating the space for the local variable \"" +
                variableName.text + "\".\n)\n";
            if (variableName.children.size() and
                variableName.children[0].text ==
                    ":=") // Initial assignment to local variables.
            {
              TreeNode assignmentNode = variableName.children[0];
              assignmentNode.children.insert(assignmentNode.children.begin(),
                                             variableName);
              assembly += assignmentNode.compile(context) + "\n";
            }
          } else { // If that's a local array declaration.
            if (!variableName.children.size()) {
              std::cerr << "Line " << variableName.lineNumber << ", Column "
                        << variableName.columnNumber
                        << ", Compiler error: Corrupt AST, the array named \""
                        << variableName.text
                        << "\" has no child node indicating size." << std::endl;
              exit(1);
            }
            if (!basicDataTypeSizes.count(childNode.text) &&
                !isPointerType(childNode.text)) {
              std::cerr << "Line " << variableName.lineNumber << ", Column "
                        << variableName.columnNumber
                        << ", Compiler error: Corrupt AST, the variable is "
                           "supposed to be of the type \""
                        << childNode.text << "\", but no such type exists."
                        << std::endl;
              exit(1);
            }
            int arraySizeInBytes =
                (isPointerType(childNode.text)
                     ? 4
                     : basicDataTypeSizes.at(childNode.text)) *
                variableName.children[0]
                    .interpretAsACompileTimeIntegerConstant();
            context.localVariables[variableName.text] = 0;
            for (auto &pair : context.localVariables)
              pair.second += arraySizeInBytes;
            context.variableTypes[variableName.text] = childNode.text;
            context.placesOfVariableDeclarations[variableName.text] =
                childNode.lineNumber;
            context.stackSizeOfThisFunction += arraySizeInBytes;
            context.stackSizeOfThisScope += arraySizeInBytes;
            assembly += "(global.set $stack_pointer\n\t(i32.add (global.get "
                        "$stack_pointer) (i32.const " +
                        std::to_string(arraySizeInBytes) +
                        ")) ;;Allocating the space for the local array \"" +
                        variableName.text + "\".\n)\n";
            if (variableName.children.size() == 2 and
                variableName.children[1].text == ":=" and
                variableName.children[1].children[0].text ==
                    "{}") // Initial assignments of local arrays.
            {
              TreeNode initialisationList =
                  variableName.children[1].children[0];
              for (unsigned int i = 0; i < initialisationList.children.size();
                   i++) {
                TreeNode element = initialisationList.children[i];
                TreeNode assignmentNode(
                    ":=", variableName.children[1].lineNumber,
                    variableName.children[1].columnNumber);
                TreeNode whereToAssignTheElement(
                    variableName.text, variableName.lineNumber,
                    variableName
                        .columnNumber); // Damn, can you think up a language in
                                        // which writing stuff like this isn't
                                        // as tedious and error-prone as it is
                                        // in C++ or JavaScript? Maybe some
                                        // language in which you can switch
                                        // between a C-like syntax and a
                                        // Lisp-like syntax at will?
                whereToAssignTheElement.children.push_back(TreeNode(
                    std::to_string(i), variableName.children[0].lineNumber,
                    variableName.children[1].columnNumber));
                assignmentNode.children.push_back(whereToAssignTheElement);
                assignmentNode.children.push_back(element);
                assembly += assignmentNode.compile(context) + "\n";
              }
            }
          }
        }
#ifdef OUTPUT_DEBUG_COMMENTS_IN_ASSEMBLY_COMMENTS
        std::string JSON =
            JSONifyMapOfInts(context.localVariables) +
            "\n"; //"\n" is added to make the output more comprehensible in case
                  // the next assembly directive is also a comment, to put an
                  // empty line between the two comments.
        std::string commentedJSON = ";;\t";
        for (size_t i = 0; i < JSON.length(); i++)
          if (JSON[i] == '\n')
            commentedJSON += "\n;;\t";
          else
            commentedJSON += JSON[i];
        assembly += ";;The JSON of the new \"localVariables\" object in the "
                    "current compilation context is:\n" +
                    commentedJSON + "\n";
#endif
      } else if (childNode.text == "InstantiateStructure") {
        if (childNode.children.size() != 1) {
          std::cerr << "Line " << childNode.lineNumber << ", Column "
                    << childNode.columnNumber
                    << ", Compiler error: Corrupt AST, the node "
                       "\"InstantiateStructure\" should "
                       "have exactly one child, but it has "
                    << childNode.children.size()
                    << ". Aborting the compilation!" << std::endl;
          exit(1);
        }
        TreeNode nodeWithStructureName = childNode.children[0];
        if (!isValidVariableName(nodeWithStructureName.text) ||
            AECkeywords.count(nodeWithStructureName.text) ||
            nodeWithStructureName.text.back() == '[') {
          std::cerr << "Line " << nodeWithStructureName.lineNumber
                    << ", Column " << nodeWithStructureName.columnNumber
                    << ", Compiler error: The name \""
                    << nodeWithStructureName.text
                    << "\" is supposed to be a structure name, but it's not a "
                       "valid AEC name. Quitting now!"
                    << std::endl;
          exit(1);
        }
        auto iteratorPointingToTheStructure =
            std::find_if(context.structures.begin(), context.structures.end(),
                         [=](structure str) {
                           return str.name == nodeWithStructureName.text;
                         });
        if (iteratorPointingToTheStructure == context.structures.end() &&
            !context.structureSizes.count(nodeWithStructureName.text)) {
          std::cerr << "Line " << nodeWithStructureName.lineNumber
                    << ", Column " << nodeWithStructureName.columnNumber
                    << ", Compiler error: The structure named \""
                    << nodeWithStructureName.text
                    << "\" is not visible in the current scope. Quitting now!"
                    << std::endl;
          auto most_similar_structure_iterator = std::max_element(
              context.structures.begin(), context.structures.end(),
              [=](const structure first_potentially_similar_structure,
                  const structure second_potentially_similar_structure) {
                return
#ifndef USING_LEVENSTEIN_DISTANCE
                    longest_common_subsequence_length(
                        first_potentially_similar_structure.name,
                        nodeWithStructureName.text) <
                    longest_common_subsequence_length(
                        second_potentially_similar_structure.name,
                        nodeWithStructureName.text);
#else
                    Levenstein_distance(
                        first_potentially_similar_structure.name,
                        nodeWithStructureName.text) >
                    Levenstein_distance(
                        second_potentially_similar_structure.name,
                        nodeWithStructureName.text);
#endif
              });
          if (most_similar_structure_iterator != context.structures.end() &&
              longest_common_subsequence_length(
                  most_similar_structure_iterator->name,
                  nodeWithStructureName.text)) {
            std::cerr << "By the way, maybe you meant \""
                      << most_similar_structure_iterator->name << "\"?"
                      << std::endl;
          }
          exit(1);
        } else if (iteratorPointingToTheStructure == context.structures.end()) {
          std::cerr
              << "Line " << nodeWithStructureName.lineNumber << ", Column "
              << nodeWithStructureName.columnNumber
              << ", Internal compiler error: Some part of the compiler has "
                 "corrupted the compilation context, the structure named \""
              << nodeWithStructureName.text
              << "\" isn't visible, but its size is. Aborting the compilation!"
              << std::endl;
          throw CorruptCompilationContextException(context);
        } else if (!context.structureSizes.count(nodeWithStructureName.text)) {
          std::cerr
              << "Line " << nodeWithStructureName.lineNumber << ", Column "
              << nodeWithStructureName.columnNumber
              << ", Internal compiler error: Some part of the compiler has "
                 "corrupted the compilation context, the structure named \""
              << nodeWithStructureName.text
              << "\" is visible, but its size isn't. Aborting the compilation!"
              << std::endl;
          throw CorruptCompilationContextException(context);
        }
        for (TreeNode instanceName : nodeWithStructureName.children) {
          if (!isValidVariableName(instanceName.text) ||
              AECkeywords.count(instanceName.text)) {
            std::cerr << "Line " << instanceName.lineNumber << ", Column "
                      << instanceName.columnNumber
                      << ", Compiler error: The name \"" << instanceName.text
                      << "\" is supposed to be an instance name, but it's not "
                      << "a valid AEC name. Quitting now!" << std::endl;
            exit(1);
          }
          context.variableTypes[instanceName.text] =
              iteratorPointingToTheStructure->name;
          context.placesOfVariableDeclarations[instanceName.text] =
              instanceName.lineNumber;
          context.localVariables[instanceName.text] = 0;
          int arraySizeInBytes = 0, arraySizeInStructures = 0;
          if (instanceName.text.back() == '[' &&
              instanceName.children.size() == 1) {
            arraySizeInStructures =
                instanceName.children[0]
                    .interpretAsACompileTimeIntegerConstant();
            arraySizeInBytes = iteratorPointingToTheStructure->sizeInBytes *
                               arraySizeInStructures;

          } else {
            if (instanceName.text.back() == '[') {
              std::cerr
                  << "Line " << instanceName.lineNumber << ", Column "
                  << instanceName.columnNumber
                  << ", Compiler error: Corrupt AST, the node with text \""
                  << instanceName.text
                  << "\" is supposed to have 1 child, but it has "
                  << instanceName.children.size()
                  << ". Aborting the compilation!" << std::endl;
              exit(1);
            } else {
              arraySizeInBytes = iteratorPointingToTheStructure->sizeInBytes;
              arraySizeInStructures = 1;
            }
          }
          if (arraySizeInBytes < 1 || arraySizeInStructures < 1) {
            std::cerr << "Line " << instanceName.lineNumber << ", Column "
                      << instanceName.columnNumber
                      << ", Internal compiler error: Some part of the "
                         "compiler attempted "
                         "to compile an array with size less than 1, which "
                         "doesn't make sense. "
                         "Throwing an exception!"
                      << std::endl;
            throw std::runtime_error("Compiling an array of negative size!");
          }
          for (auto bitand pair : context.localVariables)
            pair.second += arraySizeInBytes;
          context.stackSizeOfThisFunction += arraySizeInBytes;
          context.stackSizeOfThisScope += arraySizeInBytes;
          assembly +=
              "(global.set $stack_pointer\n\t(i32.add (global.get "
              "$stack_pointer) (i32.const " +
              std::to_string(arraySizeInBytes) +
              ")) ;;Allocating the space for the local structure instance \"" +
              instanceName.text + "\".\n)\n";
          // Now we can set the default values. Let's set the members without
          // specified default values to 0, this may avoid some bugs.
          for (int i = 0; i < arraySizeInStructures; i++) {
            for (std::string memberName :
                 iteratorPointingToTheStructure->memberNames) {
              if (context.structureSizes.count(
                      iteratorPointingToTheStructure->memberTypes.at(
                          memberName))) { // Nested local strucrues.
                for (unsigned int k = 0;
                     k < iteratorPointingToTheStructure->arraySize[memberName];
                     k++) {
                  // Let's solve this using S-expressions...
                  CompilationContext fakeContext =
                      context; // To avoid the internal compiler error in the
                               // semantic analyzer.
                  fakeContext.stackSizeOfThisFunction = 0;
                  fakeContext.stackSizeOfThisScope = 0;
                  std::string nameOfTheInnerStructure;
                  do {
                    nameOfTheInnerStructure = "innerStructureInstantiated" +
                                              std::to_string(std::rand());
                  } while (
                      context.variableTypes.count(nameOfTheInnerStructure));
                  TreeNode innerStructureNameNode(
                      nameOfTheInnerStructure, // To silence useless warnings
                                               // about supposed variable
                                               // shadowing.
                      instanceName.lineNumber, instanceName.columnNumber);
                  TreeNode innerStructureTypeNode(
                      iteratorPointingToTheStructure->memberTypes.at(
                          memberName),
                      instanceName.lineNumber, instanceName.columnNumber);
                  innerStructureTypeNode.children.push_back(
                      innerStructureNameNode);
                  TreeNode instantiateStructureNode("InstantiateStructure",
                                                    instanceName.lineNumber,
                                                    instanceName.columnNumber);
                  instantiateStructureNode.children.push_back(
                      innerStructureTypeNode);
                  TreeNode instanceNameNode(instanceName.text,
                                            instanceName.lineNumber,
                                            instanceName.columnNumber);
                  instanceNameNode.children.push_back(TreeNode(
                      std::to_string(i), instanceName.lineNumber,
                      instanceName
                          .columnNumber)); // If we don't do this, the compiler
                                           // will crash if somebody tries to
                                           // instantiate local (inside a
                                           // function, not global) array of
                                           // nested structures.
                  TreeNode memberNameNode(memberName, instanceName.lineNumber,
                                          instanceName.columnNumber);
                  TreeNode arrayIndexNode(std::to_string(k),
                                          instanceName.lineNumber,
                                          instanceName.columnNumber);
                  memberNameNode.children.push_back(arrayIndexNode);
                  TreeNode dotOperator(".", instanceName.lineNumber,
                                       instanceName.columnNumber);
                  dotOperator.children = <% instanceNameNode, memberNameNode %>;
                  TreeNode assignmentOperator(":=", instanceName.lineNumber,
                                              instanceName.columnNumber);
                  assignmentOperator.children =
                      <% dotOperator, innerStructureNameNode %>;
                  TreeNode fakeInnerFunctionNode(
                      "Does", instanceName.lineNumber,
                      instanceName
                          .columnNumber); // Again, to work around the
                                          // hard-to-fix issue in the semantic
                                          // analyzer which causes internal
                                          // compiler errors.
                  fakeInnerFunctionNode.children =
                      <% instantiateStructureNode, assignmentOperator %>;
                  assembly += fakeInnerFunctionNode.compile(fakeContext) + "\n";
                }
                continue;
              }
              for (unsigned int k = 0;
                   k < iteratorPointingToTheStructure->arraySize[memberName];
                   k++) {
                // And now we need to do that daunting task of constructing an
                // S-expression in C++ again...
                TreeNode nodeRepresentingIndex(std::to_string(i),
                                               instanceName.lineNumber,
                                               instanceName.columnNumber);
                TreeNode nodeWithInstanceName(instanceName.text,
                                              instanceName.lineNumber,
                                              instanceName.columnNumber);
                nodeWithInstanceName.children.push_back(nodeRepresentingIndex);
                TreeNode dotOperator(".", instanceName.lineNumber,
                                     instanceName.columnNumber);
                TreeNode nodeWithMemberName(memberName, instanceName.lineNumber,
                                            instanceName.columnNumber);
                TreeNode nodeWithMemberArrayIndex(std::to_string(k),
                                                  instanceName.lineNumber,
                                                  instanceName.columnNumber);
                nodeWithMemberName.children.push_back(nodeWithMemberArrayIndex);
                dotOperator.children =
                    <% nodeWithInstanceName,
                     nodeWithMemberName %>; // Is this valid in standard C++? I
                                            // am not sure. GCC (at least as
                                            // early as 4.8.5) accepts that, and
                                            // so does CLANG 10, and so does
                                            // the C++ compiler that comes with
                                            // Visual Studio 2019.
                TreeNode assignmentOperator(":=", instanceName.lineNumber,
                                            instanceName.columnNumber);
                TreeNode nodeRepresentingDefaultValue(
                    !isPointerType(
                        iteratorPointingToTheStructure->memberTypes[memberName])
                        ? std::to_string(
                              iteratorPointingToTheStructure
                                  ->defaultValuesOfMembers
                                      [memberName]) // The operator[] of
                                                    // std::map returns 0 if we
                                                    // try to read a
                                                    // non-initialized field.
                                                    // That's different from the
                                                    // "at(key)" method, which
                                                    // throws an exception in
                                                    // that case.
                        : "CharacterPointer(",
                    instanceName.lineNumber, instanceName.columnNumber);
                TreeNode zeroNode("0", instanceName.lineNumber,
                                  instanceName.columnNumber);
                nodeRepresentingDefaultValue.children.push_back(zeroNode);
                assignmentOperator.children =
                    <% dotOperator, nodeRepresentingDefaultValue %>;
                // So, finally, now we can compile that S-expression.
                assembly += assignmentOperator.compile(context) + "\n";
              }
            }
          }
          if (instanceName.children.size() &&
              instanceName.children[0].text ==
                  ":=") // Initial assignment of local structures
          {
            TreeNode assignmentOperator(
                ":=", instanceName.children[0].lineNumber,
                instanceName.children[0].columnNumber);
            TreeNode leftHandSide(instanceName.text, instanceName.lineNumber,
                                  instanceName.columnNumber);
            assignmentOperator.children =
                <% leftHandSide, instanceName.children[0].children[0] %>;
            TreeNode nodeWithFakeDoesToken(
                "Does", instanceName.lineNumber,
                instanceName.columnNumber); // Let's, for now, avoid the
                                            // Internal Compiler Error in the
                                            // semantic analyzer this way.
            nodeWithFakeDoesToken.children.push_back(assignmentOperator);
            CompilationContext fakeContext = context;
            fakeContext.stackSizeOfThisFunction = 0;
            fakeContext.stackSizeOfThisScope = 0;
            assembly += nodeWithFakeDoesToken.compile(fakeContext) + "\n";
          }
        }
#ifdef OUTPUT_DEBUG_COMMENTS_IN_ASSEMBLY_COMMENTS
        std::string JSON = JSONifyMapOfInts(context.localVariables) + "\n";
        std::string commentedJSON = ";;\t";
        for (size_t i = 0; i < JSON.length(); i++)
          if (JSON[i] == '\n')
            commentedJSON += "\n;;\t";
          else
            commentedJSON += JSON[i];
        assembly += ";;The JSON of the new \"localVariables\" object in the "
                    "current compilation context is:\n" +
                    commentedJSON + "\n";
#endif
      } else if (childNode.text == ":=" &&
                 context.structureSizes.count(
                     childNode.getType(context))) { // Structure assignments.
        std::string structureName = childNode.getType(context);
        TreeNode fakeInnerFunctionNode(
            "Does", childNode.lineNumber,
            childNode
                .columnNumber); // Again, to avoid the internal compiler error
                                // in the semantic analyzer in case of nested
                                // structures. This isn't an elegant solution,
                                // but I can't think of any better.
        TreeNode instantiateStructureNode("InstantiateStructure",
                                          childNode.lineNumber,
                                          childNode.columnNumber);
        TreeNode structureNameNode(structureName, childNode.lineNumber,
                                   childNode.columnNumber);
        std::string nameOfTheTemporaryStructure;
        do {
          nameOfTheTemporaryStructure =
              "temporaryAssignmentStructure" + std::to_string(rand());
        } while (context.variableTypes.count(nameOfTheTemporaryStructure));
        TreeNode temporaryStructureNode(nameOfTheTemporaryStructure,
                                        childNode.lineNumber,
                                        childNode.columnNumber);
        structureNameNode.children.push_back(temporaryStructureNode);
        instantiateStructureNode.children.push_back(structureNameNode);
        fakeInnerFunctionNode.children.push_back(instantiateStructureNode);
        std::vector<TreeNode> assignmentsToTemporary;
        std::vector<TreeNode> assignmentsFromTemporary;
        auto iteratorPointingToTheStructure = std::find_if(
            context.structures.begin(), context.structures.end(),
            [=](structure str) { return str.name == structureName; });
        for (std::string memberName :
             iteratorPointingToTheStructure->memberNames)
          for (unsigned memberArrayIndex = 0;
               memberArrayIndex <
               iteratorPointingToTheStructure->arraySize.at(memberName);
               memberArrayIndex++) {
            // For every member of the structure, assume it's an array, so, for
            // every element of that array, construct an S-expression for
            // assigning the corresponding element of the righ-side structure to
            // it, and then compile that S-expression.
            TreeNode nodeWithMemberArrayIndex(std::to_string(memberArrayIndex),
                                              childNode.lineNumber,
                                              childNode.columnNumber);
            TreeNode nodeWithMemberName(memberName, childNode.lineNumber,
                                        childNode.columnNumber);
            nodeWithMemberName.children.push_back(nodeWithMemberArrayIndex);
            TreeNode leftDotOperator(".", childNode.lineNumber,
                                     childNode.columnNumber);
            leftDotOperator.children =
                <% temporaryStructureNode, nodeWithMemberName %>;
            TreeNode rightDotOperator(".", childNode.lineNumber,
                                      childNode.columnNumber);
            rightDotOperator.children =
                <% childNode.children[1], nodeWithMemberName %>;
            TreeNode assignmentOperator(":=", childNode.lineNumber,
                                        childNode.columnNumber);
            assignmentOperator.children =
                <% leftDotOperator, rightDotOperator %>;
            assignmentsToTemporary.push_back(assignmentOperator);
            leftDotOperator.children =
                <% childNode.children[0], nodeWithMemberName %>;
            rightDotOperator.children =
                <% temporaryStructureNode, nodeWithMemberName %>;
            assignmentOperator.children =
                <% leftDotOperator, rightDotOperator %>;
            assignmentsFromTemporary.push_back(assignmentOperator);
          }
        fakeInnerFunctionNode.children.insert(
            fakeInnerFunctionNode.children.end(),
            assignmentsToTemporary.begin(), assignmentsToTemporary.end());
        fakeInnerFunctionNode.children.insert(
            fakeInnerFunctionNode.children.end(),
            assignmentsFromTemporary.begin(), assignmentsFromTemporary.end());
        CompilationContext fakeContext = context;
        fakeContext.stackSizeOfThisScope = 0;
        fakeContext.stackSizeOfThisFunction = 0;
        assembly += fakeInnerFunctionNode.compile(fakeContext) + "\n";
      } else if ((childNode.text.size() == 2 and childNode.text[1] == '=') or
                 childNode.getType(context) == "Nothing")
        assembly += std::string(childNode.compile(context)) + "\n";
      else {
        std::cerr
            << "Line " << childNode.lineNumber << ", Column "
            << childNode.columnNumber
            << ", Compiler error: Sorry about that, but WebAssembly doesn't "
               "support expressions which aren't assigned to anything (the "
               "assembler complains if you write something like that). The "
               "compilation is going to be aborted now."
            << std::endl;
        std::exit(1);
      }
    }
    assembly += "(global.set $stack_pointer (i32.sub (global.get "
                "$stack_pointer) (i32.const " +
                std::to_string(context.stackSizeOfThisScope) + ")))";
  } else if (text.front() == '"')
    assembly += "(i32.const " +
                std::to_string(context.globalVariables.at(text)) +
                ") ;;Pointer to " + text;
  else if ((text == "asm(" or text == "asm_i32(" or text == "asm_i64(" or
            text == "asm_f32(" or text == "asm_f64(") and
           children.size() == 1)
    assembly += convertInlineAssemblyToAssembly(children.at(0));
  else if (text == "nan")
    assembly += "(f32.reinterpret_i32\n\t(i32.const -1) ;;IEE754 for "
                "not-a-number is 0xffffffff=-1.\n)";
  else if (context.variableTypes.count(text) or text == "." || text == "->") {
    if (typeOfTheCurrentNode == "Character")
      assembly +=
          "(i32.load8_s\n" + compileAPointer(context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Integer16")
      assembly +=
          "(i32.load16_s\n" + compileAPointer(context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Integer32" or
             isPointerType(typeOfTheCurrentNode))
      assembly += "(i32.load\n" + compileAPointer(context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Integer64")
      assembly += "(i64.load\n" + compileAPointer(context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Decimal32")
      assembly += "(f32.load\n" + compileAPointer(context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Decimal64")
      assembly += "(f64.load\n" + compileAPointer(context).indentBy(1) + "\n)";
    else {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Internal compiler error: Compiler got into a forbidden "
                   "state while compiling the token \""
                << text << "\", aborting the compilation!" << std::endl;
      throw InvalidTypenameException();
    }
  } else if (text == ":=" &&
             children[0].text ==
                 "?:") { // Left-hand-side conditional operators will be
                         // converted to if-else statements on the AST level...
    TreeNode firstAssignmentNode(":=", lineNumber, columnNumber);
    TreeNode secondAssignmentNode(firstAssignmentNode);
    firstAssignmentNode.children.push_back(children[0].children.at(1));
    firstAssignmentNode.children.push_back(children[1]);
    secondAssignmentNode.children.push_back(children[0].children.at(2));
    secondAssignmentNode.children.push_back(children[1]);
    TreeNode thenNode("Then", lineNumber, columnNumber);
    thenNode.children.push_back(firstAssignmentNode);
    TreeNode elseNode("Else", lineNumber, columnNumber);
    elseNode.children.push_back(secondAssignmentNode);
    TreeNode ifNode("If", lineNumber, columnNumber);
    ifNode.children = {children[0].children[0], thenNode, elseNode};
    assembly += ifNode.compile(context);
  } else if (text == ":=") {
    TreeNode rightSide;
    if (children.at(1).text ==
        ":=") {                   // Expressions such as "a:=b:=0" or similar.
      TreeNode tmp = children[1]; // In case the "compile" changes the TreeNode.
      assembly += children[1].compile(context) + "\n";
      rightSide = tmp.children[0];
      if (children[1].children[0].getLispExpression() !=
          rightSide.getLispExpression())
        std::cerr
            << "Line " << lineNumber << ", Column " << columnNumber
            << ", Internal compiler error: It's been detected that some part "
               "of the compiler has changed a part of the AST from "
            << rightSide.getLispExpression() << " to "
            << children[1].children[0].getLispExpression()
            << ". This should be impossible. The compilation will continue, "
               "but be warned it might produce wrong code because of this."
            << std::endl;
    } else
      rightSide = children[1];
    assembly += ";;Assigning " + rightSide.getLispExpression() + " to " +
                children[0].getLispExpression() + ".\n";
    if (typeOfTheCurrentNode == "Character")
      assembly += "(i32.store8\n" +
                  children[0].compileAPointer(context).indentBy(1) + "\n" +
                  convertToInteger32(rightSide, context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Integer16")
      assembly += "(i32.store16\n" +
                  children[0].compileAPointer(context).indentBy(1) + "\n" +
                  convertToInteger32(rightSide, context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Integer32" or
             isPointerType(typeOfTheCurrentNode))
      assembly += "(i32.store\n" +
                  children[0].compileAPointer(context).indentBy(1) + "\n" +
                  convertToInteger32(rightSide, context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Integer64")
      assembly += "(i64.store\n" +
                  children[0].compileAPointer(context).indentBy(1) + "\n" +
                  convertToInteger64(rightSide, context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Decimal32")
      assembly += "(f32.store\n" +
                  children[0].compileAPointer(context).indentBy(1) + "\n" +
                  convertToDecimal32(rightSide, context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Decimal64")
      assembly += "(f64.store\n" +
                  children[0].compileAPointer(context).indentBy(1) + "\n" +
                  convertToDecimal64(rightSide, context).indentBy(1) + "\n)";
    else {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Internal compiler error: The compiler got into a "
                   "forbidden state while compiling the token \""
                << text << "\", aborting the compilation!" << std::endl;
      throw InvalidTypenameException();
    }
    if (isPointerType(typeOfTheCurrentNode) and
        !isPointerType(rightSide.getType(context)))
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler warning: You are assigning a type \""
                << rightSide.getType(context)
                << "\" to a pointer, this is likely an error!" << std::endl;
    if (!isPointerType(typeOfTheCurrentNode) and
        isPointerType(rightSide.getType(context)))
      std::cerr
          << "Line " << lineNumber << ", Column " << columnNumber
          << ", Compiler warning: You are assigning a pointer to a type \""
          << typeOfTheCurrentNode << "\", this is likely an error!"
          << std::endl;
  } else if (text.size() == 2 and
             text[1] ==
                 '=') // The assignment operators "+=", "-=", "*=" and "/="...
  {
    if (children.at(0).text.back() ==
        '[') { // https://github.com/FlatAssembler/AECforWebAssembly/issues/15
               // https://discord.com/channels/530598289813536771/847014270922391563/934823770307301416
               /*
                * So, what follows is a bit of an ugly and incomprehensible code
                * that does the following. It rewrites code such as:
                * ```
                * array_name[random_generator()] += 1;
                * ```
                * to a code such as:
                * ```
                * Integer32 temporary_subscriptXXXX;
                * temporary_subscriptXXXX := random_generator();
                * array_name[temporary_subscriptXXXX] :=
                * 	array_name[temporary_subscriptXXXX] + 1;
                * ```
                * Notice that simply rewriting it as something like:
                * ```
                * array_name[random_generator()] := array_name[random_generator()]
                * 					+ 1;
                * ```
                * would not do the trick, because then the random generator would
                * have been called twice instead of once, each time presumably
                * giving a different result.
                * TODO: Refactor that code to be more legible. I have made a
                * 	StackExchange question asking how to do that, but it got
                * 	closed for some reason:
                * 	https://codereview.stackexchange.com/questions/273535/complicated-ast-manipulation-looks-ugly-in-c-how-to-refactor-it
                */
      TreeNode fakeInnerFunctionNode("Does", lineNumber, columnNumber);
      std::string subscriptName;
      do {
        subscriptName = "temporary_subscript" + std::to_string(rand());
      } while (context.variableTypes.count(subscriptName));
      TreeNode declarationOfSubscript("Integer32", lineNumber, columnNumber);
      declarationOfSubscript.children.push_back(
          TreeNode(subscriptName, lineNumber, columnNumber));
      fakeInnerFunctionNode.children.push_back(declarationOfSubscript);
      TreeNode subscriptAssignment(":=", lineNumber, columnNumber);
      subscriptAssignment.children.push_back(
          TreeNode(subscriptName, lineNumber, columnNumber));
      subscriptAssignment.children.push_back(children[0].children.at(0));
      fakeInnerFunctionNode.children.push_back(subscriptAssignment);
      TreeNode convertedToSimpleAssignment(":=", lineNumber, columnNumber);
      convertedToSimpleAssignment.children.push_back(TreeNode(
          children[0].text, children[0].lineNumber, children[0].columnNumber));
      convertedToSimpleAssignment.children[0].children.push_back(
          TreeNode(subscriptName, lineNumber, columnNumber));
      convertedToSimpleAssignment.children.push_back(
          TreeNode(text.substr(0, 1), lineNumber, columnNumber));
      convertedToSimpleAssignment.children[1].children.push_back(TreeNode(
          children[0].text, children[0].lineNumber, children[0].columnNumber));
      convertedToSimpleAssignment.children[1].children[0].children.push_back(
          TreeNode(subscriptName, lineNumber, columnNumber));
      convertedToSimpleAssignment.children[1].children.push_back(
          children.at(1));
      fakeInnerFunctionNode.children.push_back(convertedToSimpleAssignment);
      CompilationContext fakeContext = context;
      fakeContext.stackSizeOfThisScope = 0;
      fakeContext.stackSizeOfThisFunction = 0;
      assembly += fakeInnerFunctionNode.compile(fakeContext) + "\n";
    } else {
      TreeNode convertedToSimpleAssignment(":=", lineNumber, columnNumber);
      convertedToSimpleAssignment.children.push_back(children[0]);
      convertedToSimpleAssignment.children.push_back(*this);
      convertedToSimpleAssignment.children[1].text =
          convertedToSimpleAssignment.children[1].text.substr(0, 1);
      assembly += convertedToSimpleAssignment.compile(context);
    }
  } else if (text == "If") {
    if (children.size() < 2) {
      std::cerr
          << "Line " << lineNumber << ", Column " << columnNumber
          << ", Compiler error: Corrupt AST, the \"If\" node has less than 2 "
             "child nodes. Aborting the compilation (or else we will segfault)!"
          << std::endl;
      exit(1);
    }
    if (children[1].text != "Then") {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: Corrupt AST, the second child of the "
                   "\"If\" node isn't named \"Then\". Aborting the compilation "
                   "(or else we will probably segfault)!"
                << std::endl;
      exit(1);
    }
    if (children.size() >= 3 and children[2].text != "Else") {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: Corrupt AST, the third child of the "
                   "\"If\" node is not named \"Else\", aborting the "
                   "compilation (or else we will probably segfault)!"
                << std::endl;
      exit(1);
    }
    assembly += "(if\n"
                "\t;;Compiling the condition of if-branching: " +
                children[0].getLispExpression() + "\n" +
                convertToInteger32(children[0], context).indentBy(1) +
                "\n\t(then\n" + children[1].compile(context).indentBy(2) +
                "\n\t)" +
                ((children.size() == 3)
                     ? "\n\t(else\n" +
                           children[2].compile(context).indentBy(2) + "\n\t)\n)"
                     : AssemblyCode("\n)"));
  } else if (text == "While") {
    if (children.size() < 2 or children[1].text != "Loop") {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: Corrupt AST, aborting (or else we will "
                   "segfault)!"
                << std::endl;
      exit(1);
    }
    assembly += "(block\n\t(loop\n\t\t(br_if 1\n\t\t\t(i32.eqz\n"
                "\t\t\t;; Compiling the condition of the while-loop: " +
                children[0].getLispExpression() + "\n" +
                convertToInteger32(children[0], context).indentBy(4) +
                "\n\t\t\t)\n\t\t)\n" +
                children[1].compile(context).indentBy(2) +
                "\n\t\t(br 0)\n\t)\n)";
  } else if (isInteger(text))
    assembly += "(i64.const " + text + ")";
  else if (isDecimalNumber(text))
    assembly += "(f64.const " + text + ")";
  else if (text == "Return") {
    if (currentFunction.returnType != "Nothing") {
      if (children.empty()) {
        std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                  << ", Compiler error: It's not specified what to return from "
                     "a function that's supposed to return \""
                  << currentFunction.returnType
                  << "\", aborting the compilation (or else the compiler will "
                     "segfault)!"
                  << std::endl;
        exit(1);
      }
      TreeNode valueToBeReturned = children[0];
      if (valueToBeReturned.text == ":=") {
        TreeNode tmp = valueToBeReturned;
        assembly += valueToBeReturned.compile(context) + "\n";
        if (tmp.getLispExpression() != valueToBeReturned.getLispExpression())
          std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                    << ", Internal compiler error: Some part of the compiler "
                       "has changed a part of the AST from "
                    << tmp.getLispExpression() << " into "
                    << valueToBeReturned.getLispExpression()
                    << ", which shouldn't be possible. The compilation will "
                       "continue, but be warned it might produce wrong code "
                       "because of this."
                    << std::endl;
        valueToBeReturned = TreeNode(valueToBeReturned.children[0]);
        // https://stackoverflow.com/questions/63951270/using-default-copy-constructor-corrupts-a-tree-in-c
        // And I thought C++ didn't have such quirks, typical of
        // quickly-designed languages such as JavaScript. The way it behaves
        // makes some degree sense when you read the explanation, but it's
        // definitely what one would expect, and it leads to hard-to-find bugs.
        // Now I see the reason why there are no automatically generated copy
        // constructors in Java, C# and JavaScript (there, variables don't hold
        // objects but pointers to objects, and, as confusing as it can be at
        // first, it can't lead to this kind of problems).
        if (tmp.children[0].getLispExpression() !=
            valueToBeReturned.getLispExpression()) {
          std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                    << ", Internal compiler error: Some part of the compiler "
                       "has changed a part of the AST from "
                    << tmp.children[0].getLispExpression() << " into "
                    << valueToBeReturned.getLispExpression()
                    << ", which shouldn't be possible. The compilation will "
                       "continue, but be warned it might produce wrong code "
                       "because of this."
                    << std::endl;
          valueToBeReturned = tmp.children[0];
        }
      }
      assembly +=
          ";;Setting for returning: " + valueToBeReturned.getLispExpression() +
          "\n";
      assembly += "(local.set $return_value\n";
      assembly +=
          convertTo(valueToBeReturned, currentFunction.returnType, context)
              .indentBy(1) +
          "\n)\n";
    }
    assembly += "(global.set $stack_pointer (i32.sub (global.get "
                "$stack_pointer) (i32.const " +
                std::to_string(context.stackSizeOfThisFunction) +
                "))) ;;Cleaning up the system stack before returning.\n";
    assembly += "(return";
    if (currentFunction.returnType == "Nothing")
      assembly += ")";
    else
      assembly += " (local.get $return_value))";
  } else if (text == "+") {
    std::vector<TreeNode> children =
        this->children; // To make sure we don't change the AST during
                        // compiling.
    if (isPointerType(children.at(1).getType(context)))
      std::iter_swap(children.begin(), children.begin() + 1);
    std::string firstType = children[0].getType(context);
    std::string secondType = children[1].getType(context);
    if (isPointerType(
            firstType)) // Multiply the second operand by the numbers of
                        // bytes the data type that the pointer points to
                        // takes. That is, be compatible with pointers in
                        // C and C++, rather than with pointers in
                        // Assembly (which allows unaligned access).
    {
      std::string typeName =
          firstType.substr(0, firstType.size() - std::string("Pointer").size());
      if (!basicDataTypeSizes.count(typeName) && !isPointerType(typeName) &&
          !context.structureSizes.count(typeName)) {
        std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                  << ", Internal compiler error: The size of the type \""
                  << firstType.substr(0, firstType.size() -
                                             std::string("Pointer").size())
                  << "\" is not specified in the compilation context. Aborting "
                     "before we segfault."
                  << std::endl;
        throw CorruptCompilationContextException(context);
      }
      TreeNode sizeOfNode("SizeOf(", lineNumber, columnNumber);
      sizeOfNode.children.push_back(
          TreeNode(typeName, lineNumber, columnNumber));
      assembly +=
          "(i32.add\n" + std::string(children[0].compile(context).indentBy(1)) +
          "\n\t(i32.mul\n" + sizeOfNode.compile(context).indentBy(2) + "\n" +
          convertToInteger32(children[1], context).indentBy(2) + "\n\t)\n)";
    } else
      assembly +=
          "(" + stringRepresentationOfWebAssemblyType.at(returnType) +
          ".add\n" +
          convertTo(children[0], typeOfTheCurrentNode, context).indentBy(1) +
          "\n" +
          convertTo(children[1], typeOfTheCurrentNode, context).indentBy(1) +
          "\n)";
  } else if (text == "-") {
    std::string firstType = children.at(0).getType(context);
    std::string secondType = children.at(1).getType(context);
    if (!isPointerType(firstType) and isPointerType(secondType)) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: What exactly does it mean to subtract a "
                   "pointer from a number? Aborting the compilation!"
                << std::endl;
      exit(1);
    } else if (isPointerType(firstType) and
               isPointerType(secondType)) // Subtract two pointers as if they
                                          // were two Integer32s.
      assembly += "(i32.sub\n" + children[0].compile(context).indentBy(1) +
                  "\n" + children[1].compile(context).indentBy(1) + "\n)";
    else if (isPointerType(firstType) and !isPointerType(secondType)) {
      std::string typeName =
          firstType.substr(0, firstType.size() - std::string("Pointer").size());
      if (!basicDataTypeSizes.count(typeName) && !isPointerType(typeName) &&
          !context.structureSizes.count(typeName)) {
        std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                  << ", Internal compiler error: The size of the type \""
                  << firstType.substr(0, firstType.size() -
                                             std::string("Pointer").size())
                  << "\" is not specified in the compilation context. Aborting "
                     "before we segfault."
                  << std::endl;
        throw CorruptCompilationContextException(context);
      }
      TreeNode sizeOfNode("SizeOf(", lineNumber, columnNumber);
      sizeOfNode.children.push_back(
          TreeNode(typeName, lineNumber, columnNumber));
      assembly += "(i32.sub\n" + children[0].compile(context).indentBy(1) +
                  "\n\t(i32.mul\n" + sizeOfNode.compile(context).indentBy(2) +
                  "\n" + convertToInteger32(children[1], context).indentBy(2) +
                  "\n\t)\n)";
    } else
      assembly +=
          "(" + stringRepresentationOfWebAssemblyType.at(returnType) +
          ".sub\n" +
          convertTo(children[0], typeOfTheCurrentNode, context).indentBy(1) +
          "\n" +
          convertTo(children[1], typeOfTheCurrentNode, context).indentBy(1) +
          "\n)";
  } else if (text == "*")
    assembly +=
        "(" + stringRepresentationOfWebAssemblyType.at(returnType) + ".mul\n" +
        convertTo(children[0], typeOfTheCurrentNode, context).indentBy(1) +
        "\n" +
        convertTo(children[1], typeOfTheCurrentNode, context).indentBy(1) +
        "\n)";
  else if (text == "/") {
    if (returnType == AssemblyCode::AssemblyType::i32 or
        returnType == AssemblyCode::AssemblyType::i64)
      assembly +=
          "(" + stringRepresentationOfWebAssemblyType.at(returnType) +
          ".div_s\n" +
          convertTo(children[0], typeOfTheCurrentNode, context).indentBy(1) +
          "\n" +
          convertTo(children[1], typeOfTheCurrentNode, context).indentBy(1) +
          "\n)";
    else
      assembly +=
          "(" + stringRepresentationOfWebAssemblyType.at(returnType) +
          ".div\n" +
          convertTo(children[0], typeOfTheCurrentNode, context).indentBy(1) +
          "\n" +
          convertTo(children[1], typeOfTheCurrentNode, context).indentBy(1) +
          "\n)";
  } else if (text == "<" or text == ">") {
    std::string firstType = children[0].getType(context);
    std::string secondType = children[1].getType(context);
    std::string strongerType;
    if (isPointerType(firstType) and isPointerType(secondType))
      strongerType =
          "Integer32"; // Let's allow people to shoot themselves in the foot by
                       // comparing pointers of different types.
    else
      strongerType =
          getStrongerType(lineNumber, columnNumber, firstType, secondType);
    AssemblyCode::AssemblyType assemblyType =
        mappingOfAECTypesToWebAssemblyTypes.at(strongerType);
    if (assemblyType == AssemblyCode::AssemblyType::i32 or
        assemblyType == AssemblyCode::AssemblyType::i64)
      assembly +=
          "(" + stringRepresentationOfWebAssemblyType.at(assemblyType) +
          (text == "<" ? ".lt_s\n" : ".gt_s\n") +
          convertTo(children[0], strongerType, context).indentBy(1) + "\n" +
          convertTo(children[1], strongerType, context).indentBy(1) + "\n)";
    else // If we are comparing decimal (floating-point) numbers, rather than
         // integers...
      assembly +=
          "(" + stringRepresentationOfWebAssemblyType.at(assemblyType) +
          (text == "<" ? ".lt\n" : ".gt\n") +
          convertTo(children[0], strongerType, context).indentBy(1) + "\n" +
          convertTo(children[1], strongerType, context).indentBy(1) + "\n)";
  } else if (text == "=" &&
             context.structureSizes.count(children.at(0).getType(context))) {
    if (children[0].getType(context) != children.at(1).getType(context))
      std::cerr
          << "Line " << lineNumber << ", Column " << columnNumber
          << ", Compiler warning: Comparing structures of different types: \""
          << children[0].getType(context) << "\" and \""
          << children[1].getType(context) << "\"!" << std::endl;
    auto iteratorPointingToTheStructureWeAreComparing =
        std::find_if(context.structures.begin(), context.structures.end(),
                     [=](const structure &str) {
                       return str.name == children[0].getType(context);
                     });
    if (iteratorPointingToTheStructureWeAreComparing ==
        context.structures.end()) {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Internal compiler error: The structure named \""
                << children[0].getType(context)
                << "\" is present in \"structureSizes\" array, but not in "
                   "\"structures\" array!"
                << std::endl;
      throw CorruptCompilationContextException(context);
    }
    TreeNode structureComparisonConvertedIntoManyAndNodes("1", lineNumber,
                                                          columnNumber);
    for (std::string structureMemberName :
         iteratorPointingToTheStructureWeAreComparing->memberNames)
      for (int arrayIndex = 0;
           arrayIndex <
           (int)iteratorPointingToTheStructureWeAreComparing->arraySize.at(
               structureMemberName);
           arrayIndex++) {
        TreeNode leftHandSide(".", lineNumber, columnNumber),
            rightHandSide(".", lineNumber, columnNumber);
        TreeNode nodeWithMemberName(structureMemberName, lineNumber,
                                    columnNumber);
        nodeWithMemberName.children.push_back(
            TreeNode(std::to_string(arrayIndex), lineNumber, columnNumber));
        leftHandSide.children = {children[0], nodeWithMemberName};
        rightHandSide.children = {children.at(1), nodeWithMemberName};
        TreeNode comparisonNode("=", lineNumber, columnNumber);
        comparisonNode.children = {leftHandSide, rightHandSide};
        TreeNode andNode("and", lineNumber, columnNumber);
        andNode.children = {comparisonNode,
                            structureComparisonConvertedIntoManyAndNodes};
        structureComparisonConvertedIntoManyAndNodes = andNode;
      }
    assembly += structureComparisonConvertedIntoManyAndNodes.compile(context);
  } else if (text == "=") {
    std::string firstType = children.at(0).getType(context);
    std::string secondType = children.at(1).getType(context);
    std::string strongerType;
    if (isPointerType(firstType) and isPointerType(secondType))
      strongerType = "Integer32";
    else
      strongerType =
          getStrongerType(lineNumber, columnNumber, firstType, secondType);
    AssemblyCode::AssemblyType assemblyType =
        mappingOfAECTypesToWebAssemblyTypes.at(strongerType);
    assembly +=
        "(" + stringRepresentationOfWebAssemblyType.at(assemblyType) + ".eq\n" +
        convertTo(children[0], strongerType, context).indentBy(1) + "\n" +
        convertTo(children[1], strongerType, context).indentBy(1) + "\n)";
  } else if (text == "?:")
    assembly +=
        "(if (result " + stringRepresentationOfWebAssemblyType.at(returnType) +
        ")\n" + convertToInteger32(children[0], context).indentBy(1) +
        "\n\t(then\n" +
        convertTo(children[1], typeOfTheCurrentNode, context).indentBy(2) +
        "\n\t)\n\t(else\n" +
        convertTo(children[2], typeOfTheCurrentNode, context).indentBy(2) +
        "\n\t)\n)";
  else if (text == "not(")
    assembly += "(i32.eqz\n" +
                convertToInteger32(children.at(0), context).indentBy(1) + "\n)";
  else if (text == "mod(")
    assembly +=
        "(" + stringRepresentationOfWebAssemblyType.at(returnType) +
        ".rem_s\n" +
        convertTo(children.at(0), typeOfTheCurrentNode, context).indentBy(1) +
        "\n" +
        convertTo(children.at(1), typeOfTheCurrentNode, context).indentBy(1) +
        "\n)";
  else if (text == "invertBits(")
    assembly += "(i32.xor (i32.const -1)\n" +
                convertToInteger32(children.at(0), context).indentBy(1) + "\n)";
  else if (text == "and")
    assembly += "(i32.and\n" +
                convertToInteger32(children.at(0), context).indentBy(1) + "\n" +
                convertToInteger32(children.at(1), context).indentBy(1) + "\n)";
  else if (text == "or")
    assembly += "(i32.or\n" +
                convertToInteger32(children.at(0), context).indentBy(1) + "\n" +
                convertToInteger32(children.at(1), context).indentBy(1) + "\n)";
  else if (text.back() == '(' and
           (basicDataTypeSizes.count(text.substr(0, text.size() - 1)) or
            isPointerType(
                text.substr(0, text.size() - 1)))) // The casting operator.
    assembly +=
        convertTo(children.at(0), text.substr(0, text.size() - 1), context);
  else if (std::count_if(context.functions.begin(), context.functions.end(),
                         [=](function someFunction) {
                           return someFunction.name == text;
                         })) {
    function functionToBeCalled = *find_if(
        context.functions.begin(), context.functions.end(),
        [=](function someFunction) { return someFunction.name == text; });
#ifdef ENABLE_NAMED_FUNCTION_ARGUMENTS
    bool areArgumentsNamed = false;
    for (unsigned int i = 0; i < children.size(); i++)
      if (children.at(i).text == ":=")
        areArgumentsNamed = true;
    if (areArgumentsNamed) {
      std::vector<bool> isArgumentSet(functionToBeCalled.argumentNames.size());
      TreeNode formWithoutNamedArguments(text, lineNumber, columnNumber);
      for (unsigned int i = 0; i < functionToBeCalled.argumentTypes.size(); i++)
        formWithoutNamedArguments.children.push_back(TreeNode(
            std::to_string(functionToBeCalled.defaultArgumentValues[i]),
            lineNumber, columnNumber));
      for (unsigned int i = 0; i < children.size(); i++) {
        if (children.at(i).text != ":=") // Argument is not named
        {
          if (i >= functionToBeCalled.argumentTypes.size()) {
            std::cerr << "Line " << children[i].lineNumber << ", Column "
                      << children[i].columnNumber
                      << ", Compiler error: Too many arguments passed to the "
                         "function \""
                      << text << "\" (it expects "
                      << functionToBeCalled.argumentTypes.size()
                      << " arguments). Aborting the compilation (or else the "
                         "compiler "
                         "will segfault)!"
                      << std::endl;
            exit(1);
          }
          if (isArgumentSet.at(i))
            std::cerr
                << "Line " << children[i].lineNumber << ", Column "
                << children[i].columnNumber
                << ", Compiler warning: The argument number #" << i + 1
                << ", named \"" << functionToBeCalled.argumentNames.at(i)
                << "\", has been set multiple times, each time overwriting the "
                   "previous value. This is, in all likelihood, an error. "
                   "Please check this call to function \""
                << functionToBeCalled.name << "\"!" << std::endl;
          formWithoutNamedArguments.children[i] = children.at(i);
          isArgumentSet[i] = true;
        } else { // If the argument of the function we are calling is named.
          int indexOfTheNamedArgument = 0;
          while (indexOfTheNamedArgument <
                 int(functionToBeCalled.argumentNames
                         .size())) { // If you do not explicitly cast to "int"
                                     // here, GCC issues a warning.
            if (functionToBeCalled.argumentNames.at(indexOfTheNamedArgument) ==
                children.at(i).children.at(0).text)
              break;
            indexOfTheNamedArgument++;
          }
          if (indexOfTheNamedArgument ==
              int(functionToBeCalled.argumentNames.size())) {
            std::cerr << "Line " << children.at(i).children[0].lineNumber
                      << ", Column " << children[i].children[0].columnNumber
                      << ", Compiler error: There is no argument named \""
                      << children[i].children.at(0).text
                      << "\" in the function named \""
                      << functionToBeCalled.name << "\".\n";
            std::cerr << "The arguments to that function are called: ";
            for (unsigned int i = 0;
                 i < functionToBeCalled.argumentNames.size(); i++)
              if (i == functionToBeCalled.argumentNames.size() - 1)
                std::cerr << "\"" << functionToBeCalled.argumentNames[i]
                          << "\".\n";
              else if (i == functionToBeCalled.argumentNames.size() - 2)
                std::cerr << "\"" << functionToBeCalled.argumentNames[i]
                          << "\" and ";
              else
                std::cerr << "\"" << functionToBeCalled.argumentNames[i]
                          << "\",";
            std::cerr << "Quitting now!" << std::endl;
            exit(1);
          }
          if (isArgumentSet.at(indexOfTheNamedArgument))
            std::cerr
                << "Line " << children[i].lineNumber << ", Column "
                << children[i].columnNumber
                << ", Compiler warning: The argument number #"
                << indexOfTheNamedArgument + 1 << ", named \""
                << functionToBeCalled.argumentNames.at(indexOfTheNamedArgument)
                << "\", has been set multiple times, each time overwriting the "
                   "previous value. This is, in all likelihood, an error. "
                   "Please check this call to function \""
                << functionToBeCalled.name << "\"!" << std::endl;
          formWithoutNamedArguments.children[indexOfTheNamedArgument] =
              children[i].children[1];
          isArgumentSet[indexOfTheNamedArgument] = true;
        }
      }
      assembly += ";; Compiling function with named arguments as if it were: " +
                  formWithoutNamedArguments.getLispExpression() + "\n";
      assembly += formWithoutNamedArguments.compile(context);
    } else { // If arguments are not named.
#endif
      assembly += "(call $" + text.substr(0, text.size() - 1) + "\n";
      for (unsigned int i = 0; i < children.size(); i++) {
        if (i >= functionToBeCalled.argumentTypes.size()) {
          std::cerr
              << "Line " << children[i].lineNumber << ", Column "
              << children[i].columnNumber
              << ", Compiler error: Too many arguments passed to the function "
                 "\""
              << text << "\" (it expects "
              << functionToBeCalled.argumentTypes.size()
              << " arguments). Aborting the compilation (or else the compiler "
                 "will segfault)!"
              << std::endl;
          exit(1);
        }
        assembly +=
            ";;Compiling the function argument " +
            children[i].getLispExpression() + "\n" +
            convertTo(children[i], functionToBeCalled.argumentTypes[i], context)
                .indentBy(1) +
            "\n";
      }
      for (unsigned int i = children.size();
           i < functionToBeCalled.defaultArgumentValues.size(); i++) {
        if (!functionToBeCalled.defaultArgumentValues[i])
          std::cerr
              << "Line " << lineNumber << ", Column " << columnNumber
              << ", Compiler warning: The argument #" << i + 1 << " (called \""
              << functionToBeCalled.argumentNames[i]
              << "\") of the function named \"" << text
              << "\" isn't being passed to that function, nor does it have "
                 "some "
                 "default value. Your program will very likely crash because "
                 "of "
                 "that!"
              << std::endl; // JavaScript doesn't even warn about such errors,
                            // while C++ refuses to compile a program then. I
                            // suppose I should take a middle ground here.
        assembly +=
            ";;Compiling the default value of the function argument \"" +
            functionToBeCalled.argumentNames[i] + "\", that is " +
            std::to_string(functionToBeCalled.defaultArgumentValues[i]) +
            ".\n" +
            convertTo(TreeNode(std::to_string(
                                   functionToBeCalled.defaultArgumentValues[i]),
                               lineNumber, columnNumber),
                      functionToBeCalled.argumentTypes[i], context)
                .indentBy(1);
      }
      assembly += ")";
#ifdef ENABLE_NAMED_FUNCTION_ARGUMENTS
    }
#endif
  } else if (text == "ValueAt(") {
    if (typeOfTheCurrentNode == "Character")
      assembly +=
          "(i32.load8_s\n" + children[0].compile(context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Integer16")
      assembly +=
          "(i32.load16_s\n" + children[0].compile(context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Integer32" or
             isPointerType(typeOfTheCurrentNode))
      assembly +=
          "(i32.load\n" + children[0].compile(context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Integer64")
      assembly +=
          "(i64.load\n" + children[0].compile(context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Decimal32")
      assembly +=
          "(f32.load\n" + children[0].compile(context).indentBy(1) + "\n)";
    else if (typeOfTheCurrentNode == "Decimal64")
      assembly +=
          "(f64.load\n" + children[0].compile(context).indentBy(1) + "\n)";
    else {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Internal compiler error: The compiler got into a "
                   "forbidden state while compiling \"ValueAt\", aborting!"
                << std::endl;
      throw InvalidTypenameException();
    }
  } else if (text == "AddressOf(")
    return children[0].compileAPointer(context);
  else if (text == "SizeOf(") {
    std::string typeName = children[0].text;
    int size = 0;
    if (isPointerType(typeName))
      size = 4;
    else if (basicDataTypeSizes.count(typeName))
      size = basicDataTypeSizes.at(typeName);
    else if (context.structureSizes.count(typeName))
      size = context.structureSizes.at(typeName);
    else {
      std::cerr << "Line " << lineNumber << ", Column " << columnNumber
                << ", Compiler error: \"" << typeName
                << "\" doesn't seem to be a data type. Quitting now."
                << std::endl;
      exit(1);
    }
    assembly += "(i32.const " + std::to_string(size) + ") ;;Size of \"" +
                typeName + "\"";
  } else {
    std::cerr << "Line " << lineNumber << ", Column " << columnNumber
              << ", Compiler error: No rule to compile the token \"" << text
              << "\", quitting now!" << std::endl;
    exit(1);
  }
  return AssemblyCode(assembly, returnType);
}

AssemblyCode
TreeNode::compileAPointer(const CompilationContext &context) const {
  if (text == "ValueAt(")
    return children[0].compile(context);
  if (context.localVariables.count(text) and text.back() != '[')
    return AssemblyCode(
        "(i32.sub\n\t(global.get $stack_pointer)\n\t(i32.const " +
            std::to_string(context.localVariables.at(text)) + ") ;;" + text +
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
            std::to_string(context.localVariables.at(text)) + ") ;;" + text +
            "\n\t)\n\t(i32.mul\n\t\t(i32.const " +
            std::to_string(basicDataTypeSizes.count(getType(context))
                               ? basicDataTypeSizes.at(getType(context))
                               : context.structureSizes.at(getType(context))) +
            ")\n" +
            std::string(convertToInteger32(children[0], context).indentBy(2)) +
            "\n\t)\n)",
        AssemblyCode::AssemblyType::i32);
  }
  if (context.globalVariables.count(text) and text.back() != '[')
    return AssemblyCode("(i32.const " +
                            std::to_string(context.globalVariables.at(text)) +
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
            std::to_string(context.globalVariables.at(text)) + ") ;;" + text +
            "\n\t(i32.mul\n\t\t(i32.const " +
            std::to_string(basicDataTypeSizes.count(getType(context))
                               ? basicDataTypeSizes.at(getType(context))
                               : context.structureSizes.at(getType(context))) +
            ")\n" +
            std::string(convertToInteger32(children[0], context).indentBy(3)) +
            "\n\t)\n)",
        AssemblyCode::AssemblyType::i32);
  }
  if (text == ".") { // The dot-operator for accessing structure members.
    std::string structureName = children[0].getType(context);
    auto iteratorPointingToTheStructure =
        std::find_if(context.structures.begin(), context.structures.end(),
                     [=](structure str) { return str.name == structureName; });
    unsigned offset = iteratorPointingToTheStructure->memberOffsetInBytes.at(
        children[1].text);
    if (children[1].text.back() == '[') // If it's an array inside of a
                                        // structure
      return AssemblyCode(
          "(i32.add\n" + children[0].compileAPointer(context).indentBy(1) +
              "\n\t(i32.add\n\t\t(i32.const " + std::to_string(offset) +
              ") ;;The offset of the structure member " + structureName + "." +
              children[1].text + "\n\t\t(i32.mul\n\t\t\t(i32.const " +
              std::to_string(
                  isPointerType(iteratorPointingToTheStructure->memberTypes.at(
                      children[1].text))
                      ? 4
                  : context.structureSizes.count(
                        iteratorPointingToTheStructure->memberTypes.at(
                            children[1].text))
                      ? context.structureSizes.at(
                            iteratorPointingToTheStructure->memberTypes.at(
                                children[1].text))
                      : basicDataTypeSizes.at(
                            iteratorPointingToTheStructure->memberTypes.at(
                                children[1].text))) +
              ") ;;Size of the type \"" +
              iteratorPointingToTheStructure->memberTypes.at(children[1].text) +
              "\"\n" +
              convertToInteger32(children[1].children[0], context).indentBy(3) +
              "\n\t\t)\n\t)\n)",
          AssemblyCode::AssemblyType::i32);
    else
      return AssemblyCode("(i32.add\n" +
                              children[0].compileAPointer(context).indentBy(1) +
                              "\n\t(i32.const " + std::to_string(offset) +
                              ") ;;The offset of the structure member " +
                              structureName + "." + children[1].text + "\n)",
                          AssemblyCode::AssemblyType::i32);
  }
  if (text == "->") {
    TreeNode dotOperator(".", lineNumber, columnNumber);
    TreeNode valueAtOperator("ValueAt(", lineNumber, columnNumber);
    valueAtOperator.children.push_back(children[0]);
    dotOperator.children = <% valueAtOperator, children[1] %>;
    return dotOperator.compileAPointer(context);
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
