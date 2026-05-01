// It has happened to me that I screw something up while manually testing the
// Huffman Coding in AEC, so I have decided I will write a script that will do
// that for me.

"use strict";

const fs = require("fs");
const path = require("path");

const wasmPath = path.join(__dirname, "HuffmanCodingInAEC.wasm");
const wasmBytes = fs.readFileSync(wasmPath);

let text = "";
let memory;

function fail(message) {
  console.error(message);
  process.exit(1);
}

// ASCII C-string reader from WASM memory (null-terminated).
function readCString(ptr, { maxBytes = 1_000_000 } = {}) {
  if (!memory) throw new Error("WASM memory not initialized yet.");

  const mem8 = new Uint8Array(memory.buffer);

  if (!Number.isInteger(ptr) || ptr < 0 || ptr >= mem8.length) {
    throw new Error(`Invalid pointer: ${ptr}`);
  }

  let str = "";
  for (let i = 0; i < maxBytes; i++) {
    const p = ptr + i;
    if (p >= mem8.length) {
      throw new Error("Unterminated string (hit end of memory).");
    }
    const b = mem8[p];
    if (b === 0) return str;
    str += String.fromCharCode(b); // OK: output is strictly ASCII.
  }

  throw new Error(`Unterminated string (exceeded maxBytes=${maxBytes}).`);
}

function firstDiffIndex(a, b) {
  const n = Math.min(a.length, b.length);
  for (let i = 0; i < n; i++) {
    if (a[i] !== b[i]) return i;
  }
  return a.length === b.length ? -1 : n;
}

function assertEqual(actual, expected, label) {
  if (actual === expected) return;

  const i = firstDiffIndex(actual, expected);
  console.error(`The AEC program has failed the test: ${label}`);
  console.error(`Expected length: ${expected.length}, actual length: ${actual.length}`);

  if (i >= 0) {
    const start = Math.max(0, i - 30);
    const end = i + 30;
    console.error(`First difference at index ${i}`);
    console.error(`Expected slice: ${JSON.stringify(expected.slice(start, end))}`);
    console.error(`Actual slice:   ${JSON.stringify(actual.slice(start, end))}`);
  }

  process.exit(1); // fail fast
}

function assertAlmostEqual(actual, expected, epsilon, label) {
  if (Number.isNaN(actual) || Math.abs(actual - expected) > epsilon) {
    fail(`${label}: got ${actual}, expected ${expected} (epsilon=${epsilon})`);
  }
}

// WASI imports (keep wasi_unstable as requested)
function printString(ptr) {
  console.log(readCString(ptr));
}
function clearScreen() {}
function noMoreFreeMemory() {
  fail("The AEC program reports to have run out of memory!");
}
function segmentationFault() {
  fail("The AEC program reported to have experienced a segmentation fault!");
}
function getLengthOfTheInput() {
  return text.length;
}
function getCharacterOfTheInput(i) {
  return text.charCodeAt(i);
}

const importObject = {
  wasi_unstable: {
    printString,
    clearScreen,
    noMoreFreeMemory,
    segmentationFault,
    getLengthOfTheInput,
    getCharacterOfTheInput,
    drawRectangle() {},
    drawLine() {},
    drawText() {},
    setDiagramWidth() {},
    setDiagramHeight() {},
  },
};

async function main() {
  const { instance } = await WebAssembly.instantiate(wasmBytes, importObject);
  const exports = instance.exports;
  memory = exports.memory;

  console.log("The AEC program has been loaded successfully!");

  // Sanity check (fail fast). Tighten/adjust epsilon based on expected precision.
  assertAlmostEqual(exports.log2(27), Math.log2(27), 1e-2, "log2(27) mismatch");

  const tests = [
    {
      input: "TEO SAMARZIJA",
      expected: "10001001101010111100011101011110111100000101\n",
    },
    {
      input: "HENDRIK WADE BODE",
      expected: "01001100101111011001111000001100110101111100011011000111110\n",
    },
    {
      input: "NO MAN IS AS BLOODTHIRSTY AS CAT",
      expected:
        "10000001111010001010001111001011111010011111101011011000000010111001110001001110010110011101011101001111111011010001\n",
    },
    {
      input: "NIJEDAN COVJEK NIJE BAS TOLIKO KRVOLOCAN KAO STO JE MACKA",
      expected: "1000010010011010110100001100011101010111011010011010000111100001001001101011111010100110111111110000111100101000000111110001101101011001111001011010100110001110000010111111011111000011111100110101111101110010101000001\n",
    },
    {
      input: "KARASICA DOLAZI OD ILIRSKOG *KURR-URR-ISSIA (TECI-VODA-SUFIKS)",
      expected: "100011110001111001011010111111110111101100110110111110111001111011001111011101011101100110000011000100110111111011100001000010000000010100100000000101001100100101111111101110001110010110011010110111010111000100111101111110100010100111001011100000101010\n",
    },
  ];

  for (const t of tests) {
    text = t.input;
    console.log("Testing with the text: " + text);

    exports.main();

    const outPtr = exports.getAddressOfTheOutput();
    const actual = readCString(outPtr);

    assertEqual(actual, t.expected, JSON.stringify(t.input));
    console.log("The AEC program has passed the test!");
  }

  console.log("All tests passed.");
}

main().catch((e) => {
  console.error(e && e.stack ? e.stack : String(e));
  process.exit(1);
});
