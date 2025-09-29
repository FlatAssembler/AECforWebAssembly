const fs = require("fs");
const buffer = fs.readFileSync("alignedAccessTest.wasm");
WebAssembly.instantiate(buffer).then((results) => {
  const exports = results.instance.exports;
  if (exports.test() === 1) {
    process.exitCode = 0;
    console.log("Aligned Access Test succeeded!");
  } else {
    process.exitCode = 1;
    console.log("Aligned Access Test failed!");
  }
});
