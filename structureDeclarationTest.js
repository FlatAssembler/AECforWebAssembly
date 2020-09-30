const fs = require("fs");
const buffer = fs.readFileSync("structureDeclarationTest.wasm");
let memory = new WebAssembly.Memory({initial : 1});
let stack_pointer = new WebAssembly.Global({value : "i32", mutable : true}, 0);
let importObject = {
  JavaScript : {stack_pointer : stack_pointer, memory : memory},
};
WebAssembly.instantiate(buffer, importObject).then((results) => {
  const exports = results.instance.exports;
  if (exports.foo() == 3 && exports.bar() == 12 && exports.spam() == 3 &&
      exports.eggs() == 3 && exports.onion() == 1)
    console.log("Structure declaration test succeeded!");
  else
    console.log("Structure declaration test failed!\n" +
                "AEC function\tExpected Return Value\tActual Return Value\n" +
                "foo()\t3\t" + exports.foo() + "\n" +
                "bar()\t12\t" + exports.bar() + "\n" +
                "spam()\t3\t" + exports.spam() + "\n" +
                "eggs()\t3\t" + exports.eggs() + "\n" +
                "onion()\t1\t" + exports.onion() + "\n");
});
