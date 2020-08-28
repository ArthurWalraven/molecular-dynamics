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
void physics__update(atom a[], const int n, const float ellapsed_time, const float box_radius);
void physics__sort_by_Y(atom a[], const int n);
