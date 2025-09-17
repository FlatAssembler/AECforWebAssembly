/*
 * This is a NodeJS script which reads an AEC file and outputs a syntax
 * highlighted code in form of a HTML file. Might come useful if I need
 * to publish a paper including code in the new version of AEC. The
 * same code is used in the script in "AEC_specification.html", so it's
 * written to be compatible with as many browsers as possible, rather
 * than to follow the best software engineering practices.
 */
"use strict";
function highlightedToken(token) {
  if (token == "<")
    token = "&lt;";
  if (token == ">")
    token = "&gt;";
  if (token.substr(0, 2) == "//" || token.substr(0, 2) == "/*")
    return '<span class="Comment">' + token + "</span>";
  if (token.charAt(0) == '"' || token.charAt(0) == "'")
    return ('<span class="String">' +
            token.replace(/\\n/g, "<b>\\<i>n</i></b>")
                .replace(/\\\\/g, "<b>\\<i>\\</i></b>")
                .replace(/\\t/g, "<b>\\<i>t</i></b>")
                .replace(/\\\"/g, "<b>\\<i>\"</i></b>") +
            "</span>");
  if (token.substr(0, 2) == 'R"')
    return ('<span class="String">' +
            token.replace(/\n/g, '</span>\n<span class="String">') + "</span>");
  if (token == "Character" || token == "CharacterPointer" ||
      token == "Integer16" || token == "Integer16Pointer" ||
      token == "Integer32" || token == "Integer32Pointer" ||
      token == "Integer64" || token == "Integer64Pointer" ||
      token == "Decimal32" || token == "Decimal64" ||
      token == "Decimal32Pointer" || token == "Decimal64Pointer" ||
      token == "CharacterPointerPointer" || token == "DataType" ||
      token == "Nothing" || token == "InstantiateStructure" ||
      /Pointer$/.test(token) || /^PointerTo/.test(token))
    return '<span class="Type">' + token + "</span>";
  if (token.charAt(0) >= "0" && token.charAt(0) <= "9")
    return '<span class="Constant">' + token + "</span>";
  if (token == "Function" || token == "Which" || token == "Returns" ||
      token == "Is" || token == "External" || token == "Declared" ||
      token == "Does" || token == "EndFunction" || token == "Return" ||
      token == "If" || token == "ElseIf" || token == "Else" ||
      token == "EndIf" || token == "Then" || token == "While" ||
      token == "EndWhile" || token == "Loop" || token == ";" ||
      token == "Structure" || token == "Consists" || token == "Of" ||
      token == "EndStructure" || token == "asm" || token == "asm_i32" ||
      token == "asm_i64" || token == "asm_f32" || token == "asm_f64" ||
      token == "Break" || token == "Continue")
    return '<span class="Keyword">' + token + "</span>";
  if (token == "and" || token == "or" || token == "mod" ||
      token == "AddressOf" || token == "ValueAt" || token == "," ||
      token == "+" || token == "-" || token == "/" || token == "*" ||
      token == "?" || token == ":" || token == "=" || token == "&lt;" ||
      token == "&gt;" || token == "not" || token == "invertBits" ||
      token == "SizeOf")
    return '<span class="Operator">' + token + "</span>";
  if (token == "{" || token == "}" || token == "(" || token == ")" ||
      token == "[" || token == "]")
    return '<span class="Parenthesis">' + token + "</span>";
  return token;
}
function highlightAEC(sourceCode) {
  var currentToken = "", areWeInAString = false, areWeInAComment = false,
      howTheCommentStarted, howTheStringStarted, highlightedCode = "";
  for (var i = 0; i < sourceCode.length; i++) {
    if (!areWeInAComment && !areWeInAString &&
        (sourceCode.substr(i, 2) == "//" || sourceCode.substr(i, 2) == "/*")) {
      areWeInAComment = true;
      howTheCommentStarted = sourceCode.substr(i, 2);
      highlightedCode += highlightedToken(currentToken);
      currentToken = howTheCommentStarted;
      i++;
      continue;
    }
    if (areWeInAComment &&
        ((howTheCommentStarted == "//" && sourceCode.charAt(i) == "\n") ||
         (howTheCommentStarted == "/*" && sourceCode.substr(i, 2) == "*/"))) {
      areWeInAComment = false;
      if (howTheCommentStarted == "//")
        currentToken += "\n";
      else
        currentToken += "*/";
      if (howTheCommentStarted == "/*")
        i++;
      highlightedCode += highlightedToken(currentToken);
      currentToken = "";
      continue;
    }
    if (areWeInAComment) {
      currentToken += sourceCode.charAt(i);
      continue;
    }
    if (!areWeInAString &&
        (sourceCode.charAt(i) == '"' || sourceCode.charAt(i) == "'" ||
         sourceCode.substr(i, 2) == 'R"')) {
      areWeInAString = true;
      if (sourceCode.substr(i, 2) == 'R"') {
        howTheStringStarted = "";
        var whereIsTheOpenParenthesis = i + 2;
        while (sourceCode.charAt(whereIsTheOpenParenthesis) != '(') {
          howTheStringStarted += sourceCode.charAt(whereIsTheOpenParenthesis);
          whereIsTheOpenParenthesis++;
        }
        howTheStringStarted = ')' + howTheStringStarted + '"';
      } else
        howTheStringStarted = sourceCode.charAt(i);
      highlightedCode += highlightedToken(currentToken);
      currentToken = sourceCode.charAt(i);
      continue;
    }
    if (areWeInAString &&
        (currentToken.charAt(currentToken.length - 1) != "\\" ||
         (currentToken.charAt(currentToken.length - 2) == "\\" &&
          currentToken.charAt(currentToken.length - 3) != "\\")) &&
        sourceCode.substr(i, howTheStringStarted.length) ==
            howTheStringStarted) {
      areWeInAString = false;
      currentToken += howTheStringStarted;
      i += howTheStringStarted.length - 1;
      highlightedCode += highlightedToken(currentToken);
      currentToken = "";
      continue;
    }
    if (areWeInAString) {
      currentToken += sourceCode.charAt(i);
      continue;
    }
    if ((sourceCode.charAt(i) >= "0" && sourceCode.charAt(i) <= "9") ||
        (sourceCode.charAt(i) >= "a" && sourceCode.charAt(i) <= "z") ||
        (sourceCode.charAt(i) >= "A" && sourceCode.charAt(i) <= "Z") ||
        sourceCode.charAt(i) == "_" || sourceCode.charAt(i) == ".") {
      currentToken += sourceCode.charAt(i);
      continue;
    }
    highlightedCode +=
        highlightedToken(currentToken) + highlightedToken(sourceCode.charAt(i));
    currentToken = "";
  }
  return highlightedCode + highlightedToken(currentToken);
}
if (typeof require == "undefined")
  (print || console.log)("Please run this program in NodeJS!");
