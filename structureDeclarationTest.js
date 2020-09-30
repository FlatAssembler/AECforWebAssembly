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
      exports.eggs() == 3)
    console.log("Structure declaration test succeeded!");
  else
    console.log("Structure declaration test failed!");
});
