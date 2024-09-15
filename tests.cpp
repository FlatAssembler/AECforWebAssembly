/*
 * So, I've implemented some automated tests (I am not sure those can be
 * called unit tests) for the tokenizer and the parser. Unfortunately,
 * it's not nearly as easy to implement automated tests for the core of
 * the compiler.
 */

#include "CorruptCompilationContextException.cpp"
#include "InvalidTypenameException.cpp"
#include "NotImplementedException.cpp"
#include "TreeNode.cpp"
#include <ciso646> // Necessary for Microsoft C++ Compiler.
#include <typeinfo>

struct test { // When the debugger doesn't work (and I can't get it to
              // work on my Linux machine), "cassert" doesn't help a lot.
  std::string input, expectedResult;
};

void tokenizerTests() {
  std::vector<test> tests(
      {{"\"/*Comment inside a string*/\"",
        "['\"/*Comment inside a string*/\"']"},
       {"5+5", "['5','+','5']"},
       {"5+5=10", "['5','+','5','=','10']"},
       {"55.;", "['55.',';']"},
       {"structureName.structureMember/3.14159265359",
        "['structureName','.','structureMember','/','3.14159265359']"},
       {"sin(pi/2)=1", "['sin(','pi','/','2',')','=','1']"},
       {"'A'+2='C'", "['65','+','2','=','67']"},
       {"a:=a+1; //Some random comment.\nb:=b-1;",
        "['a',':=','a','+','1',';','b',':=','b','-','1',';']"},
       {"/*This should be tokenized into\nan empty string*/", "[]"},
       {"a/*Randomly\ninserted\ncomment.*/+/*Another\nrandom\ncomment*/b",
        "['a','+','b']"},
       {"array_name := {1, 1 + 1, 1 + 1 + 1}",
        "['array_name',':=','{','1',',','1','+','"
        "1',',','1','+','1','+','1','}']"},
       {"array_name [index] /*Make sure whitespacing does not break it.*/",
        "['array_name[','index',']']"},
       {"function_name (argument) /*Again, whitespace.*/",
        "['function_name(','argument',')']"},
       {
           R"(
array_name
//A comment about the name of the array.
[index])",
           "['array_name[','index',']']"},
       {
           R"abc(R"(
Hello world!
)";)abc",
           "['\"\\nHello world!\\n\"',';']"},
       {
           R"(R"abc(
Hello world!
)abc";)",
           "['\"\\nHello world!\\n\"',';']"},
       {R"(('\"'))", R"(['(',''\"'',')'])"},
       {R"(("\\\"Hello world!\"\\"))", R"(['(','"\\\"Hello world!\"\\"',')'])"},
       {"2_147_483_647", "['2147483647']"}});
  for (unsigned int i = 0; i < tests.size(); i++) {
    std::string result =
        TreeNode::JSONifyArrayOfTokens(TreeNode::tokenize(tests[i].input));
    if (result != tests[i].expectedResult) {
      std::cerr << "Internal compiler error: Tokenizer test failed: \""
                << tests[i].input << "\" tokenizes into \"" << result
                << "\" instead of to \"" << tests[i].expectedResult << "\"!"
                << std::endl;
      std::exit(1);
    }
  }
}

