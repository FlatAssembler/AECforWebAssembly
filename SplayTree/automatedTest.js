'use strict';

const fs = require('fs');
const { SplayAPI } = require('./splay_api.js');

function assertEqual(actual, expected, message) {
  if (actual !== expected) {
    const err = new Error(
      `${message}\nExpected: ${JSON.stringify(expected)}\nActual:   ${JSON.stringify(actual)}`
    );
    err.actual = actual;
    err.expected = expected;
    throw err;
  }
}

function assert(condition, message) {
  if (!condition) throw new Error(message);
}

function readCString(mem, ptr) {
  const bytes = new Uint8Array(mem.buffer);
  let s = '';
  for (let i = ptr; bytes[i] !== 0; i++) {
    s += String.fromCharCode(bytes[i]);
  }
  return s;
}

async function instantiateWasm() {
  const wasmBytes = fs.readFileSync('SplayTree.wasm');

  const fatal = (name) => () => {
    throw new Error(`WASM called fatal import: ${name}`);
  };
  const noop = () => {};

  return WebAssembly.instantiate(wasmBytes, {
    wasi_unstable: {
      printString: noop,
      clearScreen: noop,
      drawRectangle: noop,
      drawLine: noop,
      drawText: noop,
      setDiagramWidth: noop,
      setDiagramHeight: noop,

      // Fail fast if these happen:
      noMoreFreeMemory: fatal('noMoreFreeMemory'),
      segmentationFault: fatal('segmentationFault'),

      // Inputs should not be used in automated tests:
      getLengthOfTheInput: fatal('getLengthOfTheInput'),
      getCharacterOfTheInput: fatal('getCharacterOfTheInput'),
    },
  });
}

function parseFirstIntFromTraversalString(traversal) {
  // traversal format: "7 4 2 ..."
  const firstToken = traversal.trim().split(/\s+/)[0];
  return firstToken.length ? parseInt(firstToken, 10) : NaN;
}

function getTraversals(api) {
  const mem = api.getMemory();
  const preorderPtr = Number(api.exports.getAddressOfPreorderTraversal());
  const inorderPtr = Number(api.exports.getAddressOfInorderTraversal());
  return {
    preorder: readCString(mem, preorderPtr),
    inorder: readCString(mem, inorderPtr),
  };
}

function runCase_InsertOnly(api) {
  api.clearKeys();
  api.pushKey(1);
  api.pushKey(2);
  api.pushKey(3);

  api.render();

  const { preorder, inorder } = getTraversals(api);

  assertEqual(inorder, '1 2 3 ', 'Inorder after inserts must be sorted');
  assert(preorder.length > 0, 'Preorder must be non-empty');
  assert(preorder.endsWith(' '), 'Preorder should end with a space (format consistency)');
}

function runCase_SearchSplaysToRoot(api) {
  api.clearKeys();
  // Build some tree
  api.pushKey(10);
  api.pushKey(5);
  api.pushKey(15);
  api.pushKey(3);
  api.pushKey(7);

  // Now search; should splay searched key to root.
  api.searchKey(3);

  api.render();

  const { preorder, inorder } = getTraversals(api);

  assertEqual(inorder, '3 5 7 10 15 ', 'Inorder must match expected set');
  const root = parseFirstIntFromTraversalString(preorder);
  assertEqual(root, 3, 'After searching for 3, root (first in preorder) should be 3');
}

function runCase_Delete(api) {
  api.clearKeys();
  api.pushKey(4);
  api.pushKey(2);
  api.pushKey(6);
  api.pushKey(1);
  api.pushKey(3);
  api.pushKey(5);
  api.pushKey(7);

  api.deleteKey(4); // delete something that likely becomes root sometimes

  api.render();

  const { inorder } = getTraversals(api);
  assertEqual(inorder, '1 2 3 5 6 7 ', 'Inorder after delete must match expected set');
}

async function main() {
  const instantiateResult = await instantiateWasm();
  const api = await SplayAPI.init(instantiateResult);

  runCase_InsertOnly(api);
  runCase_SearchSplaysToRoot(api);
  runCase_Delete(api);

  console.log('All SplayTree tests passed.');
}

main().catch((err) => {
  console.error('Test failed:');
  console.error(err && err.stack ? err.stack : err);
  process.exitCode = 1;
});
