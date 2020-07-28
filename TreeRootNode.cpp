#include "TreeNode.cpp"

class TreeRootNode : public TreeNode // Sets up the compilation context, rather
                                     // than compiling itself.
{
public:
  TreeRootNode() {
    text = "module"; // That's how the root node of the AST is called in
                     // WebAssembly and JavaScript.
  }
  AssemblyCode compile() {
    std::cerr << "Compiler is not yet implemented!" << std::endl;
    exit(1);
  }
  AssemblyCode compile(CompilationContext context) {
    std::cerr << "Internal compiler error: Some part of the compiler attempted "
                 "to recursively call the compiler from the beginning (leading "
                 "to an infinite loop), quitting now!"
              << std::endl;
    exit(1);
    return AssemblyCode(
        "()"); // So that the compiler doesn't throw a bunch of warnings about
               // the control reaching the end of a non-void function.
  }
  AssemblyCode compileAPointer(CompilationContext context) {
    std::cerr << "Internal compiler error: Some part of the compiler attempted "
                 "to get the assembly of the pointer of a module, which "
                 "doesn't make sense. Quitting now!"
              << std::endl;
    exit(1);
    return AssemblyCode("()");
  }
  int interpretAsACompileTimeIntegerConstant() {
    std::cerr
        << "Some part of the compiler attempted to convert a module to the "
           "compile time constant, which makes no sense. Quitting now!"
        << std::endl;
    exit(1);
    return 0;
  }
};
