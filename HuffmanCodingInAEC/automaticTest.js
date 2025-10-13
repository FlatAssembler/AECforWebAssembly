// It has happened to me that I screw something up while manually testing the
// Huffman Coding in AEC, so I have decided I will write a script that will do
// that for me.
const fs = require("fs");
const buffer = fs.readFileSync("HuffmanCodingInAEC.wasm");
const stack_pointer = new WebAssembly.Global(
    {value : "i32", mutable : true},
    0,
);
let text = "";
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
  JavaScript : {
    stack_pointer : stack_pointer,
    memory : memory,
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
});
