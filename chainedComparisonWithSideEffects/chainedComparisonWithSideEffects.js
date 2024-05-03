const fs = require("fs");
const buffer = fs.readFileSync("chainedComparisonWithSideEffects.wasm");
WebAssembly.instantiate(buffer).then((results) => {
  const exports = results.instance.exports;
  console.log("The test returns: " + exports.test());
});