void simpleParserTests() {
  std::vector<test> tests(
      {{"5+3", "(+ 5 3)"},
       {"2+3*4", "(+ 2 (* 3 4))"},
       {"2+2<5 and 3.14159265359<3.2",
        "(and (< (+ 2 2) 5) (< 3.14159265359 3.2))"},
       {"1-2-3-4", "(- (- (- 1 2) 3) 4)"},
       {"a:=b:=c", "(:= a (:= b c))"},
       {"2+2<=5", "(<= (+ 2 2) 5)"},
       {"3.14.16", "(. 3.14 16)"},
       {"1*(2+3)", "(* 1 (+ 2 3))"},
       {"pow(2+2/*A nonsensical expression,\nbut good for "
        "testing the parser.*/,sin(pi/2))",
        "(pow (+ 2 2) (sin (/ pi 2)))"},
       {"some_procedure()", "some_procedure("},
       {"abs(function_returning_pi()-pi)<1/1000",
        "(< (abs (- function_returning_pi( pi)) (/ 1 1000))"},
       {"not(d=0)?c/d:0", "(?: (not (= d 0)) (/ c d) 0)"},
       {"(2+2>5?3+3<7?1:0-2:2+2-4<1?0:2+2<4?0-1:0-3)",
        "(?: (> (+ 2 2) 5) (?: (< (+ 3 3) 7) 1 (- 0 2)) (?: (< (- (+ 2 2) 4) "
        "1) 0 (?: (< (+ 2 2) 4) (- 0 1) (- 0 3))))"},
       {"(2+2>5?3+3<7?1:-2:2+2-4<1?0:2+2<4?-1:-3)",
        "(?: (> (+ 2 2) 5) (?: (< (+ 3 3) 7) 1 (- 0 2)) (?: (< (- (+ 2 2) 4) "
        "1) 0 (?: (< (+ 2 2) 4) (- 0 1) (- 0 3))))"},
       {"some_array[i+1]", "(some_array (+ i 1))"},
       {"array_pointer:={1,1+1,1+1+1}",
        "(:= array_pointer ({} 1 (+ 1 1) (+ (+ 1 1) 1)))"}});
  for (unsigned int i = 0; i < tests.size(); i++) {
    std::string result =
        TreeNode::parseExpression(TreeNode::tokenize(tests[i].input))[0]
            .getLispExpression();
    if (result != tests[i].expectedResult) {
      std::cerr << "Internal compiler error: Basic parser test failed: \""
                << tests[i].input << "\" parses into \"" << result
                << "\" instead of to \"" << tests[i].expectedResult << "\"!"
                << std::endl;
      std::exit(1);
    }
  }
}

void interpreterTests() {
  std::vector<test> tests(
      {{"1+2*3", "7"},
       {"(1+2)*3", "9"},
       {"(2+2=4)?2:0", "2"},
       {"mod(5,2)", "1"},
       {"(2+2>5?3+3<7?1:-2:2+2-4<1?0:2+2<4?-1:-3)+('A'+2='C'?0:-1)", "0"},
       {"1 ? 2 ? 3 : 4 : 5","3"}, // https://www.reddit.com/r/ProgrammingLanguages/comments/10rxexc/comment/j72oxrl/?utm_source=reddit&utm_medium=web2x&context=3
       {"0xff", "255"},
       {"0x41='A' and 0xff=255", "1"},
       {"0x42='A' or 0x2b=127", "0"},
       {"5/2", "2" /*A simple division in a compile-time integer constant was
                     crashing the compiler all the way up to version v1.4.3*/},
       {"3 >= 2 >= 1", "1"},
       {R"(
       	       (1 < 2 < 3) and
	       not(2 < 3 < 1) and
	       (-3 < -2 < -1) and
	       (3 > 2 > 1)
       )", "1"},
       {"5 + + 1", "6"},
       {"5*-1", "-5"}
      });
  for (unsigned int i = 0; i < tests.size(); i++) {
    std::string result = std::to_string(
        TreeNode::parseExpression(TreeNode::tokenize(tests[i].input))[0]
            .interpretAsACompileTimeIntegerConstant());
    if (result != tests[i].expectedResult) {
      std::cerr << "Internal compiler error: Interpreter test failed: \""
                << tests[i].input << "\" interprets into \"" << result
                << "\" instead of to \"" << tests[i].expectedResult << "\"!"
                << std::endl;
      std::exit(1);
    }
  }
  struct decimalTest {
    std::string input;
    double expectedResult;
  };
  std::vector<decimalTest> decimalTests({{"5/2", 5. / 2},
                                         {"sqrt(9)", 3},
                                         {"pow(sin(pi/4),2)", 1. / 2},
                                         {"log(e)", 1},
                                         {"1.5+1.5", 3}});
  for (unsigned int i = 0; i < decimalTests.size(); i++) {
    double result =
        TreeNode::parseExpression(TreeNode::tokenize(decimalTests[i].input))[0]
            .interpretAsACompileTimeDecimalConstant();
    if (std::fabs(result - decimalTests[i].expectedResult) > 1. / 1000) {
      std::cerr << "Internal compiler error: Interpreter test failed: \""
                << decimalTests[i].input << "\" interprets into \"" << result
                << "\" instead of to \"" << decimalTests[i].expectedResult
                << "\"!" << std::endl;
      std::exit(1);
    }
  }
}

