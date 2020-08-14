# AEC for WebAssembly

This is my attempt to port the [ArithmeticExpressionCompiler](https://github.com/FlatAssembler/ArithmeticExpressionCompiler) language to WebAssembly (the JavaScript bytecode). Right now, it's about as powerful as the original compiler (targeting x86 assembly).

## Compiling instructions

You should be able to compile this program as follows:

```
g++ -o aec AECforWebAssembly.cpp -std=c++11 -O3
#-std=c++11 is only needed for older versions of GCC.
```

Or with:

```
clang++ -o aec AECforWebAssembly.cpp -O3
```

At least some compilers are known to miscompile the program. Namely, GCC 4.8.5 (that comes with recent versions of Oracle Linux) compiles the code successfully, but the program exits with [the message](https://github.com/FlatAssembler/AECforWebAssembly/blob/c9212990ec6655d14331fae71f1d782a697a518c/AECforWebAssembly.cpp#L21) that your C++ compiler appears not to support regular expressions. GCC 4.9.2 (that comes with recent versions of Debian) already seems to work. CLANG 10 on Oracle Linux again miscompiles the program, but apparently that happens only on Oracle Linux. Oddly enough, CLANG 9 appears to work fine on Oracle Linux. Honestly, supporting misbehaving C++ compilers makes way less sense than supporting misbehaving browsers. C++ compilers, unlike browsers, are used only by users who can easily install another one. Also, newest versions of GCC, unlike the latest browsers, run on Windows 98 without problems.

To make sure that's not the problem, I've included some binary files in the releases section. There is also a WebAssebly file compiled with Emscripten, which should work basically wherever NodeJS works.

## Usage instructions

Linux:

```
./aec name_of_the_aec_program.aec
#If everything is fine, it should produce WebAssembly text file named
# "name_of_the_aec_program.wat".
wat2wasm name_of_the_aec_program.wat
```

Windows:

```
aec name_of_the_aec_program.aec
wat2wasm name_of_the_aec_program.wat
```

Of course, you need to have `wat2wasm` from [WebAssembly Binary Toolkit](https://github.com/WebAssembly/wabt) installed. Then you'll get a WASM file, which nearly all modern JavaScript environments support.

## Limitations

1. The syntax is slightly different from the original AEC to enable new features (useful primarily at string manipulation, but also make it easier to write clean code). Namely, variables have types (`Character`, `CharacterPointer`, `Integer16`, `Integer16Pointer`, `Integer32`, `Integer32Pointer`, `Decimal32`, `Decimal32Pointer`, `Decimal64`, `Decimal64Pointer`) and must be declared before usage. Also, the semicolon `;` is no longer used for single-line comments (now the more-widely-used token `//` is used for that), but for ending the statement (as in most programming languages). Furthermore, the conditions in `If` and `While` are no longer ended with a newline character, but with `Then` and `Loop`.
1. It can't be used to target browsers which don't support WebAssembly (such as Internet Explorer 11) or which support WebAssembly but don't support WebAssembly global variables (Firefox 52, the most common browser on modern computers with Windows XP). In my opinion, this is not a problem now, and will be even less of a problem in the future.
1. There is some weird bug that modifies AST during compilation (at least when the compiler is compiled with GCC 9.3.0 on Oracle Linux), which exposes itself when returning (from a function) an expression with an assignment operator in it. I've made a [simple mitigation](https://github.com/FlatAssembler/AECforWebAssembly/blob/c9212990ec6655d14331fae71f1d782a697a518c/compiler.cpp#L469) of it, but, in order to make the compiler reliable, we need to find what causes it. What makes it even more weird is that the compilation methods are all defined as `const`, so the C++ compilers should refuse to compile a code that does that, but all compilers I tried don't.
1. While the code builds cleanly with `-Wall`, it doesn't build without warnings when using `-Wall -O3`, complaining about [something](https://github.com/FlatAssembler/AECforWebAssembly/blob/c9212990ec6655d14331fae71f1d782a697a518c/bitManipulations.cpp#L71) in `bitManipulations.cpp`. While I indeed deviated from standard C++, there doesn't appear to be a simple solution. Writing a decimal-to-IEEE754-converter myself would be tedious and error prone.
1. The compiler crashes when you write `AddressOf(name_of_some_pointer)` and pointers to pointers are therefore not currently supported. There is simple [workaround](https://github.com/FlatAssembler/AECforWebAssembly/blob/c9212990ec6655d14331fae71f1d782a697a518c/analogClock/analogClock.aec#L12) for that, declare pointers you need pointers to as `Integer32`. However, in order to support data structures and custom data types, we will need to solve this problem. We will need to significantly modify the compiler (keep in mind it shouldn't crash even when somebody writes `AddressOf(pointer_to_pointer)`) and slightly modify the parser (to recognize `CharacterPointerPointer` as a legitimate token in variable declarations) to make that possible.
