class SplayAPI {
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
      throw new Error('Unsupported wasmSource for SplayAPI.init');
    }

    const exports = instantiateResult.instance.exports;
    if (!exports.memory) {
      throw new Error('WASM instance has no exported memory');
    }

    const api = new SplayAPI(exports, instantiateResult.instance);
    return api;
  }

  constructor(wasmExports, instance) {
    this.exports = wasmExports;
    this.instance = instance;
    this.mem = wasmExports.memory;
    this.keysPtr = Number(this.exports.getAddressOfKeys()); // pointer in bytes
    this.insertOrDeletePtr = Number(this.exports.getAddressOfInsertOrDelete());
    this.capacity = 128; // matches AEC buffer
  }

  setKeys(keysArray) {
    const n = Math.min(keysArray.length, this.capacity);
    const int32View = new Int32Array(this.mem.buffer, this.keysPtr, this.capacity);
    for (let i = 0; i < n; i++) int32View[i] = keysArray[i] | 0;
    for (let i = n; i < this.capacity; i++) int32View[i] = 0;
    const int8View = new Int8Array(this.mem.buffer, this.insertOrDeletePtr, this.capacity);
    for (let i= 0; i < n; i++) int8View[i] = 'I'.charCodeAt(0);
    this.exports.setNumberOfKeys(n);
  }

  pushKey(key) {
    const n = this.exports.getNumberOfKeys();
    if (n >= this.capacity) throw new Error('keys buffer full');
    const int32View = new Int32Array(this.mem.buffer, this.keysPtr, this.capacity);
    int32View[n] = key | 0;
    const int8View = new Int8Array(this.mem.buffer, this.insertOrDeletePtr, this.capacity);
    int8View[n] = 'I'.charCodeAt(0);
    this.exports.setNumberOfKeys(n + 1);
  }

  deleteKey(key) {
    const n = this.exports.getNumberOfKeys();
    if (n >= this.capacity) throw new Error('keys buffer full');
    const int32View = new Int32Array(this.mem.buffer, this.keysPtr, this.capacity);
    int32View[n] = key | 0;
    const int8View = new Int8Array(this.mem.buffer, this.insertOrDeletePtr, this.capacity);
    int8View[n] = 'D'.charCodeAt(0);
    this.exports.setNumberOfKeys(n + 1);
  }

  pushLikeInBST(key) {
    const n = this.exports.getNumberOfKeys();
    if (n >= this.capacity) throw new Error('keys buffer full');
    const int32View = new Int32Array(this.mem.buffer, this.keysPtr, this.capacity);
    int32View[n] = key | 0;
    const int8View = new Int8Array(this.mem.buffer, this.insertOrDeletePtr, this.capacity);
    int8View[n] = 'B'.charCodeAt(0);
    this.exports.setNumberOfKeys(n + 1);
  }
  
  searchKey(key) {
    const n = this.exports.getNumberOfKeys();
    if (n >= this.capacity) throw new Error('keys buffer full');
    const int32View = new Int32Array(this.mem.buffer, this.keysPtr, this.capacity);
    int32View[n] = key | 0;
    const int8View = new Int8Array(this.mem.buffer, this.insertOrDeletePtr, this.capacity);
    int8View[n] = 'S'.charCodeAt(0);
    this.exports.setNumberOfKeys(n + 1);
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