void parsingVariableDeclarationsTests() {
  std::vector<test> tests(
      {{"Integer32 some_array[80*23],array_width:=80,array_height:=23",
        "(Integer32 (some_array (* 80 23)) (array_width (:= 80)) (array_height "
        "(:= 23)))"}});
  for (unsigned int i = 0; i < tests.size(); i++) {
    std::string result = TreeNode::parseVariableDeclaration(
                             TreeNode::tokenize(tests[i].input))[0]
                             .getLispExpression();
    if (result != tests[i].expectedResult) {
      std::cerr << "Internal compiler error: Parser test failed: \""
                << tests[i].input << "\" parses into \"" << result
                << "\" instead of to \"" << tests[i].expectedResult << "\"!"
                << std::endl;
      std::exit(1);
    }
  }
}

void parserTests() {
  std::vector<test> tests(
      {{"Integer32 some_array[80*23],array_width:=80,array_height:=23;",
        "(Integer32 (some_array (* 80 23)) (array_width (:= 80)) (array_height "
        "(:= 23)))"},
       {"Function log(CharPointer string) Which Returns Nothing Is External;",
        "(Function (log (CharPointer string)) (Returns Nothing) External)"},
       {"Function foo(Integer32 a:=0, Integer32 c) Which Returns Nothing Does "
        "//Nonsense "
        "code, but useful to see if the parser is capable of parsing simple "
        "functions.\n"
        "Integer32 b;\n"
        "b:=a+1;\n"
        "EndFunction",
        "(Function (foo (Integer32 (a (:= 0))) (Integer32 c)) (Returns "
        "Nothing) (Does "
        "(Integer32 b) (:= b (+ a 1))))"},
       {"Function factorial(Integer32 n) Which Returns Integer64 Does "
        "Integer64 counter:=1,result:=1;"
        "While counter<n or counter=n Loop "
        "result:=result*counter;"
        "EndWhile "
        "Return result;"
        "EndFunction",
        "(Function (factorial (Integer32 n)) (Returns Integer64) (Does "
        "(Integer64 (counter (:= 1)) (result (:= 1))) (While (or (< counter n) "
        "(= counter n)) (Loop (:= result (* result counter)))) (Return "
        "result)))"},
       {"Function gcd(Integer32 a, Integer32 b) Which Returns Integer32 Does "
        "//Euclidean Algorithm\n"
        "While not(b=0) Loop\n"
        "If a>b Then\n"
        "a:=a-b;\n"
        "Else\n"
        "b:=b-a;\n"
        "EndIf\n"
        "EndWhile\n"
        "Return a;\n"
        "EndFunction",
        "(Function (gcd (Integer32 a) (Integer32 b)) (Returns Integer32) (Does "
        "(While (not (= b 0)) (Loop (If (> a b) (Then (:= a (- a b))) "
        "(Else (:= b (- b a)))))) (Return a)))"},
       {"If a<b and a<c Then //This is semantically very wrong (an "
        "if-statement outside of a function), but it should still parse.\n"
        "If a>0 Then Return a; Else Return 0; EndIf\n"
        "ElseIf b<a and b<c Then\n"
        "If b>0 Then Return b; Else Return 0; EndIf\n"
        "Else\n"
        "If c>0 Then Return c; Else Return 0; EndIf\n"
        "EndIf",
        "(If (and (< a b) (< a c)) (Then "
        "(If (> a 0) (Then (Return a)) (Else (Return 0)))) "
        "(Else (If (and (< b a) (< b c)) (Then "
        "(If (> b 0) (Then (Return b)) (Else (Return 0)))) "
        "(Else "
        "(If (> c 0) (Then (Return c)) (Else (Return 0)))))))"},
       {"Structure Point Consists Of Decimal32 x,y,z;Integer16 "
        "number_of_dimensions;EndStructure",
        "(Structure Point (Of (Decimal32 x y z) (Integer16 "
        "number_of_dimensions)))"},
       {"Structure empty_structure Consists Of EndStructure //Nonsense code, "
        "but should not crash the parser.",
        "(Structure empty_structure Of)"},
       {"Function random_function(CharacterPointer ptr) Which Returns Nothing "
        "Does Integer32 first_array[3]:={1,2,3};CharacterPointer "
        "helloWorldString:=\"Hello world!\";EndFunction",
        "(Function (random_function (CharacterPointer ptr)) (Returns Nothing) "
        "(Does (Integer32 (first_array 3 (:= ({} 1 2 3)))) (CharacterPointer "
        "(helloWorldString (:= \"Hello world!\")))))"},
       {"Function random_function(PointerToCharacter ptr) Which Returns "
        "Nothing "
        "Does Integer32 first_array[3]:={1,2,3};PointerToCharacter "
        "helloWorldString:=\"Hello world!\";EndFunction",
        "(Function (random_function (CharacterPointer ptr)) (Returns Nothing) "
        "(Does (Integer32 (first_array 3 (:= ({} 1 2 3)))) (CharacterPointer "
        "(helloWorldString (:= \"Hello world!\")))))"},
       {"Function empty_function_without_arguments() Which Returns Nothing "
        "Does /*Nothing*/ EndFunction //Who would have thought something like "
        "that would cause the earlier version of my parser to crash? It's "
        "really hard to guess what are edge cases and what can trigger them.",
        "(Function empty_function_without_arguments( (Returns Nothing) "
        "Does)"},
       {R"( //It actually makes a lot of sense to use multi-line strings
             //in the tests, doesn't it?
        If (a or b) and (c or d) Then //Something like this crashed
                                      //a previous version of the parser.
            Return (a + b) * (c + d);
        EndIf
        )",
        "(If (and (or a b) (or c d)) (Then (Return (* (+ a b) (+ c d)))))"},
       {R"(
			Function gcd(Integer32 a, Integer32 b) Which Returns Integer32 Does
			{ //The curly brace here is a hint to the code formatter.
				While not(b = 0) Loop {
					If a > b Then {
						a -= b;
					} Else {
						b -= a;
					} EndIf
				}
				EndWhile;
				Return a;
			}
			EndFunction;
		)",
        "(Function (gcd (Integer32 a) (Integer32 b)) (Returns Integer32) (Does "
        "(While (not (= b 0)) (Loop (If (> a b) (Then (-= a b)) (Else (-= b "
        "a))))) (Return a)))"},
       {"Return nestedStructure.structureWithInnerNumber.innerNumber;",
        "(Return (. (. nestedStructure structureWithInnerNumber) "
        "innerNumber))"},
       {"InstantiateStructure QuadraticEquationSolution solutions[1 * 2 * 3];",
        "(InstantiateStructure (QuadraticEquationSolution (solutions (* (* 1 "
        "2) 3))))"},
       {
           R"(
	       Function f(Decimal64 milliard := pow(10, 9)) Which Returns Nothing Does
	       		// This function only parses correctly as of AECforWebAssembly v2.7.0.
			Nothing;
	       EndFunction
	       )",
           "(Function (f (Decimal64 (milliard (:= (pow 10 9))))) (Returns "
           "Nothing) (Does Nothing))"}});
  for (unsigned int i = 0; i < tests.size(); i++) {
    std::string result = TreeNode::parse(TreeNode::tokenize(tests[i].input))[0]
                             .getLispExpression();
    if (result != tests[i].expectedResult) {
      std::cerr << "Internal compiler error: Parser test failed: \""
                << tests[i].input << "\" parses into \"" << result
                << "\" instead of to \"" << tests[i].expectedResult << "\"!"
                << std::endl;
      std::exit(1);
    }
  }
}

