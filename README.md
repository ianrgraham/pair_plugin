# hoomd-pair-ext

A couple of handy MD pair potentials for HOOMD-blue. 

# Installation

To use this plugin, you must install HOOMD by [compiling it from source](https://hoomd-blue.readthedocs.io/en/latest/building.html). Once HOOMD is built and installed, clone this repo and build with CMake.

```
$ cmake -B build/pair_plugin -S hoomd-pair-ext && cmake --build build/pair_plugin && cmake --install build/pair_plugin
```

# Example

Once installed, Python pair classes exposed in `pair.py` should be available under the `hoomd.pair_plugin` module.

``` python
import hoomd
import hoomd.pair_plugin.pair as p_pair

sim = hoomd.Simulation(hoomd.device.CPU())
sim.create_state_from_gsd("traj.gsd")

integrator = hoomd.md.Integrator(0.001)
nlist = hoomd.md.nlist.Tree(0.3)
hertz = p_pair.Hertzian(nlist)
hertz[('A', 'A')] = dict(epsilon=1.0, sigma=1.0)
hertz.r_cut[('A', 'A')] = 1.0

nvt = hoomd.md.methods.NVT(kT=0.1, tau=0.1, filter=hoomd.filter.All())
integrator.forces = [hertz]
integrator.methods = [nvt]

sim.operation.integrator = integrator

sim.run(100)

```
