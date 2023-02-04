# hoomd-pair-ext

A couple of handy MD pair potentials for HOOMD-blue. 

# Installation

To use this plugin, you must install HOOMD by [compiling it from source](https://hoomd-blue.readthedocs.io/en/latest/building.html). Once HOOMD is built and installed, clone this repo and build with CMake.

```
$ cmake -B build/pair_plugin -S hoomd-pair-ext && cmake --build build/pair_plugin && cmake --install build/pair_plugin
```