#include "physics.h"


inline float gravity(const vec a, const vec b) {
    const float G = 6.67408e-11 * 1e1;
    return G / dist_sq(b, a);
}

inline float physics__max_radius(const atom a[], const int n) {
    float max_r = 0;

    for (int i = 0; i < n; ++i) {
        max_r = fmaxf(max_r, a[i].r);
    }
    
    return max_r;
}

static inline void sort_by_Y(atom a[], const int n) {
    for (int i = 1; i < n; ++i) {
        if_unlikely (a[i].p.y > a[i-1].p.y) {
            int j = i-2;
            while ((j >= 0) && (a[j].p.y <= a[i].p.y)) {
                --j;
            }

            const atom temp = a[i];
            
            memmove(&a[j+2], &a[j+1], (i - (j+1)) * sizeof(*a));
            a[j+1] = temp;
        }
    }

    TEST(
        for (int i = 1; i < n; ++i) {
            assert(a[i-1].p.y >= a[i].p.y);
            assert(memcmp(&a[i-1], &a[i], sizeof(a[i])) && "This is very likely an error");
        }
    )
}

__always_inline
static void adjust_for_collision(atom * a, atom * b) {
    const vec p = sub(b->p, a->p);

    const vec v = sub(a->v, b->v);
    const float v_len = norm(v);
    const vec v_n = divide(v, v_len);

    const vec pPv = mul(v_n, dot(p, v_n));

    const float h_sq = norm_sq(p) - norm_sq(pPv);
    const float l = sqrtf(sq(a->r + b->r) - h_sq);

    const float time_since_collision = (l - norm(pPv)) / v_len;

    a->p.x -= a->v.x * time_since_collision;
    a->p.y -= a->v.y * time_since_collision;
    b->p.x -= b->v.x * time_since_collision;
    b->p.y -= b->v.y * time_since_collision;

    // const vec q_n = divide(sub(b->p, a->p), norm(sub(b->p, a->p)));
    const vec q_n = normalize(sub(b->p, a->p));   // Faster

    const vec proj_A = mul(q_n, dot(sub(a->v, b->v), q_n));
    const vec proj_B = mul(q_n, dot(sub(b->v, a->v), q_n));
    a->v = sub(a->v, proj_A);
    b->v = sub(b->v, proj_B);

    a->p.x += a->v.x * time_since_collision;
    a->p.y += a->v.y * time_since_collision;
    b->p.x += b->v.x * time_since_collision;
    b->p.y += b->v.y * time_since_collision;
}

inline void physics__random_populate(atom a[], const int n, const float box_radius, const float avg_speed) {
    assert(n >= 0);
    assert(box_radius > 0);

    const float speed_range = 2 * avg_speed / sqrtf(2);

    for (int i = 0; i < n; ++i) {
        a[i].p.x = (2 * randf() - 1) * box_radius;
        a[i].p.y = (2 * randf() - 1) * box_radius;
        a[i].v.x = (2 * randf() - 1) * speed_range;
        a[i].v.y = (2 * randf() - 1) * speed_range;
        a[i].r = 1 * 1e3;
    }
    // TODO: Balls could be inside one another

    sort_by_Y(a, n);

    TEST(
        for (int i = 0; i < n; ++i) {
            assert(a[i].r > EPS);
        }
    )
}

inline void physics__update(atom a[], const int n, const float box_radius, const float ellapsed_time) {
    //* Update positions
    for (int i = 0; i < n; ++i) {
        a[i].p.x += a[i].v.x * ellapsed_time;
        a[i].p.y += a[i].v.y * ellapsed_time;
    }
    //*/

    //* Ball-ball collision
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

                adjust_for_collision(&A, &B);

                const float reciprocal_scale_factor = 1.0 / scale_factor;

                a[i].p = mul(A.p, reciprocal_scale_factor);
                a[i].v = mul(A.v, reciprocal_scale_factor);

                a[j].p = mul(B.p, reciprocal_scale_factor);
                a[j].v = mul(B.v, reciprocal_scale_factor);
            }
        }
    }
    //*/

    //* Ball-box collision
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

    // Keep balls sorted
    sort_by_Y(a, n);
}
