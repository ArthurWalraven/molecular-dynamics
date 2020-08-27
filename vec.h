#pragma once


#include <math.h>
#include <assert.h>
#include <xmmintrin.h>

#include "global.h"


extern const float EPS;


typedef struct {
    float x;
    float y;
} vec;


__always_inline
vec add(const vec a, const vec b) {
    return (vec) {
        .x = a.x + b.x,
        .y = a.y + b.y
    };
}

__always_inline
vec sub(const vec a, const vec b) {
    return (vec) {
        .x = a.x - b.x,
        .y = a.y - b.y
    };
}

__always_inline
vec mul(const vec a, const float x) {
    return (vec) {
        .x = a.x * x,
        .y = a.y * x
    };
}

__always_inline
vec divide(const vec a, const float x) {
    return (vec) {
        .x = a.x / x,
        .y = a.y / x
    };
}

__always_inline
float dot(const vec a, const vec b) {
    return (a.x * b.x + a.y * b.y);
}

__always_inline
float norm_sq(const vec a) {
    return dot(a, a);
}

__always_inline
float norm(const vec a) {
    return sqrtf(norm_sq(a));
}

__always_inline
float norm_max(const vec a) {
    return fmaxf(fabsf(a.x), fabsf(a.y));
}

__always_inline
float dist_sq(const vec a, const vec b) {
    return norm_sq(sub(b, a));
}

__always_inline
float dist(const vec a, const vec b) {
    return norm(sub(b, a));
}

__always_inline
vec normalize(const vec a) {
    __m128 t = _mm_set_ss(norm_sq(a));
    t = _mm_rsqrt_ss(t);
    return mul(a, _mm_cvtss_f32(t));
}
