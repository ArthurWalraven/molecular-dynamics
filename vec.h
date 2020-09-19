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


#define assert_type(type, exp) assert(__builtin_types_compatible_p(type, typeof(exp)))


#define to_vec(x_, y_) ({\
    (vec) {.x = x_, .y = y_};\
})

#define add(a, b) ({\
    assert_type(vec, a);\
    assert_type(vec, b);\
\
    (vec) {\
        .x = (a).x + (b).x,\
        .y = (a).y + (b).y\
    };\
})

#define sub(a, b) ({\
    assert_type(vec, a);\
    assert_type(vec, b);\
\
    (vec) {\
        .x = (a).x - (b).x,\
        .y = (a).y - (b).y\
    };\
})

#define mul(a, x_) ({\
    assert_type(vec, a);\
\
    (vec) {\
        .x = (a).x * x_,\
        .y = (a).y * x_\
    };\
})

#define divide(a, x_) ({\
    assert_type(vec, a);\
\
    (vec) {\
        .x = (a).x / x_,\
        .y = (a).y / x_\
    };\
})

#define dot(a, b) ({\
    assert_type(vec, a);\
    assert_type(vec, b);\
\
    ((a).x * (b).x + (a).y * (b).y);\
})

#define norm_sq(a) ({\
    assert_type(vec, a);\
\
    dot(a, a);\
})

#define norm(a) ({\
    assert_type(vec, a);\
\
    sqrtf(norm_sq(a));\
})

#define norm_max(a) ({\
    assert_type(vec, a);\
\
    fmaxf(fabsf((a).x), fabsf((a).y));\
})

#define dist_sq(a, b) ({\
    assert_type(vec, a);\
    assert_type(vec, b);\
\
    norm_sq(sub(b, a));\
})

#define dist(a, b) ({\
    assert_type(vec, a);\
    assert_type(vec, b);\
\
    norm(sub(b, a));\
})

#define normalize(a) ({\
    assert_type(vec, a);\
\
    __m128 t_ = _mm_set_ss(norm_sq(a));\
    t_ = _mm_rsqrt_ss(t_);\
    mul(a, _mm_cvtss_f32(t_));\
})
