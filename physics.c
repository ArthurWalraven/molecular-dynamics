#include "physics.h"


inline float gravity(const vec a, const vec b) {
    const float G = 6.67408e-11 * 1e1;
    return G / dist_sq(b, a);
}

static inline void sort_by_Y(atom a[], const int n) {
    for (int i = 1; i < n; ++i) {
        if_unlikely (a[i].p.y < a[i-1].p.y) {
            int j = i-2;
            while ((j >= 0) && (a[j].p.y > a[i].p.y)) {
                --j;
            }

            const atom temp = a[i];
            
            memmove(&a[j+2], &a[j+1], (i - (j+1)) * sizeof(*a));
            a[j+1] = temp;
        }
    }

    TEST(
        for (int i = 1; i < n; ++i) {
            assert(a[i-1].p.y <= a[i].p.y);
            assert(memcmp(&a[i-1], &a[i], sizeof(a[i])) && "This is very likely an error");
        }
    )
}

inline void physics__random_populate(atom a[], const int n, const float box_radius, const float avg_speed) {
    assert(n >= 0);
    assert(box_radius > 0);

    const float speed_range = avg_speed * sqrtf(2);

    for (int i = 0; i < n; ++i) {
        a[i].p.x = (2 * randf() - 1) * box_radius;
        a[i].p.y = (2 * randf() - 1) * box_radius;
        a[i].v.x = (2 * randf() - 1) * speed_range;
        a[i].v.y = (2 * randf() - 1) * speed_range;
        a[i].r = 1;
    }

    sort_by_Y(a, n);
}

inline void physics__update(atom a[], const int n, const float box_radius, const float ellapsed_time) {
    //* Update positions
    for (int i = 0; i < n; ++i) {
        a[i].p.x += a[i].v.x * ellapsed_time;
        a[i].p.y += a[i].v.y * ellapsed_time;
    }
    //*/

    /* Gravity
    for (int i = 0; i < n-1; ++i) {
        for (int j = i+1; j < n; ++j) {
            const float g = gravity(a[i].p, a[j].p);
            const vec pn = normalize(sub(a[j].p, a[i].p));

            a[i].a = add(a[i].a, mul(pn, -g));
            a[j].a = add(a[j].a, mul(pn, g));
        }
    }
    //*/

    //* Atom-atom collision
    for (int i = 0; i < n-1; ++i) {
        for (int j = i+1; j < n; ++j) {
            if_unlikely(dist_sq(a[i].p, a[j].p) < sq(a[i].r + a[j].r)) {
                const float scale_factor = 1.0 / ((double) (a[i].r + a[j].r) - (double) norm(sub(a[j].p, a[i].p)));
                
                atom A = {
                    .p = mul(a[i].p, scale_factor),
                    .v = mul(a[i].v, scale_factor),
                    .r = a[i].r * scale_factor
                };
                atom B = {
                    .p = mul(a[j].p, scale_factor),
                    .v = mul(a[j].v, scale_factor),
                    .r = a[j].r * scale_factor
                };

                const vec p = sub(B.p, A.p);

                const vec v = sub(A.v, B.v);
                const float v_len = norm(v);
                const vec v_n = divide(v, v_len);

                const vec pPv = mul(v_n, dot(p, v_n));

                const float h_sq = norm_sq(p) - norm_sq(pPv);
                const float l = sqrtf(sq(A.r + B.r) - h_sq);

                const float time_since_collision = (l - norm(pPv)) / v_len;

                A.p.x -= A.v.x * time_since_collision;
                A.p.y -= A.v.y * time_since_collision;
                B.p.x -= B.v.x * time_since_collision;
                B.p.y -= B.v.y * time_since_collision;

                // const vec q_n = divide(sub(B.p, A.p), norm(sub(B.p, A.p)));
                const vec q_n = normalize(sub(B.p, A.p));   // Faster

                const vec proj_A = mul(q_n, dot(sub(A.v, B.v), q_n));
                const vec proj_B = mul(q_n, dot(sub(B.v, A.v), q_n));
                A.v = sub(A.v, proj_A);
                B.v = sub(B.v, proj_B);

                A.p.x += A.v.x * time_since_collision;
                A.p.y += A.v.y * time_since_collision;
                B.p.x += B.v.x * time_since_collision;
                B.p.y += B.v.y * time_since_collision;


                const float reciprocal_scale_factor = 1.0 / scale_factor;

                a[i].p = mul(A.p, reciprocal_scale_factor);
                a[i].v = mul(A.v, reciprocal_scale_factor);

                a[j].p = mul(B.p, reciprocal_scale_factor);
                a[j].v = mul(B.v, reciprocal_scale_factor);
            }
        }
    }
    //*/

    //* Atom-box collision
    for (int i = 0; i < n; ++i) {
        if_unlikely(norm_max(a[i].p) > (box_radius - a[i].r)) {
            // Horizontal collisions
            if_unlikely(a[i].p.x > (box_radius - a[i].r)) {
                a[i].v.x *= -1;
                a[i].p.x = -a[i].p.x + 2 * (box_radius - a[i].r);
            }
            else if_unlikely(a[i].p.x < -(box_radius - a[i].r)) {
                a[i].v.x *= -1;
                a[i].p.x = -a[i].p.x + 2 * -(box_radius - a[i].r);
            }

            // Vertical collisions
            if_unlikely(a[i].p.y > (box_radius - a[i].r)) {
                a[i].v.y *= -1;
                a[i].p.y = -a[i].p.y + 2 * (box_radius - a[i].r);
            }
            else if_unlikely(a[i].p.y < -(box_radius - a[i].r)) {
                a[i].v.y *= -1;
                a[i].p.y = -a[i].p.y + 2 * -(box_radius - a[i].r);
            }
        }
    }
    //*/

    // Keep atoms sorted
    sort_by_Y(a, n);
}