void testAssemblyCodeStructure() {
  std::vector<test> tests({{"(i32.load\n\t(i32.const 32)\n)",
                            "\t(i32.load\n\t\t(i32.const 32)\n\t)"}});
  for (unsigned i = 0; i < tests.size(); i++) {
    std::string result = AssemblyCode(tests[i].input).indentBy(1);
    if (result != tests[i].expectedResult) {
      std::cerr << "Internal compiler error: Pretty-printer test failed: \""
                << tests[i].input << "\" prints into \"" << result
                << "\" instead of to \"" << tests[i].expectedResult << "\"!"
                << std::endl;
      std::exit(1);
    }
  }
}

void testTypeChecking() {
  std::vector<test> tests(
      {{"128;", "Integer64"},
       {"'A';", "Integer64"},
       {"3.14159265359;", "Decimal64"},
       {"0.142857;", "Decimal64"},
       {"1.;", "Decimal64"},
       {"55.;", "Decimal64"},
       {"0x1Ff;", "Integer64"},
       {"1+1;", "Integer64"},
       {"Integer32(1)+Integer32(1);", "Integer32"},
       {"3.14159265359-1.618033989;", "Decimal64"},
       {"CharacterPointer(10)-CharacterPointer(0);", "Integer32"},
       {"3.14159265359*1;", "Decimal64"},
       {"1/2;", "Integer64"},
       {"(5+5=10 and 2+2=4)?(3.14159265359):(0);", "Decimal64"},
       {"5+5=10 and 2+2=4;", "Integer32"},
       {"mod(Integer32(5),2);", "Integer64"},
       {"not(2+2=5);", "Integer32"}});
  for (unsigned i = 0; i < tests.size(); i++) {
    std::string result =
        TreeNode::parse(TreeNode::tokenize(tests[i].input))[0].getType(
            CompilationContext());
    if (result != tests[i].expectedResult) {
      std::cerr << "Internal compiler error: Compiler test failed: \""
                << tests[i].input << "\" compiles into something of the type "
                << result << "\" instead of into something of the type \""
                << tests[i].expectedResult << "\"!" << std::endl;
      std::exit(1);
    }
  }
}

