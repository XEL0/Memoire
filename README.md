# On Terrain Visibility Graphs: Characterisation, Encoding and Algorithms

Characterisation of terrain visibility graphs (VG) remains an open question
to this date. Visibility graphs are usually dense graphs: encoding them using
biclique covers allows a compact representation.

This repository provides two programs:

- **tool**: a GUI application to generate random graphs from different families
  (simple, bipartite, comparability graphs, comparability bigraphs, or terrain VGs).
  Once generated, the graph is displayed in a window and, depending on the graph type,
  algorithms can be applied — namely a biclique decomposition for comparability
  bigraphs and terrain VGs. The execution tree can be explored in an interactive window.

- **analysis**: a lightweight version of the tool (without GUI and with lighten structure) along with an
  experiment script. Results and seeds used are available in the `analysis/results` folder.

## Requirements

- CMake ≥ 3.16
- [Qt6](https://www.qt.io/development/qt-framework/qt6) (Widgets, Core, Gui)
- A C++23-compatible compiler (GCC, Clang, MSVC)
- [nlohmann/json](https://github.com/nlohmann/json) ≥ 3.11.3 (fetched automatically if not found)

## Build Instructions

```bash
cmake -B build
cmake --build build
```

This builds both targets. The binaries are located at:
- `build/tool/Tool`
- `build/analysis/Analysis`

### Build a single target

```bash
cmake --build build --target Tool
cmake --build build --target Analysis
```

## Usage

### Tool
Launch `Tool` and select a graph family from the main window.
Once generated, use the available buttons to apply algorithms.
The execution tree of the biclique decomposition can be explored
in a separate interactive window.

### Analysis
Run `Analysis` to reproduce the experiments.
