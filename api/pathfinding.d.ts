// TypeScript bindings for emscripten-generated code.  Automatically generated at compile time.
declare namespace RuntimeExports {
    let HEAPF32: any;
    let HEAPF64: any;
    let HEAP_DATA_VIEW: any;
    let HEAP8: any;
    let HEAPU8: any;
    let HEAP16: any;
    let HEAPU16: any;
    let HEAP32: any;
    let HEAPU32: any;
    let HEAP64: any;
    let HEAPU64: any;
}
interface WasmModule {
  _main(_0: number, _1: number): number;
}

type EmbindString = ArrayBuffer|Uint8Array|Uint8ClampedArray|Int8Array|string;
export interface API {
  delete(): void;
}

export interface graph {
  delete(): void;
}

export interface node {
  delete(): void;
}

interface EmbindModule {
  API: {
    new(): API;
    initialize_graph(_0: EmbindString): graph;
    astar_find_path(_0: graph, _1: node, _2: node): string;
    dijkstra_find_path(_0: graph, _1: node, _2: node): string;
  };
  graph: {
    new(): graph;
  };
  node: {
    new(_0: number, _1: number): node;
  };
}

export type MainModule = WasmModule & typeof RuntimeExports & EmbindModule;
export default function MainModuleFactory (options?: unknown): Promise<MainModule>;