void testBitManipulations() {
  if (getCharVectorRepresentationOfInteger64(255) !=
      "\"\\ff\\00\\00\\00\\00\\00\\00\\00\"") {
    std::cerr << "Internal compiler error: Bit manipulation test failed: \""
              << getCharVectorRepresentationOfInteger64(255) << "\"!"
              << std::endl;
    std::exit(1);
  }
  if (getCharVectorRepresentationOfInteger64(-1) !=
      "\"\\ff\\ff\\ff\\ff\\ff\\ff\\ff\\ff\"") {
    std::cerr
        << "Internal compiler error: Bit manipulation test failed. Your C++ "
           "compiler appears not to use double-complement for negative "
           "numbers. getCharVectorRepresentationOfInteger64(-1) gives \""
        << getCharVectorRepresentationOfInteger64(-1) << "\"!" << std::endl;
    std::exit(1);
  }
  if (getCharVectorRepresentationOfInteger32(-1) != "\"\\ff\\ff\\ff\\ff\"") {
    std::cerr
        << "Internal compiler error: Bit manipulation test failed. Your C++ "
           "compiler appears not to use double-complement for negative "
           "numbers. getCharVectorRepresentationOfInteger32(-1) gives \""
        << getCharVectorRepresentationOfInteger32(-1) << "\"!" << std::endl;
    std::exit(1);
  }
  if (getCharVectorRepresentationOfCharacter('A') != "\"\\41\"") {
    std::cerr << "Internal compiler error: Bit manipulation test failed. Your "
                 "C++ compiler appears not to use ASCII. "
                 "getCharVectorRepresentationOfCharacter(\'A\') gives \""
              << getCharVectorRepresentationOfCharacter('A') << "\"!"
              << std::endl;
    std::exit(1);
  }
  if (getCharVectorRepresentationOfInteger32(256) != "\"\\00\\01\\00\\00\"") {
    std::cerr
        << "Internal compiler error: Bit manipulation test failed. Your C++ "
           "compiler appears not to use double-complement for negative "
           "numbers. getCharVectorRepresentationOfInteger32(256) gives \""
        << getCharVectorRepresentationOfInteger32(256) << "\"!" << std::endl;
    std::exit(1);
  }
  if (getCharVectorRepresentationOfInteger32(4096) != "\"\\00\\10\\00\\00\"") {
    std::cerr
        << "Internal compiler error: Bit manipulation test failed. Your C++ "
           "compiler appears not to use double-complement for negative "
           "numbers. getCharVectorRepresentationOfInteger32(4096) gives \""
        << getCharVectorRepresentationOfInteger32(4096) << "\"!" << std::endl;
    std::exit(1);
  }
  if (getCharVectorRepresentationOfDecimal64(double(1) / 3) !=
      "\"\\55\\55\\55\\55\\55\\55\\d5\\3f\"") {
    std::cerr << "Internal compiler error: Bit manipulation test failed. Your "
                 "C++ compiler appears not to use IEEE754 standard for storing "
                 "decimal numbers, like JavaScript Virtual Machine does. "
                 "getCharVectorRepresentationOfDecimal64(double(1)/3) gives \""
              << getCharVectorRepresentationOfDecimal64(double(1) / 3) << "\"!"
              << std::endl;
    std::exit(1);
  }
}

