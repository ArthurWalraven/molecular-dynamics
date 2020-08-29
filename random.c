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

float mean(const float a[], const int n) {
    assert(n > 0);

    float sum = 0;

    for (int i = 0; i < n; ++i) {
        sum += a[i];
    }
    
    return sum/n;
}

float stddev(const float a[], const int n, const float mu) {
    assert(n > 0);
    
    float variance = 0;

    for (int i = 0; i < n; ++i) {
        variance += sq(mu - a[i]);
    }

    return sqrtf(variance);
}

