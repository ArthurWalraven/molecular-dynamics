# Playing with C and Molecular Dynamics

![alt-text][sample-animation]

A simple 2D Molecular Dynamics simulator. This project is intended to be dependency-free (even the GIF file creation is "handmade").


# Building

Unix only. Tested with Clang but should also work with GCC.


- Required compilation flags: `-lm -msse` (it uses `rsqrtss` SSE instruction)
- Recommended release compilation flags: `-DNDEBUG -DNTEST -fopenmp -march=native -O3 -ffast-math`


[sample-animation]: https://github.com/ArthurWalraven/c_sandbox/blob/master/samples/sample.gif
