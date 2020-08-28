#pragma once


#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "global.h"
#include "vec.h"


typedef struct {
    vec r;
    vec v;
    vec a;
} atom;


void physics__lattice_populate(atom a[], const int n, const float box_radius);
void physics__update(atom a[], const int n, const float dt, const float box_radius);
void physics__sort_by_Y(atom a[], const int n);
vec physics__periodic_boundary_shift(vec v, const float box_radius);
