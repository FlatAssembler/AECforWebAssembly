const fs = require("fs");
const buffer = fs.readFileSync("namedArgumentsTest.wasm");
WebAssembly
    .instantiate(
        buffer,
        {JavaScript : {printFloat32 : function(f32) { console.log(f32); }}})
    .then((results) => {
      const exports = results.instance.exports;
      if (exports.namedArgumentsTest() === 0) {
        process.exitCode = 0;
        console.log("Named Arguments Test succeeded!");
      } else {
        process.exitCode = exports.namedArgumentsTest();
        console.log(
            "Named Arguments Test failed, \"namedArgumentsTest\" returned the error code " +
            exports.namedArgumentsTest() + "!");
      }
    });
