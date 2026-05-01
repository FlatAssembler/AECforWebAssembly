"use strict";

const avl_api = require("./avl_api.js");
const fs = require("fs");
const path = require("path");

function makeImports() {
  const noop = function () {};
  return {
    wasi_unstable: {
      printString: noop,
      clearScreen: noop,
      noMoreFreeMemory: noop,
      segmentationFault: noop,
      getLengthOfTheInput: noop,
      getCharacterOfTheInput: noop,
      drawRectangle: noop,
      drawLine: noop,
      drawText: noop,
      setDiagramWidth: noop,
      setDiagramHeight: noop,
    },
  };
}

function readCString(memoryU8, ptr, maxLen = 1000000) {
  if (!Number.isInteger(ptr) || ptr < 0 || ptr >= memoryU8.length) {
    throw new Error("Invalid pointer returned from WASM: " + ptr);
  }

  let end = ptr;
  let limit = ptr + maxLen;
  if (limit > memoryU8.length) limit = memoryU8.length;

  while (end < limit && memoryU8[end] !== 0) end++;

  if (end === limit) {
    throw new Error(
      "CString not terminated within " +
        maxLen +
        " bytes (ptr=" +
        ptr +
        ", mem=" +
        memoryU8.length +
        ")"
    );
  }

  return Buffer.from(memoryU8.slice(ptr, end)).toString("utf8");
}

async function main() {
  const wasmPath =
    process.argv[2] ||
    process.env.AVL_WASM_PATH ||
    path.join(__dirname, "AVLInAEC.wasm");

  if (!fs.existsSync(wasmPath)) {
    process.exitCode = 1;
    console.log("WASM file not found: " + wasmPath);
    return;
  }

  const bytes = fs.readFileSync(wasmPath);

  const result = await WebAssembly.instantiate(bytes, makeImports());
  const instance = result.instance || result; // Node gives { module, instance }

  const avlapi = await avl_api.AVLAPI.init(instance);

  const keysToInsert = [4, 1, 9, 0, 3, 7, 10, 2, 5, 8, 11, 6];
  for (const k of keysToInsert) avlapi.pushKey(k);

  avlapi.deleteKey(
    0
  ); // Should trigger four rotations (max rotations per deletion in AVL tree).

  avlapi.render();

  const memory = new Uint8Array(avlapi.getMemory().buffer);
  const pointer = avlapi.exports.getAddressOfPreorderTraversal();
  const preorder = readCString(memory, pointer);

  const expected = "7 4 2 1 3 5 6 9 8 10 11 ";

  if (preorder === expected) {
    process.exitCode = 0;
    console.log("The test succeeded!");
  } else {
    process.exitCode = 1;
    console.log("The test failed!");
    console.log("Expected: " + JSON.stringify(expected));
    console.log("Actual:   " + JSON.stringify(preorder));
    console.log("Pointer:  " + pointer);
  }
}

main().catch((err) => {
  process.exitCode = 1;
  console.log(err && err.stack ? err.stack : String(err));
});
