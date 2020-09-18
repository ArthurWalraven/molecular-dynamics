# Playing with C and Molecular Dynamics

![alt-text](samples/sample.gif)

A simple 2D [Molecular Dynamics](https://en.wikipedia.org/wiki/Molecular_dynamics) simulator. This project is intended to be dependency-free (even the GIF file creation is "handmade").

Inter-atomic forces are modeled by differentiating the [Lennard-Jones potential][LJ] and
all the values are currently in [reduced units][units].
The simulation update is based on [Velocity Verlet algorithm][Verlet].


# Building

Unix only. Tested with Clang but should also work with GCC.


- Required compilation flags: `-lm -msse` (it uses `rsqrtss` SSE instruction)
- Recommended release compilation flags: `-DNDEBUG -DNTEST -fopenmp -march=native -O3 -ffast-math`


[units]: https://en.wikipedia.org/wiki/Lennard-Jones_potential#Dimensionless_(reduced)_units
[LJ]: https://en.wikipedia.org/wiki/Lennard-Jones_potential
[Verlet]: https://en.wikipedia.org/wiki/Verlet_integration#Velocity_Verlet
