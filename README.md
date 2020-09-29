# AEC for WebAssembly

This is my attempt to port the [ArithmeticExpressionCompiler](https://github.com/FlatAssembler/ArithmeticExpressionCompiler) language to WebAssembly (the JavaScript bytecode). Right now, it's about as powerful as the original compiler (targeting x86 assembly). The specification for the ArithmeticExpressionCompiler language is (hopefully) available on [my website](https://flatassembler.github.io/AEC_specification.html).

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

<del>At least some compilers are known to miscompile the program. Namely, GCC 4.8.5 (that comes with recent versions of Oracle Linux) compiles the code successfully, but the program exits with [the message](https://github.com/FlatAssembler/AECforWebAssembly/blob/c9212990ec6655d14331fae71f1d782a697a518c/AECforWebAssembly.cpp#L21) that your C++ compiler appears not to support regular expressions. GCC 4.9.2 (that comes with recent versions of Debian) already seems to work. CLANG 10 on Oracle Linux again miscompiles the program, but apparently that happens only on Oracle Linux. Oddly enough, CLANG 9 appears to work fine on Oracle Linux. Honestly, supporting misbehaving C++ compilers makes way less sense than supporting misbehaving browsers. C++ compilers, unlike browsers, are used only by users who can easily install another one. Also, newest versions of GCC, unlike the latest browsers, run on Windows 98 without problems.</del> (UPDATE: I've mitigated that problem by avoiding complicated regular expressions, which some C++ compilers fail to compile. Besides, those complicated regular expressions appear to slow down the program significantly and make it less legible.)

To make sure compiling my compiler is not the problem, I've included some binary files in the releases section. There is also a WebAssebly file compiled with Emscripten, which should work basically wherever NodeJS works.

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

Note that browsers will generally not allow you to run a WASM executable stored on your file system, you need to use some web-server (I use `php -S 127.0.0.1:8080`) to run it in a browser. It's for security reasons. When running a program stored on the Internet, the JavaScript Virtual Machine can be sandboxed from the file system. When running a program stored on your computer, the JavaScript Virtual Machine can't be sandboxed from the file system (it needs to read the program that's stored on your file system). Modern browsers generally allow JavaScript programs on your file system to run, because the JavaScript compiler (V8, SpiderMonkey, ChakraCore...) is trusted not to produce WebAssembly code which will cause the JavaScript Virtual Machine to read private files stored on the computer or modify files. An untrusted WASM file might contain some obscure piece of WebAssembly which will cause a particular JavaScript Virtual Machine to misbehave.

WASM files can be run in NodeJS, as you can see in [this example](https://github.com/FlatAssembler/AECforWebAssembly/blob/master/analogClock/analogClock.js). NodeJS is (unlike browsers) targeted at tech-savvy users, and it trusts you to verify that the WASM file is not malicious before your run it. Well, when compiling an AEC program, you can check both the source code of that program and the source code of the compiler, so that shouldn't be a problem here.

## Limitations

1. The syntax is slightly different from the original AEC to enable new features (useful primarily at string manipulation, but also make it easier to write clean code). Namely, variables have types (`Character`, `CharacterPointer`, `Integer16`, `Integer16Pointer`, `Integer32`, `Integer32Pointer`, `Decimal32`, `Decimal32Pointer`, `Decimal64`, `Decimal64Pointer`) and must be declared before usage. Also, the semicolon `;` is no longer used for single-line comments (now the more-widely-used token `//` is used for that), but for ending the statement (as in most programming languages). Furthermore, the conditions in `If` and `While` are no longer ended with a newline character, but with `Then` and `Loop`.
1. It can't be used to target browsers which don't support WebAssembly (such as Internet Explorer 11) or which support WebAssembly but don't support WebAssembly global variables (Firefox 52, the most common browser on modern computers with Windows XP). In my opinion, this is not a problem now, and will be even less of a problem in the future.
1. <del>There is some weird [heisenbug](https://en.wikipedia.org/wiki/Heisenbug) that modifies the syntax tree during compilation (at least when the compiler is compiled with GCC 9.3.0 on Oracle Linux), which sometimes exposes itself when returning (from a function) an expression with an assignment operator in it. I've made a [simple mitigation](https://github.com/FlatAssembler/AECforWebAssembly/blob/c9212990ec6655d14331fae71f1d782a697a518c/compiler.cpp#L469) of it, but, in order to make the compiler reliable, we need to find what causes it. What makes it even more weird is that the compilation methods are all defined as `const`, so the C++ compilers should refuse to compile a code that does that, but all compilers I tried don't. I think what's going on is that there is a strong incentive for those who make C++ compilers to make compilers that will accept invalid code, incentive created by people who want to compile programs they don't understand how they work from source (not to say that I am not one of those people). Of course, that's a bad thing if you are using that C++ compiler to develop a program that will actually work. Still, the situation with C++ is better than the situation with JavaScript. (UPDATE: I apparently triggered an undefined behavior in C++ that no compiler warned me about, see more details here: https://stackoverflow.com/questions/63951270/using-default-copy-constructor-corrupts-a-tree-in-c ).</del> (UPDATE: I think I solved that problem.)
1. While the code builds cleanly with `-Wall`, it doesn't build without warnings when using `-Wall -O3`, complaining about [something](https://github.com/FlatAssembler/AECforWebAssembly/blob/c9212990ec6655d14331fae71f1d782a697a518c/bitManipulations.cpp#L71) in `bitManipulations.cpp`. While I indeed deviated from standard C++, there doesn't appear to be a simple solution. Writing a decimal-to-IEEE754-converter myself would be tedious and error prone.
1. <del>The compiler crashes when you write `AddressOf(name_of_some_pointer)` and pointers to pointers are therefore not currently supported. There is simple [workaround](https://github.com/FlatAssembler/AECforWebAssembly/blob/c9212990ec6655d14331fae71f1d782a697a518c/analogClock/analogClock.aec#L12) for that, declare pointers you need pointers to as `Integer32`. However, in order to support data structures and custom data types, we will need to solve this problem. We will need to significantly modify the compiler (keep in mind it shouldn't crash even when somebody writes `AddressOf(pointer_to_pointer)`) and slightly modify the parser (to recognize `CharacterPointerPointer` as a legitimate token in variable declarations) to make that possible.</del> The compiler now supports pointers to pointers, but not in a way that would make it any easier to implement the structures.

## Notes for Contributors

If you don't know about compiler theory and don't want to read about such complicated stuff in English, you can read the [paper about compiler theory](https://github.com/FlatAssembler/ArithmeticExpressionCompiler/raw/master/seminar/PojednostavljeniSeminar.pdf) I've published in Osječki Matematički List. It's in Croatian, and I hope it helps.

To format the code, I generally prefer to use [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) for languages that it supports (JavaScript, C++), because I find the code it produces nicer. For languages it doesn't support (HTML, CSS), I use [Prettier](https://prettier.io/). The code Prettier outputs isn't particularly nice, but something is better than nothing. It's also interesting that `clang-format`, if you supply it with code in this version of AEC and tell it to format the code, it will format the code assuming it's a C-like language, but (as far as I can tell) it won't break it. The result isn't particularly nice, but maybe some automated formatting is better than nothing.
