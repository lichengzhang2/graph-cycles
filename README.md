See https://lichengzhang2.github.io/graph-cycles/my_cycles_viewer.html

# Description
This web-based tool parses undirected graphs given either as a graph6 string or as an edge list, visualizes the graph, and uses a C/WebAssembly backend to search for simple cycles of a fixed length k.
Users can:

- Check whether any simple cycle of length k exists,

- Enumerate all simple cycles of length k (up to a safe limit), or

- Enumerate the first m simple cycles of length k.

The heavy combinatorial search is implemented in C and compiled to WebAssembly for performance, while the browser UI handles input, visualization, and interaction.





# Files

Place these files in the same directory:

my_cycles_viewer.html – the main web UI (English)

cycles_lib.js – Emscripten-generated JavaScript

cycles_lib.wasm – WebAssembly module

cycles_lib.c – (optional) C source for rebuilding the core

# Requirements

To run the tool (no compilation needed):

A modern browser (Chrome / Edge / Firefox).

Python 3 for a simple local HTTP server (or any static file server).

To rebuild the C → WASM core (optional):

Emscripten SDK (emcc) properly installed and activated.

Running the Tool (no compilation)

Put my_cycles_viewer.html, cycles_lib.js, and cycles_lib.wasm in the same folder, e.g.:

```
C:\graph-cycles\
    my_cycles_viewer.html
    cycles_lib.js
    cycles_lib.wasm
```

Open a terminal / command prompt in that folder and start a local server:
```
cd C:\graph-cycles
python -m http.server 8000
```

Open your browser and go to:
```
http://localhost:8000/graph_cycles_wasm_en.html
```

Use the web UI:

Choose input type: graph6 or Edge list.

For graph6, you can try this example:
```
Q?_GWL?KG@C@_Ag?g?a?@CO?O?W
```

Click “Parse graph / show edges / draw” to:

Parse the graph,

Show its edge list,

Draw the graph.

Set cycle length k (e.g. 16 for this example graph).

Choose a mode:

Existence: “Only check existence of simple cycles of length k”.

All: “Enumerate all cycles of length k”.

First m: “Enumerate only the first m cycles of length k”, and set m.

Click “Run cycle computation” to see the result and the cycles found.

Optional: Rebuilding cycles_lib.js/.wasm from C

If you want to modify the C code (cycles_lib.c) and rebuild:

Make sure Emscripten is activated (in that terminal):

```
emcc --version
```

From the directory containing cycles_lib.c, run:
```
emcc cycles_lib.c -O3 \
  -s EXPORTED_FUNCTIONS='["_cycles_exists","_cycles_enumerate","_malloc","_free"]' \
  -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","HEAP8","HEAPU8","HEAP32","HEAPU32"]' \
  -s MODULARIZE=1 -s EXPORT_NAME="CyclesModule" \
  -o cycles_lib.js
```

This will regenerate cycles_lib.js and cycles_lib.wasm.
Then restart python -m http.server 8000 and reload the page.
