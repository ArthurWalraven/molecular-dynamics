#pragma once


#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <omp.h>

#include "global.h"
#include "vec.h"
#include "random.h"


void physics__lattice_populate(vec r[], vec v[], const int n, const float box_radius, const float energy);
void physics__update(vec r[], vec v[], vec a[], const int n, const float dt, const float box_radius);
void physics__sort_by_Y(vec r[], vec v[], const int n);
vec physics__periodic_boundary_shift(vec v, const float box_radius);
float physics__thermometer(const vec v[], const int n);
float physics__barometer(const vec v[], const int n, const float box_radius);
