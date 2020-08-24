#pragma once


#include <stdbool.h>
#include <assert.h>

#include "global.h"
#include "vec.h"


typedef struct {
    vec p;
    vec v;
    // vec a;
    float r;
} atom;


void physics__random_populate(atom a[], const int n, const float box_radius);
void physics__update(atom atoms[], const int n, const float box_radius, const float ellapsed_time);