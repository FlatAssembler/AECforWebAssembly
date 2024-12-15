const fs = require("fs");
const buffer = fs.readFileSync("typeOfTest.wasm");
WebAssembly.instantiate(buffer).then((results) => {
  const exports = results.instance.exports;
  if (exports.test()) {
    process.exitCode = 0;
    console.log("The TypeOf Test succeeded!");
  } else {
    process.exitCode = 1;
    console.log("The TypeOf Test failed!");
  }
});