else if (process.argv.length != 3 || !/\.AEC$/i.test(process.argv[2])) {
  console.log(`Please invoke this program as follows:
"${process.argv[0]}" "${process.argv[1]}" name_of_the_AEC_program.aec
`);
} else {
  const FileSystem = require("fs");
  let originalCode;
  try {
    originalCode =
        FileSystem.readFileSync(process.argv[2], "utf8").replace(/\r/g, "");
  } catch (error) {
    console.log(`Can't open the file "${process.argv[2]}" for reading! `,
                error.message);
  }
  if (originalCode != undefined) {
    // If reading the input file succeeded.
    const bodyWithoutLineNumbers = highlightAEC(originalCode);
    const bodySplitByLines = bodyWithoutLineNumbers.split("\n");
    let body = "";
    for (let i = 0; i < bodySplitByLines.length; i++) {
      if (i + 1 < 10)
        body += "   " +
                '<span class="LineNumber">' + (i + 1) + "</span> " +
                bodySplitByLines[i] + "\n";
      else if (i + 1 < 100)
        body += "  " +
                '<span class="LineNumber">' + (i + 1) + "</span> " +
                bodySplitByLines[i] + "\n";
      else if (i + 1 < 1000)
        body += " " +
                '<span class="LineNumber">' + (i + 1) + "</span> " +
                bodySplitByLines[i] + "\n";
      else
        body += '<span class="LineNumber">' + (i + 1) + "</span> " +
                bodySplitByLines[i] + "\n";
    }
    const html = `
<!doctype html>
<html>
<head>
    <title>${process.argv[2]} - Syntax Highlighted in NodeJS</title>
    <style type="text/css">
    pre {
       font-size: 13px;
    }
    .LineNumber {
       color: #772200;
       font-weight: normal;
    }
    .Type {
       color: #775500;
       font-weight: bold;
    }
    .Keyword {
        color: #770000;
        font-weight: bold;
    }
    .Comment {
        color: #004444;
        font-weight: bold;
    }
    .Parenthesis {
        color: #333322;
        font-weight: bold;
    }
    .Operator {
        color: #770077;
        font-weight: bold;
    }
    .Constant {
        color: #006600;
        font-weight: bold;
    }
    .String {
        color: #003377;
    }
    </style>
</head>
<body>
<pre>${body}</pre>
</body>
</html>
`;
    try {
      FileSystem.writeFileSync(process.argv[2] + ".html", html);
    } catch (error) {
      console.log('Unable to open the file "' + process.argv[2] +
                      '.html" for output! ',
                  error.message);
    }
  }
}
