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
} ball;


void physics__random_populate(ball b[], const int n, const float box_radius, const float avg_speed);
float physics__max_radius(const ball b[], const int n);
void physics__update(ball b[], const int n, const float box_radius, const float ellapsed_time);
