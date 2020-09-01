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
        a[i].v = normal_vec();

        a[i].a.x = 0;
        a[i].a.y = 0;
    }
}

// Insertion sort
inline void physics__sort_by_Y(atom a[], const int n) {
    for (int i = 1; i < n; ++i) {
        if unlikely(a[i].r.y > a[i-1].r.y) {
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

// Periodic Boundary Condition
inline vec physics__periodic_boundary_shift(vec v, const float box_radius) {
    if unlikely(norm_max(v) > box_radius) {
        if unlikely(v.x > box_radius) {
            v.x -= 2 * box_radius;
        }
        else if (v.x < -box_radius) {
            v.x += 2 * box_radius;
        }

        if unlikely(v.y > box_radius) {
            v.y -= 2 * box_radius;
        }
        else if (v.y < -box_radius) {
            v.y += 2 * box_radius;
        }
    }

    return v;
}

// Velocity verlet
inline void physics__update(atom a[], const int n, const float dt, const float box_radius) {
    vec accs[THREAD_COUNT][n] __attribute__ ((aligned (64)));

    #pragma omp parallel
    {
        #pragma omp for schedule(static)
        for (int i = 0; i < n; ++i) {
            a[i].v.x += 0.5 * a[i].a.x * dt;
            a[i].v.y += 0.5 * a[i].a.y * dt;
            a[i].r.x += a[i].v.x * dt;
            a[i].r.y += a[i].v.y * dt;
            a[i].a.x = 0;
            a[i].a.y = 0;

            for (int t = 0; t < THREAD_COUNT; ++t) {
                accs[t][i] = to_vec(0, 0);
            }
        }
        
        #pragma omp for schedule(static)
        for (int i = 0; i < n-1; ++i) {
            for (int j = i+1; j < n; ++j) {
                vec dr = sub(a[j].r, a[i].r);
                dr = physics__periodic_boundary_shift(dr, box_radius);

                const float recip_drdr = 1/dot(dr, dr);

                // Gradient of the Lennard-Jones potential
                const vec acc = mul(dr, -24 * recip_drdr * ( 2 * powf(recip_drdr, 6) - powf(recip_drdr, 3)));

                const int t = omp_get_thread_num();
                accs[t][i] = add(accs[t][i], acc);
                accs[t][j] = sub(accs[t][j], acc);
            }
        }

        #pragma omp for schedule(static)
        for (int i = 0; i < n; ++i) {
            for (int t = 0; t < THREAD_COUNT; ++t) {
                a[i].a = add(a[i].a, accs[t][i]);
            }

            a[i].v.x += 0.5 * a[i].a.x * dt;
            a[i].v.y += 0.5 * a[i].a.y * dt;

            a[i].r = physics__periodic_boundary_shift(a[i].r, box_radius);
        }
    }
}
