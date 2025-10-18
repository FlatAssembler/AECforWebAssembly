// AVLInAEC/avl_api.js
// Small JS API to push keys into the wasm module built from AVLInAEC.aec
// Usage:
//   const api = await AVLAPI.init('/path/to/AVLInAEC.wasm', importObject);
//   api.setKeys([30,20,40]);
//   api.render();
//   api.pushKey(15); api.render();
//   api.deleteKey(20); api.render();

class AVLAPI {
  static async init(wasmSource, importObject = {}) {
    let instantiateResult;
    if (typeof wasmSource === 'string') {
      const resp = await fetch(wasmSource);
      const bytes = await resp.arrayBuffer();
      instantiateResult = await WebAssembly.instantiate(bytes, importObject);
    } else if (wasmSource instanceof WebAssembly.Module) {
      instantiateResult = await WebAssembly.instantiate(wasmSource, importObject);
    } else if (wasmSource && wasmSource.instance) {
      instantiateResult = wasmSource;
    } else {
      throw new Error('Unsupported wasmSource for AVLAPI.init');
    }

    const exports = instantiateResult.instance.exports;
    if (!exports.memory) {
      throw new Error('WASM instance has no exported memory');
    }

    const api = new AVLAPI(exports, instantiateResult.instance);
    return api;
  }

  constructor(wasmExports, instance) {
    this.exports = wasmExports;
    this.instance = instance;
    this.mem = wasmExports.memory;
    this.keysPtr = Number(this.exports.getAddressOfKeys()); // pointer in bytes
    this.capacity = 128; // matches AEC buffer
  }

  setKeys(keysArray) {
    const n = Math.min(keysArray.length, this.capacity);
    const int32View = new Int32Array(this.mem.buffer, this.keysPtr, this.capacity);
    for (let i = 0; i < n; i++) int32View[i] = keysArray[i] | 0;
    for (let i = n; i < this.capacity; i++) int32View[i] = 0;
    this.exports.setNumberOfKeys(n);
  }

  pushKey(key) {
    const n = this.exports.getNumberOfKeys();
    if (n >= this.capacity) throw new Error('keys buffer full');
    const int32View = new Int32Array(this.mem.buffer, this.keysPtr, this.capacity);
    int32View[n] = key | 0;
    this.exports.setNumberOfKeys(n + 1);
  }

  // Remove the first occurrence of key from the keys[] buffer (shift remaining)
  // and update the wasm numberOfKeys. Returns true if a key was removed.
  deleteKey(key) {
    const n = this.exports.getNumberOfKeys();
    if (n === 0) return false;
    const int32View = new Int32Array(this.mem.buffer, this.keysPtr, this.capacity);
    let foundIndex = -1;
    for (let i = 0; i < n; i++) {
      if (int32View[i] === (key | 0)) { foundIndex = i; break; }
    }
    if (foundIndex === -1) return false;
    // shift left
    for (let i = foundIndex; i < n - 1; i++) {
      int32View[i] = int32View[i + 1];
    }
    int32View[n - 1] = 0;
    this.exports.setNumberOfKeys(n - 1);
    return true;
  }

  clearKeys() {
    this.exports.clearKeys();
  }

  getKeysBuffer() {
    return new Int32Array(this.mem.buffer, this.keysPtr, this.capacity);
  }

  render() {
    if (!this.exports.render) throw new Error('WASM module does not export render()');
    this.exports.render();
  }

  setKeysAndRender(keysArray) {
    this.setKeys(keysArray);
    this.render();
  }
  
  getMemory() {
    return this.mem;
  }
}
