#pragma once


#include <stdint.h>

#include "global.h"
#include "vec.h"


// Marsaglia polar method
vec normal_vec();
float mean(const float a[], const int n);
float stddev(const float a[], const int n, const float mu);


__always_inline
uint64_t rand_fast() {
	extern uint64_t state;
	
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;

	return state;
}

__always_inline
float randf() {
	return (float) rand_fast() / UINT64_MAX;
}
