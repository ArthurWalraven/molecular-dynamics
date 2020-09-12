#include "random.h"

inline vec normal_vec() {
    vec t;
    float tt;

    do {
        t.x = 2 * randf() - 1;
        t.y = 2 * randf() - 1;
        tt = norm_sq(t);
    } while (tt >= 1.f || tt == 0);

    const float f = sqrtf(-2.f * logf(tt) / tt);

    return mul(t, f);
}

inline float normal() {
    static bool has_cached_value = false;
    static float cached_value;

    if (has_cached_value) {
        has_cached_value = false;
        return cached_value;
    }

    const vec r = normal_vec();

    has_cached_value = true;
    cached_value = r.x;

    return r.y;
}
