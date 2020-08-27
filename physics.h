#pragma once


#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "global.h"
#include "vec.h"


typedef struct {
    vec p;
    vec v;
    float r;
} atom;


void physics__random_populate(atom a[], const int n, const float box_radius, const float avg_speed);
float physics__max_radius(const atom a[], const int n);
void physics__update(atom a[], const int n, const float box_radius, const float ellapsed_time);
