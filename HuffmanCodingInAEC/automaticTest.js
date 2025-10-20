// It has happened to me that I screw something up while manually testing the
// Huffman Coding in AEC, so I have decided I will write a script that will do
// that for me.
const fs = require("fs");
const buffer = fs.readFileSync("HuffmanCodingInAEC.wasm");
let text = "";
let memory;
function printString(ptr) {
  let buffer = new Uint8Array(memory.buffer);
  let str = "";
  while (buffer[ptr]) {
    str += String.fromCharCode(buffer[ptr]);
    ptr++;
  }
  console.log(str);
}
function clearScreen() {}
function noMoreFreeMemory() {
  console.log("The AEC program reports to have run out of memory!");
}
function segmentationFault() {
  console.log(
      "The AEC program reported to have experienced a segmentation fault!",
  );
}
function getLengthOfTheInput() { return text.length; }
function getCharacterOfTheInput(i) { return text.charCodeAt(i); }
let importObject = {
  wasi_unstable : {
    printString : printString,
    clearScreen : clearScreen,
    noMoreFreeMemory : noMoreFreeMemory,
    segmentationFault : segmentationFault,
    getLengthOfTheInput : getLengthOfTheInput,
    getCharacterOfTheInput : getCharacterOfTheInput,
    drawRectangle : function() {},
    drawLine : function() {},
    drawText : function() {},
    setDiagramWidth : function() {},
    setDiagramHeight : function() {},
  },
};
WebAssembly.instantiate(buffer, importObject).then((result) => {
  let exports = result.instance.exports;
  memory = exports.memory;
  console.log("The AEC program has been loaded successfully!");

  text = "TEO SAMARZIJA";
  console.log("Testing with the text: " + text);
  exports.main();
  console.log("The AEC program has finished its execution!");
  let buffer = new Uint8Array(memory.buffer);
  let str = "";
  let ptr = exports.getAddressOfTheOutput();
  while (buffer[ptr]) {
    str += String.fromCharCode(buffer[ptr]);
    ptr++;
  }
  if (str === "10001001101010111100011101011110111100000101\n") {
    console.log("The AEC program has passed the test!");
  } else {
    console.log("The AEC program has failed the test!");
    process.exit(1);
  }

  text = "HENDRIK WADE BODE";
  console.log("Testing with the text: " + text);
  exports.main();
  console.log("The AEC program has finished its execution!");
  str = "";
  ptr = exports.getAddressOfTheOutput();
  while (buffer[ptr]) {
    str += String.fromCharCode(buffer[ptr]);
    ptr++;
  }
  if (str === "01001100101111011001111000001100110101111100011011000111110\n") {
    console.log("The AEC program has passed the test!");
  } else {
    console.log("The AEC program has failed the test!");
    process.exit(1);
  }

  text = "NO MAN IS AS BLOODTHIRSTY AS CAT";
  console.log("Testing with the text: " + text);
  exports.main();
  console.log("The AEC program has finished its execution!");
  str = "";
  ptr = exports.getAddressOfTheOutput();
  while (buffer[ptr]) {
    str += String.fromCharCode(buffer[ptr]);
    ptr++;
  }
  if (str ===
      "10000001111010001010001111001011111010011111101011011000000010111001110001001110010110011101011101001111111011010001\n") {
    console.log("The AEC program has passed the test!");
  } else {
    console.log("The AEC program has failed the test!");
    process.exit(1);
  }

  text = "NIJEDAN COVJEK NIJE BAS TOLIKO KRVOLOCAN KAO STO JE MACKA";
  console.log("Testing with the text: " + text);
  exports.main();
  console.log("The AEC program has finished its execution!");
  str = "";
  ptr = exports.getAddressOfTheOutput();
  while (buffer[ptr]) {
    str += String.fromCharCode(buffer[ptr]);
    ptr++;
  }
  if (str ===
      "1000010010011010110100001100011101010111011010011010000111100001001001101011111010100110111111110000111100101000000111110001101101011001111001011010100110001110000010111111011111000011111100110101111101110010101000001\n") {
    console.log("The AEC program has passed the test!");
  } else {
    console.log("The AEC program has failed the test!");
    process.exit(1);
  }

  text = "KARASICA DOLAZI OD ILIRSKOG *KURR-URR-ISSIA (TECI-VODA-SUFIKS)";
  console.log("Testing with the text: " + text);
  exports.main();
  console.log("The AEC program has finished its execution!");
  str = "";
  ptr = exports.getAddressOfTheOutput();
  while (buffer[ptr]) {
    str += String.fromCharCode(buffer[ptr]);
    ptr++;
  }
  if (str ===
      "100011110001111001011010111111110111101100110110111110111001111011001111011101011101100110000011000100110111111011100001000010000000010100100000000101001100100101111111101110001110010110011010110111010111000100111101111110100010100111001011100000101010\n") {
    console.log("The AEC program has passed the test!");
  } else {
    console.log("The AEC program has failed the test!");
    process.exit(1);
  }
});
