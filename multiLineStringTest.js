const fs = require("fs");
const buffer = fs.readFileSync("multiLineStringTest.wasm");
WebAssembly.instantiate(buffer).then((results) => {
  const exports = results.instance.exports;
  if (exports.multiLineStringTest() == 1) {
    process.exitCode = 0;
    console.log("Multi-Line String Test succeeded!");
  } else {
    process.exitCode = 1;
    console.log("Multi-Line String Test failed!");
  }
});
