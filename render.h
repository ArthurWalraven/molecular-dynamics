#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <error.h>
#include <endian.h>
#include <assert.h>
#include <math.h>
#include <omp.h>

#include "global.h"
#include "physics.h"


void render__frame(const atom a[], const int n, const int W, const int H, uint8_t frame[][W], const float box_radius);
void render__animation(const int W, const int H, const int T, const uint8_t frame[][H][W], const float FPS, const char * filename);

void render__test_BMP();
