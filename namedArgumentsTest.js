const fs = require("fs");
const buffer = fs.readFileSync("namedArgumentsTest.wasm");
WebAssembly.instantiate(buffer).then((results)=>{
  const exports = results.instance.exports;
  if (exports.namedArgumentsTest() === 1) {
    process.exitCode = 0;
    console.log("Named Arguments Test succeeded!");
  }
  else {
    process.exitCode = 1;
    console.log("Named Arguments Test failed!");
  }
});
