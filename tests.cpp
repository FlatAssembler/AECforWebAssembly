#include "TreeNode.cpp"

struct test { // When the debugger doesn't work, "cassert" doesn't help a lot.
  std::string input, expectedResult;
};

void tokenizerTests() {
  std::vector<test> tests(
      {{"\"/*Comment inside a string*/\"",
        "['\"/*Comment inside a string*/\"']"},
       {"5+5", "['5','+','5']"},
       {"5+5=10", "['5','+','5','=','10']"},
       {"structureName.structureMember/3.14159265359",
        "['structureName','.','structureMember','/','3.14159265359']"},
       {"sin(pi/2)=1", "['sin(','pi','/','2',')','=','1']"},
       {"'A'+2='C'", "['65','+','2','=','67']"},
       {"a:=a+1; //Some random comment.\nb:=b-1;",
        "['a',':=','a','+','1',';','b',':=','b','-','1',';']"},
       {"/*This should be tokenized into\nan empty string*/", "[]"},
       {"a/*Randomly\ninserted\ncomment.*/+/*Another\nrandom\ncomment*/b",
        "['a','+','b']"},
       {"array_name:={1,1+1,1+1+1}", "['array_name',':=','{','1',',','1','+','"
                                     "1',',','1','+','1','+','1','}']"}});
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
       {"(2+2>5?3+3<7?1:-2:2+2-4<1?0:2+2<4?-1:-3)+('A'+2='C'?0:-1)", "0"}});
  for (unsigned int i = 0; i < tests.size(); i++) {
    std::string result = std::to_string(
        TreeNode::parseExpression(TreeNode::tokenize(tests[i].input))[0]
            .interpretAsACompileTimeConstant());
    if (result != tests[i].expectedResult) {
      std::cerr << "Internal compiler error: Interpreter test failed: \""
                << tests[i].input << "\" interprets into \"" << result
                << "\" instead of to \"" << tests[i].expectedResult << "\"!"
                << std::endl;
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
       {"Function foo(Integer32 a:=0) Which Returns Nothing Does //Nonsense "
        "code, but useful to see if the parser is capable of parsing simple "
        "functions.\n"
        "Integer32 b;\n"
        "b:=a+1;\n"
        "EndFunction",
        "(Function (foo (Integer32 (a (:= 0)))) (Returns Nothing) (Does "
        "(Integer32 b) (:= b (+ a 1))))"}});
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

void runTests() {
  tokenizerTests();
  simpleParserTests();
  interpreterTests();
  parsingVariableDeclarationsTests();
  parserTests();
}