void testMaps() {
  if (std::isnan(std::map<std::string, float>()[""]) or
      std::isnan(
          std::map<std::string, double>()[""])) // The Virtual DOS Machine on
                                                // ReactOS, when this program is
                                                // compiled with DJGPP 10.2.
    std::cerr
        << R"(Your C++ runtime environment apparently returns a NaN instead of 0 for
"std::map<std::string, float>()[""]". The program will continue, but be warned
it may produce wrong code or crash because of that.)"
        << std::endl;
}

void testLongestCommonSubsequence() {
  if (longest_common_subsequence_length(
          "ABCD",
          "ACBAD") // https://en.wikipedia.org/wiki/Longest_common_subsequence_problem
          != 3 ||
      longest_common_subsequence_length("", "") != 0 ||
      longest_common_subsequence_length("", "ABC") != 0 ||
      longest_common_subsequence_length("ABC", "") != 0) {
    std::cerr << "Internal compiler error: The function calculating the "
                 "longest common subsequence of strings seems not to work!"
              << std::endl;
    std::exit(1);
  }
  if (Levenstein_distance("", "") != 0 || Levenstein_distance("", "ABC") != 3 ||
      Levenstein_distance("ABC", "") != 3 ||
      Levenstein_distance(
          "kitten",
          "sitting") // https://en.wikipedia.org/wiki/Levenshtein_distance#Example
          != 3) {
    std::cerr << "Internal compiler error: The function calculating the "
                 "Levenstein Distance of strings seems not to work!"
              << std::endl;
    std::exit(1);
  }
}

void checkTypeId() {
  if (std::string(typeid(NotImplementedException()).name())
          .find("NotImplementedException") == std::string::npos) {
    std::cerr << "Internal Compiler Error: "
                 "typeid(NotImplementedException()).name()==\""
              << typeid(NotImplementedException()).name()
              << "\", which does not mention \"NotImplementedException\"?!"
              << std::endl;
    std::exit(1);
  }
  if (std::string(typeid(CorruptCompilationContextException()).name())
          .find("CorruptCompilationContextException") == std::string::npos) {
    std::cerr
        << "Internal Compiler Error: "
           "typeid(CorruptCompilationContextException()).name()==\""
        << typeid(CorruptCompilationContextException()).name()
        << "\", which does not mention \"CorruptCompilationContextException\"?!"
        << std::endl;
    std::exit(1);
  }
  if (std::string(typeid(InvalidTypenameException()).name())
          .find("InvalidTypenameException") == std::string::npos) {
    std::cerr << "Internal Compiler Error: "
                 "typeid(InvalidTypenameException()).name()==\""
              << typeid(InvalidTypenameException()).name()
              << "\", which does not mention \"InvalidTypenameException\"?!"
              << std::endl;
    std::exit(1);
  }
}

void runTests() {
  tokenizerTests();
  simpleParserTests();
  interpreterTests();
  parsingVariableDeclarationsTests();
  parserTests();
  testAssemblyCodeStructure();
  testTypeChecking();
  testBitManipulations();
  testMaps();
  testLongestCommonSubsequence();
  checkTypeId();
}
