/*
 * So, this is an example of how to run WebAssembly programs produced by
 * AECtoWebAssembly on NodeJS
 */

if (typeof require === "undefined") // NodeJS has a "require" object,
                                    // other JavaScript environments
                                    // don't.
  // Now, some JavaScript environments don't support "console.log", but
  // they support "print". And there is no way to exit from a program
  // in standard JavaScript.
  (print || console.log)("Please run this program in NodeJS.");
else {
  const FileSystem = require("fs");
  let buffer;
  try {
    buffer = FileSystem.readFileSync("analogClock.wasm");
    /*
     * "Sync" means "do it in this thread, don't start a new thread".
     */
  } catch (error) {
    console.log("Can't open \"analogClock.wasm\": ", error.code);
    process.exit(1); // This is a way to exit a NodeJS program (doesn't work
                     // in other JavaScript environments.
  }
  if (!WebAssembly.validate(buffer)) { // Validate the WebAssembly, but
                                       // don't compile it yet.
    console.log(`The file "analogClock.wasm" doesn't appear to be a valid
WebAssembly dynamic-link library with which this JavaScript code can link,
quitting now!`);
    process.exit(1);
  }
  let memory;
  function logString(ptr) {} // Let's not implement the logging
                             // functionality here, we've already tested
                             // the code in the browser.
  function logInteger(integer) {}
  let importObject = {
    "wasi_unstable" : {
      logString : logString,   // If you don't provide those functions,
      logInteger : logInteger, // AEC program will crash when trying to
                               // call them.
    },
  };
  WebAssembly.instantiate(buffer, importObject).then((results) => {
    // Now the WebAssembly binary has been converted to the format
    // compatible with NodeJS JavaScript Virtual Machine Machine and
    // stored in memory where the pointer "results" points to.
    const exports = results.instance.exports;
    memory = exports.memory;
    // Now, let's import functions declared in the AEC program into
    // JavaScript. AEC compiler automatically "extern"s them, so that
    // they can be imported.
    const getAddressOfOutput = exports.getAddressOfOutput;
    const updateClockToTime = exports.updateClockToTime;
    let now = new Date();
    // Now, let's finally run the AEC program...
    updateClockToTime(now.getHours(), now.getMinutes(), now.getSeconds());
    // Now, the AEC program has finished (it runs in the same thread).
    let addressOfOutput = getAddressOfOutput(); // When I haven't
                                                // implemented "extern" for
                                                // global variables into
                                                // my AEC compiler, I need
                                                // to access the AEC global
                                                // variables from JS using
                                                // pointers.
    let ASCIIofOutput =
        new Uint8Array(memory.buffer, addressOfOutput,
                       80 * 23); // 80*23 is the size declared in AEC.
    const ASCIIdecoder = new TextDecoder();
    let outputAsString = ASCIIdecoder.decode(ASCIIofOutput);
    console.log(outputAsString); // Because JavaScript console.log directive
                                 // will print a bunch of numbers if you
                                 // provide ASCIIofOutput as an argument.
  });
}
