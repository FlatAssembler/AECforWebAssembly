const fs = require("fs");
const buffer = fs.readFileSync("leftHandSideConditionalTest.wasm");
WebAssembly.instantiate(buffer).then((results) => {
  const exports = results.instance.exports;
  if (exports.gcd(25, 80) === 5 && exports.gcd(80, 25) === 5 &&
      exports.gcd(8, 12) === 4 && exports.gcd(12, 8) === 4 &&
      exports.gcd(54, 24) === 6 && exports.gcd(48, 18) === 6) {
    process.exitCode = 0;
    console.log("Left Hand Side Conditional Test succeeded!");
  } else {
    process.exitCode = 1;
    console.log("Left Hand Side Conditional Test failed!");
  }
});
