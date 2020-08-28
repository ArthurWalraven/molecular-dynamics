#include "physics.h"


inline void physics__lattice_populate(atom a[], const int n, const float box_radius) {
    assert(box_radius > 0);
    assert(n >= 0);

    const int m = (int) roundf(sqrtf(n/2));
    assert(2 * sq(m) == n  && "'n / 2' needs to be a perfect square");

    const float lattice_step = 2 * box_radius / (m - 0);

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < m; ++j) {
            a[(i * m) + j].r.x = -box_radius + j * lattice_step;
            a[(i * m) + j].r.y = -box_radius + i * lattice_step;
        }
        for (int j = 0; j < m; ++j) {
            a[sq(m) + (i * m) + j].r.x = -box_radius + (j + 0.5f) * lattice_step;
            a[sq(m) + (i * m) + j].r.y = -box_radius + (i + 0.5f) * lattice_step;
        }
    }

    for (int i = 0; i < n; ++i) {
        a[i].v.x = (2 * randf() - 1) * 1;
        a[i].v.y = (2 * randf() - 1) * 1;
        // a[i].v.x = 0;
        // a[i].v.y = 0;

        a[i].a.x = 0;
        a[i].a.y = 0;
    }
}

static inline void sort_by_Y(atom a[], const int n) {
    for (int i = 1; i < n; ++i) {
        if_unlikely (a[i].r.y > a[i-1].r.y) {
            int j = i-2;
            while ((j >= 0) && (a[j].r.y <= a[i].r.y)) {
                --j;
            }

            const atom temp = a[i];
            
            memmove(&a[j+2], &a[j+1], (i - (j+1)) * sizeof(*a));
            a[j+1] = temp;
        }
    }

    TEST(
        for (int i = 1; i < n; ++i) {
            assert(a[i-1].r.y >= a[i].r.y);
            assert(memcmp(&a[i-1], &a[i], sizeof(a[i])) && "This is very likely an error");
        }
    )
}

inline void physics__update(atom a[], const int n, const float ellapsed_time, const float box_radius) {
    for (int i = 0; i < n; ++i) {
        a[i].v.x += 0.5 * a[i].a.x * ellapsed_time;
        a[i].v.y += 0.5 * a[i].a.y * ellapsed_time;
        a[i].r.x += a[i].v.x * ellapsed_time;
        a[i].r.y += a[i].v.y * ellapsed_time;
        a[i].a.x = 0;
        a[i].a.y = 0;
    }

    #pragma omp parallel for
    for (int i = 0; i < n-1; ++i) {
        for (int j = i+1; j < n; ++j) {
            vec dr = sub(a[j].r, a[i].r);

            if_unlikely(norm_max(dr) > box_radius) {
                if(dr.x > box_radius) {
                    dr.x -= 2 * box_radius;
                }
                else if(dr.x < -box_radius) {
                    dr.x += 2 * box_radius;
                }

                if(dr.y > box_radius) {
                    dr.y -= 2 * box_radius;
                }
                else if(dr.y < -box_radius) {
                    dr.y += 2 * box_radius;
                }
            }


            const float recip_rr = 1/dot(dr, dr);
            const vec acc = mul(dr, -24 * recip_rr * ( 2 * powf(recip_rr, 6) - powf(recip_rr, 3)));

            a[i].a = add(a[i].a, acc);
            a[j].a = sub(a[j].a, acc);
        }
    }

    for (int i = 0; i < n; ++i) {
        a[i].v.x += 0.5 * a[i].a.x * ellapsed_time;
        a[i].v.y += 0.5 * a[i].a.y * ellapsed_time;
    }

    for (int i = 0; i < n; ++i) {
        if_unlikely(norm_max(a[i].r) > box_radius) {
            if_unlikely(a[i].r.x > box_radius) {
                a[i].r.x -= 2 * box_radius;
            }
            else if_unlikely(a[i].r.x < -box_radius) {
                a[i].r.x += 2 * box_radius;
            }

            if_unlikely(a[i].r.y > box_radius) {
                a[i].r.y -= 2 * box_radius;
            }
            else if_unlikely(a[i].r.y < -box_radius) {
                a[i].r.y += 2 * box_radius;
            }
        }
    }

    // Keep atoms sorted
    sort_by_Y(a, n);
}
