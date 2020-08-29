#pragma once


#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "global.h"
#include "vec.h"


// Marsaglia polar method
vec normal_vec();
float normal();
float mean(const float a[], const int n);
float stddev(const float a[], const int n, const float mu);

void test_random();


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
	return rand_fast() / ((float) UINT64_MAX);
}
