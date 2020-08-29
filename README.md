# particle_sandbox

Unix only. Tested with Clang but should also work with GCC.


- Required compilation flags: `-lm -msse` (it uses `rsqrtss` SSE instruction)
- Recommended release compilation flags: `-DNDEBUG -DNTEST -fopenmp -march=native -O3 -ffast-math`

Sample output:

![alt-text][sample-animation]


[sample-animation]: https://github.com/ArthurWalraven/c_sandbox/blob/master/samples/sample.gif
