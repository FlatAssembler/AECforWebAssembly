# AEC for WebAssembly

![Logo](logo.PNG)

This is my attempt to port the [ArithmeticExpressionCompiler](https://github.com/FlatAssembler/ArithmeticExpressionCompiler) language to WebAssembly (the JavaScript bytecode). The compiler has been rewritten from scratch in C++ (the original compiler was written in a combination of C and JavaScript, using the Duktape framework). Right now, it's more powerful than the original compiler (targeting x86 assembly), as it includes support for more data types, including the support for structures. The specification for the ArithmeticExpressionCompiler language is (hopefully) available on [my website](https://flatassembler.github.io/AEC_specification).

## Compiling instructions

More detailed compiling instructions, as well as compiling instructions for the original ArithmeticExpressionCompiler, are available on [my website](https://flatassembler.github.io/AEC_specification#AEC_to_WebAssembly). They are somewhat Linux-specific, but I do not think that is a problem. Anyway, here are some less detailed compiling instructions...

While this project includes a CMAKE build script, this is primarily to make it easier to open this project in various IDEs and for automated testing on GitHub and GitLab. You should be able to compile this project simply as follows:

```
g++ -o aec AECforWebAssembly.cpp -std=c++11 -O3
#-std=c++11 is only needed for older versions of GCC.
```

Or with:

```
clang++ -o aec AECforWebAssembly.cpp -O3
```

<del>At least some compilers are known to miscompile the program. Namely, GCC 4.8.5 (that comes with recent versions of Oracle Linux) compiles the code successfully, but the program exits with [the message](https://github.com/FlatAssembler/AECforWebAssembly/blob/c9212990ec6655d14331fae71f1d782a697a518c/AECforWebAssembly.cpp#L21) that your C++ compiler appears not to support regular expressions. GCC 4.9.2 (that comes with recent versions of Debian) already seems to work. CLANG 10 on Oracle Linux again miscompiles the program, but apparently that happens only on Oracle Linux. Oddly enough, CLANG 9 appears to work fine on Oracle Linux. Honestly, supporting misbehaving C++ compilers makes way less sense than supporting misbehaving browsers. C++ compilers, unlike browsers, are used only by users who can easily install another one. Also, newest versions of GCC, unlike the latest browsers, run on Windows 98 without problems.</del> (UPDATE: I've mitigated that problem by avoiding complicated regular expressions, which some C++ compilers fail to compile. Besides, those complicated regular expressions appear to slow down the program significantly and make it less legible.)

To make sure compiling my compiler is not the problem, I've included some binary files in the releases section on GitHub, and they are also available on SourceForge (but not on GitLab). There is also a WebAssembly file compiled with Emscripten, which should work basically wherever NodeJS works.

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

Of course, you need to have `wat2wasm` from [WebAssembly Binary Toolkit](https://github.com/WebAssembly/wabt) installed. The simplest way to install it is probably to globally install the `wabt` package from the Node Package Manager (NPM), assuming you already have NodeJS installed. After you run the command above, you'll get a WASM file, which nearly all modern JavaScript environments support (but notice that the WASM files produced by this compiler assume `WebAssembly.Global` exists, which is not true in some older JavaScript environments which otherwise support WebAssembly, such as Firefox 52).

Note that browsers will generally not allow you to run a WASM executable stored on your file system, you need to use some web-server (I use `php -S 127.0.0.1:8080`) to run it in a browser. It's for security reasons. When running a program stored on the Internet, the JavaScript Virtual Machine can be sandboxed from the file system. When running a program stored on your computer, the JavaScript Virtual Machine can't be sandboxed from the file system (it needs to read the program that's stored on your file system). Modern browsers generally allow JavaScript programs on your file system to run, because the JavaScript compiler (V8, SpiderMonkey, ChakraCore...) is trusted not to produce WebAssembly code which will cause the JavaScript Virtual Machine to read private files stored on the computer or modify files. An untrusted WASM file might contain some obscure piece of WebAssembly which will cause a particular JavaScript Virtual Machine to misbehave.

WASM files can be run in NodeJS, as you can see in [this example](https://github.com/FlatAssembler/AECforWebAssembly/blob/master/analogClock/analogClock.js). NodeJS is (unlike browsers) targeted at tech-savvy users, and it trusts you to verify that the WASM file is not malicious before you run it. Well, when compiling an AEC program, you can check both the source code of that program and the source code of the compiler, so that shouldn't be a problem here.

## Shell script

So, to summarize how to compile and use my compiler, I've written a shell script to download the compiler, compile and assemble the Analog Clock example program, and run it in NodeJS:
```bash
if [ $(command -v git > /dev/null 2>&1 ; echo $?) -eq 0 ]
then
  git clone https://github.com/FlatAssembler/AECforWebAssembly.git
  cd AECforWebAssembly
elif [ $(command -v wget > /dev/null 2>&1 ; echo $?) -eq 0 ]
then
  mkdir AECforWebAssembly
  cd AECforWebAssembly
  wget https://github.com/FlatAssembler/AECforWebAssembly/archive/refs/heads/master.zip
  unzip master.zip
  cd AECforWebAssembly-master
else
  mkdir AECforWebAssembly
  cd AECforWebAssembly
  curl -o AECforWebAssembly.zip -L https://github.com/FlatAssembler/AECforWebAssembly/archive/refs/heads/master.zip # Without the "-L", "curl" will store HTTP Response headers of redirects to the ZIP file instead of the actual ZIP file.
  unzip AECforWebAssembly.zip
  cd AECforWebAssembly-master
fi
if [ $(command -v g++ > /dev/null 2>&1 ; echo $?) -eq 0 ]
then
  g++ -std=c++11 -o aec AECforWebAssembly.cpp # "-std=c++11" should not be necessary for newer versions of "g++". Let me know if it is, as that probably means I disobeyed some new C++ standard (say, C++23).
else
  clang++ -o aec AECforWebAssembly.cpp
fi
cd analogClock
../aec analogClock.aec
npx -p wabt wat2wasm analogClock.wat
if [ "$OS" = "Windows_NT" ] # https://stackoverflow.com/a/75125384/8902065
                            # https://www.reddit.com/r/bash/comments/10cip05/comment/j4h9f0x/?utm_source=share&utm_medium=web2x&context=3
then
  node_version=$(node.exe -v)
else # We are presumably running on an UNIX-like system, where storing output of some program into a variable works as expected.
  node_version=$(node -v)
fi
# "node -v" outputs version in the format "v18.12.1"
node_version=${node_version:1} # Remove 'v' at the beginning
node_version=${node_version%\.*} # Remove trailing ".*".
node_version=${node_version%\.*} # Remove trailing ".*".
node_version=$(($node_version)) # Convert the NodeJS version number from a string to an integer.
if [ $node_version -lt 11 ]
then
  echo "NodeJS version is lower than 11 (it is $node_version), you will probably run into trouble!"
fi
node analogClock

```
If everything is fine, the Analog Clock program should print the current time in the terminal.

## Limitations

* The syntax is slightly different from the original AEC to enable new features (useful primarily at string manipulation, but also make it easier to write clean code). Namely, variables have types (`Character`, `CharacterPointer`, `Integer16`, `Integer16Pointer`, `Integer32`, `Integer32Pointer`, `Decimal32`, `Decimal32Pointer`, `Decimal64`, `Decimal64Pointer`) and must be declared before usage. Also, the semicolon `;` is no longer used for single-line comments (now the more-widely-used token `//` is used for that), but for ending the statement (as in most programming languages). Furthermore, the conditions in `If` and `While` are no longer ended with a newline character, but with `Then` and `Loop`. Also, the hard-to-type operators `&` and `|` are replaced by `and` and `or`, respectively. There are many minor differences, such as the original AEC allowing the conditional `?:` operator only in expressions, whereas the [new AEC also allows `?:` on the left-hand-side of the assignment `:=` operator](https://flatassembler.github.io/AEC_specification#left_hand_side_conditional_operator). And, more importantly, the AEC-to-x86 compiler supports both `(` and `[` to be used for arrays, whereas the AEC-to-WebAssembly compiler supports only `[`. Also, the AEC-to-WebAssembly compiler supports the operators `+=`, `-=`, `*=` and `/=`, with the same meaning as they have in C.<br/>
For example, the following code in the x86 dialect of AEC:
```
i := 0
While i < n | i = n
	If i = 0
		fib(i) := 0
	ElseIf i = 1
		fib(i) := 1
	Else
		fib(i) := fib(i - 1) + fib(i - 2)
	EndIf
	i := i + 1
EndWhile
fib(n)
```
is nearly equal to the following code in the WebAssembly dialect of AEC:
```
Function fib(Integer16 n) Which Returns Decimal64 Does
  Integer16 i := 0;
  Decimal64 fib[100];
  While i < n or i = n Loop
     If i = 0 Then
       fib[i] := 0;
     ElseIf i = 1 Then
       fib[i] := 1;
     Else
       fib[i] := fib[i - 1] + fib[i - 2];
     EndIf
     i += 1;
  EndWhile
  Return fib[n];
EndFunction
```
* The AEC-to-WebAssembly compiler can't be used to target browsers which don't support WebAssembly (such as Internet Explorer 11) or which support WebAssembly but don't support WebAssembly global variables (Firefox 52, the most common browser on modern computers with Windows XP). In my opinion, this is not a problem now, and will be even less of a problem in the future.
* <del>There is some weird [heisenbug](https://en.wikipedia.org/wiki/Heisenbug) that modifies the syntax tree during compilation (at least when the compiler is compiled with GCC 9.3.0 on Oracle Linux), which sometimes exposes itself when returning (from a function) an expression with an assignment operator in it. I've made a [simple mitigation](https://github.com/FlatAssembler/AECforWebAssembly/blob/c9212990ec6655d14331fae71f1d782a697a518c/compiler.cpp#L469) of it, but, in order to make the compiler reliable, we need to find what causes it. What makes it even more weird is that the compilation methods are all defined as `const`, so the C++ compilers should refuse to compile a code that does that, but all compilers I tried don't. I think what's going on is that there is a strong incentive for those who make C++ compilers to make compilers that will accept invalid code, incentive created by people who want to compile programs they don't understand how they work from source (not to say that I am not one of those people). Of course, that's a bad thing if you are using that C++ compiler to develop a program that will actually work. Still, the situation with C++ is better than the situation with JavaScript. (UPDATE: I apparently triggered an undefined behavior in C++ that no compiler warned me about, see more details here: https://stackoverflow.com/questions/63951270/using-default-copy-constructor-corrupts-a-tree-in-c ).</del> (UPDATE: I think I solved that problem.)
* <del>While the code builds cleanly with `-Wall`, it doesn't build without warnings when using `-Wall -O3`, complaining about [something](https://github.com/FlatAssembler/AECforWebAssembly/blob/c9212990ec6655d14331fae71f1d782a697a518c/bitManipulations.cpp#L71) in `bitManipulations.cpp`. While I indeed deviated from standard C++, there doesn't appear to be a simple solution. Writing a decimal-to-IEEE754-converter myself would be tedious and error prone. (UPDATE: Solved that using unions.).</del> (UPDATE: I solved that problem using `memcpy`. Apparently using unions for that is not valid in standard C++, as [the friends I met on Discord warned me](https://discord.com/channels/172018499005317120/172018499005317120/807361535193776138), even though every compiler I tried accepts that without a warning.)
* <del>The compiler crashes when you write `AddressOf(name_of_some_pointer)` and pointers to pointers are therefore not currently supported. There is simple [workaround](https://github.com/FlatAssembler/AECforWebAssembly/blob/c9212990ec6655d14331fae71f1d782a697a518c/analogClock/analogClock.aec#L12) for that, declare pointers you need pointers to as `Integer32`. However, in order to support data structures and custom data types, we will need to solve this problem. We will need to significantly modify the compiler (keep in mind it shouldn't crash even when somebody writes `AddressOf(pointer_to_pointer)`) and slightly modify the parser (to recognize `CharacterPointerPointer` as a legitimate token in variable declarations) to make that possible. (UPDATE: The compiler now supports pointers to pointers, but not in a way that would make it any easier to implement the structures.)</del> (UPDATE: Structures are now supported to some extent, as the tests in `structureDeclarationTest.aec` pass.)
* The tokenizer of the AEC-to-WebAssembly compiler is slow. I have opened a [discussion on forum.hr about how to speed the tokenizer up](https://www.forum.hr/showthread.php?t=1243509). (UPDATE: It has been significantly speeded up since then. Right now, the core of the compiler is slower than the tokenizer, but it can be speeded up by disabling logging.)
* There is a [weird bug causing Stack Overflow](https://github.com/FlatAssembler/AECforWebAssembly/issues/5) error if the program is compiled using Visual Studio <del>or CLANG on Windows</del>. It does not seem to occur in other compilers.
* The build using Microsoft C++ Compiler is very unclean, as it outputs **a ton** of warnings when compiling the AEC-to-WebAssembly compiler.

## Notes for Contributors

If you don't know about compiler theory and don't want to read about such complicated stuff in English, you can read the [paper about compiler theory](https://github.com/FlatAssembler/ArithmeticExpressionCompiler/raw/master/seminar/PojednostavljeniSeminar.pdf) I've published in Osječki Matematički List. It's in Croatian, and I hope it helps.<br/>
**UPDATE** on 23/10/2020: I've published a [YouTube video](https://youtu.be/Br6Zh3Rczig) in English with approximately the same content as the paper. In case your browser cannot play streamed MP4 videos, you can download the [MP4 file](https://flatassembler.github.io/compiler_theory.mp4) and try opening it in VLC or something like that.<br/>
**UPDATE** on 23/07/2025: To support the Latina Rediviva movement (which seeks to revive the Latin language as a medium of international communication), I made [a video about the basics of how compilers work in the Latin language](https://youtu.be/hlw72oFlKZA). There is [an MP4 file](https://flatassembler.github.io/compiler-theory-latin.mp4) as well.

To format the code, I generally prefer to use [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) for languages that it supports (JavaScript, C++), because I find the code it produces nicer. For languages it doesn't support (HTML, CSS), I use [Prettier](https://prettier.io/). The code Prettier outputs isn't particularly nice, but something is better than nothing. It's also interesting that `clang-format`, if you supply it with code in this version of AEC and tell it to format the code, it will format the code assuming it's a C-like language, but (as far as I can tell) it won't break it. The result isn't particularly nice, but maybe some automated formatting is better than nothing.
