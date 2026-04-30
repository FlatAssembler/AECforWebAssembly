'use strict';

const { RBTAPI } = require('./rbt_api.js');
const fs = require('fs');

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

function mulberry32(seed) {
  // Small deterministic RNG for reproducible randomized tests.
  return function () {
    let t = (seed += 0x6D2B79F5);
    t = Math.imul(t ^ (t >>> 15), t | 1);
    t ^= t + Math.imul(t ^ (t >>> 7), t | 61);
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

function shuffle(array, rng) {
  for (let i = array.length - 1; i > 0; i--) {
    const j = Math.floor(rng() * (i + 1));
    [array[i], array[j]] = [array[j], array[i]];
  }
  return array;
}

async function instantiateWasm() {
  const wasmBytes = fs.readFileSync('RedBlackTree.wasm');

  // Important: make "fatal" imports crash the test immediately.
  const fatal = (name) => () => {
    throw new Error(`WASM called fatal import: ${name}`);
  };

  // Keep no-op for UI-ish functions, but consider also failing them if you want strictness.
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

      // If these happen, the test should fail:
      noMoreFreeMemory: fatal('noMoreFreeMemory'),
      segmentationFault: fatal('segmentationFault'),

      // Input not used in tests; if it gets called unexpectedly, fail:
      getLengthOfTheInput: fatal('getLengthOfTheInput'),
      getCharacterOfTheInput: fatal('getCharacterOfTheInput'),
    },
  });
}

function runGoldenPreorderTests(rbtapi) {
  const cases = [
    {
      keys: [1, 2, 3],
      preorder: '2 1 3 ',
      name: 'simple 3-node',
    },
    {
      keys: [1, 3, 2, 5, 4, -1, 0],
      preorder: '2 0 -1 1 4 3 5 ',
      name: 'mixed with negatives',
    },
    // Add a few more deterministic cases (you can adjust expected strings as needed):
    // NOTE: these are placeholders until you compute the expected preorder from your implementation.
    // {
    //   keys: [10, 20, 30, 40, 50, 25],
    //   preorder: '...',
    //   name: 'classic RB-tree rotation scenario',
    // },
  ];

  for (const tc of cases) {
    const preorder = rbtapi.setKeysRenderAndGetPreorder(tc.keys);
    assertEqual(preorder, tc.preorder, `Golden preorder test failed: ${tc.name} keys=${tc.keys.join(',')}`);
  }
}

function runRandomizedPreorderStabilityTests(rbtapi) {
  // With only preorder available, we can still do useful randomized testing:
  // - Check that repeated runs on the same sequence are stable/deterministic.
  // - Optionally check that inserting a permutation of the same set doesn't crash
  //   and returns a non-empty traversal string.
  //
  // If you later add validateRBTree() export, we can check invariants (see below).

  const rng = mulberry32(0xC0FFEE);

  const trials = 200;
  const n = 40;

  for (let t = 0; t < trials; t++) {
    const base = [];
    for (let i = 0; i < n; i++) base.push(i - Math.floor(n / 2)); // includes negatives

    const keys = shuffle(base.slice(), rng);

    const preorder1 = rbtapi.setKeysRenderAndGetPreorder(keys);
    const preorder2 = rbtapi.setKeysRenderAndGetPreorder(keys);

    // Determinism check: same input should give same output (catches use-after-free / uninit memory / nondeterminism)
    assertEqual(preorder2, preorder1, `Determinism failed for trial ${t}`);

    // Basic sanity: should contain at least one number + space if n>0.
    assert(preorder1.length > 0, `Empty preorder for trial ${t}`);

    // Optional: ensure it ends with a space if that is the convention of your traversal buffer.
    assert(preorder1.endsWith(' '), `Preorder did not end with space for trial ${t}: ${JSON.stringify(preorder1)}`);
  }
}

function runOptionalInvariantValidation(rbtapi) {
  // If you add an exported function in WASM later, this will start working automatically.
  // Example export name: validateRBTree (returns 1 if OK, 0 if violation)
  const ex = rbtapi.exports;

  if (typeof ex.validateRBTree === 'function') {
    const rng = mulberry32(12345);
    const trials = 200;
    const n = 80;

    for (let t = 0; t < trials; t++) {
      const keys = [];
      for (let i = 0; i < n; i++) keys.push(i + 1);
      shuffle(keys, rng);

      rbtapi.setKeys(keys);
      rbtapi.render();

      const ok = ex.validateRBTree();
      assert(ok === 1, `validateRBTree() failed on trial ${t}`);
    }
  }
}

async function main() {
  const instantiateResult = await instantiateWasm();
  const rbtapi = await RBTAPI.init(instantiateResult);

  runGoldenPreorderTests(rbtapi);
  runRandomizedPreorderStabilityTests(rbtapi);
  runOptionalInvariantValidation(rbtapi);

  console.log('All tests passed.');
}

main().catch((err) => {
  console.error('Test failed:');
  console.error(err && err.stack ? err.stack : err);
  process.exitCode = 1;
});