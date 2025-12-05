Description
This web-based tool parses undirected graphs given either as a graph6 string or as an edge list, visualizes the graph, and uses a C/WebAssembly backend to search for simple cycles of a fixed length k.
Users can:

Check whether any simple cycle of length k exists,

Enumerate all simple cycles of length k (up to a safe limit), or

Enumerate the first m simple cycles of length k.

The heavy combinatorial search is implemented in C and compiled to WebAssembly for performance, while the browser UI handles input, visualization, and interaction.
