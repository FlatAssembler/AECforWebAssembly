/*
 * Ovo je JavaScriptska ovojnica oko programa "havlik.js", koju treba
 * pokrenuti u NodeJS-u da inicijalizira JavaScriptinu virtualnu
 * mašinu, tako da se glavni program pisan u AEC-u može pokrenuti.
 */

if (typeof require === "undefined") // NodeJS ima globalni objekt
                                    // "require" pomoću kojeg se mogu
                                    // dohvatiti moduli za, primjerice,
                                    // pristup datotečnom sustavu.
                                    // Drugi JavaScript enginei to nemaju.
  // Sad, većina JavaScript enginea za ispis na standardni izlaz podržava
  // naredbu "console.log", ali neki umjesto toga (Duktape, Rhino) koriste
  // "print".
  (print || console.log)("Please run this program in NodeJS.");
else {
  const FileSystem = require("fs");
  let buffer;
  try {
    buffer = FileSystem.readFileSync("havlik.wasm");
    /*
     * "Sync" znači "radi to u ovoj dretvi, nemoj napraviti drugu dretvu".
     */
  } catch (error) {
    console.log("Can't open \"havlik.wasm\": ", error.code);
    process.exit(1); // Ovaj način izlaska iz JavaScript programa specifičan
                     // je za NodeJS, i ne funkcionira u drugim JavaScript
                     // engineima.
  }
  if (!WebAssembly.validate(buffer)) { // Prije no što pokušaš pokrenuti
                                       // neki JavaScript Bytecode
                                       // (WebAssembly), moraš provjeriti
                                       // je li ispravan.
    console.log(`The file "havlik.wasm" doesn't appear to be a valid
WebAssembly dynamic-link library with which this JavaScript code can link,
quitting now!`);
    process.exit(1);
  }
  const stack_pointer =
      new WebAssembly.Global({value : 'i32', mutable : true}, 0);
  let memory = new WebAssembly.Memory({initial : 1});
  function printString(ptr) {
    let buffer = new Uint8Array(memory.buffer);
    let str = "";
    while (buffer[ptr]) {
      str += String.fromCharCode(buffer[ptr]);
      ptr++;
    }
    console.log(str);
  }
  function printInteger(integer) { console.log(integer); }
  let importObject = {
    JavaScript : {
      stack_pointer : stack_pointer,
      memory : memory,
      printString : printString,
      printInteger : printInteger,
      random : Math.random,
    },
  };
  WebAssembly.instantiate(buffer, importObject).then((results) => {
    // Sada se je WebAssembly binarna datoteka pretvorila u format
    // kompatibilan sa NodeJS-ovom JavaScript virtualnom mašinom i
    // pohranjen je u memoriji gdje pokazivač "results" pokazuje.
    const exports = results.instance.exports;
    // I, sada ćemo u JavaScript uvesti AEC-ovu funkciju "glavni_program"
    // (koju moj compiler automatski externa) i pokrenut ćemo je.
    const glavni_program = exports.glavni_program;
    glavni_program();
  });
}
