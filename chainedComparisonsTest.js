const fs = require("fs");
const buffer = fs.readFileSync("chainedComparisonsTest.wasm");
WebAssembly.instantiate(buffer).then((results) => {
  const exports = results.instance.exports;
  if (exports.testChainedComparisons()) {
    process.exitCode = 0;
    console.log("Chained Comparisons Test succeeded!");
  } else {
    process.exitCode = 1;
    console.log("Chained Comparisons Test failed!");
  }
});
