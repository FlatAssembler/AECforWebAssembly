const fs = require("fs");
const buffer = fs.readFileSync("break_and_continue_test.wasm");
WebAssembly.instantiate(buffer).then((results) => {
  const exports = results.instance.exports;
  if (exports.test() === 124 && exports.issue25() === 1) {
    process.exitCode = 0;
    console.log("Break and Continue Test succeeded!");
  } else {
    process.exitCode = 1;
    console.log("Break and Continue Test failed!");
  }
});
