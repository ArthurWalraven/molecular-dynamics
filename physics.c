#include "physics.h"


inline float gravity(const vec a, const vec b) {
    const float G = 6.67408e-11 * 1e1;
    return G / dist_sq(b, a);
}

inline float physics__max_radius(const ball b[], const int n) {
    float max_r = 0;

    for (int i = 0; i < n; ++i) {
        max_r = fmaxf(max_r, b[i].r);
    }
    
    return max_r;
}

static inline void sort_by_Y(ball b[], const int n) {
    for (int i = 1; i < n; ++i) {
        if_unlikely (b[i].p.y > b[i-1].p.y) {
            int j = i-2;
            while ((j >= 0) && (b[j].p.y <= b[i].p.y)) {
                --j;
            }

            const ball temp = b[i];
            
            memmove(&b[j+2], &b[j+1], (i - (j+1)) * sizeof(*b));
            b[j+1] = temp;
        }
    }

    TEST(
        for (int i = 1; i < n; ++i) {
            assert(b[i-1].p.y >= b[i].p.y);
            assert(memcmp(&b[i-1], &b[i], sizeof(b[i])) && "This is very likely an error");
        }
    )
}

__always_inline
static void adjust_for_collision(ball * a, ball * b) {
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

inline void physics__random_populate(ball b[], const int n, const float box_radius, const float avg_speed) {
    assert(n >= 0);
    assert(box_radius > 0);

    const float speed_range = 2 * avg_speed / sqrtf(2);

    for (int i = 0; i < n; ++i) {
        b[i].p.x = (2 * randf() - 1) * box_radius;
        b[i].p.y = (2 * randf() - 1) * box_radius;
        b[i].v.x = (2 * randf() - 1) * speed_range;
        b[i].v.y = (2 * randf() - 1) * speed_range;
        b[i].r = 1 * 1e3;
    }
    // TODO: Balls could be inside one another

    sort_by_Y(b, n);

    TEST(
        for (int i = 0; i < n; ++i) {
            assert(b[i].r > EPS);
        }
    )
}

inline void physics__update(ball b[], const int n, const float box_radius, const float ellapsed_time) {
    //* Update positions
    for (int i = 0; i < n; ++i) {
        b[i].p.x += b[i].v.x * ellapsed_time;
        b[i].p.y += b[i].v.y * ellapsed_time;
    }
    //*/

    //* Ball-ball collision
    for (int i = 0; i < n-1; ++i) {
        for (int j = i+1; j < n; ++j) {
            if_unlikely(dist_sq(b[i].p, b[j].p) < sq(b[i].r + b[j].r)) {
                const float scale_factor = 1.0 / ((double) (b[i].r + b[j].r) - (double) norm(sub(b[j].p, b[i].p)));
                
                ball A = {
                    .p = mul(b[i].p, scale_factor),
                    .v = mul(b[i].v, scale_factor),
                    .r = b[i].r * scale_factor
                };
                ball B = {
                    .p = mul(b[j].p, scale_factor),
                    .v = mul(b[j].v, scale_factor),
                    .r = b[j].r * scale_factor
                };

                adjust_for_collision(&A, &B);

                const float reciprocal_scale_factor = 1.0 / scale_factor;

                b[i].p = mul(A.p, reciprocal_scale_factor);
                b[i].v = mul(A.v, reciprocal_scale_factor);

                b[j].p = mul(B.p, reciprocal_scale_factor);
                b[j].v = mul(B.v, reciprocal_scale_factor);
            }
        }
    }
    //*/

    //* Ball-box collision
    for (int i = 0; i < n; ++i) {
        if_unlikely(norm_max(b[i].p) > (box_radius - b[i].r)) {
            // Horizontal collisions
            if_unlikely(b[i].p.x > (box_radius - b[i].r)) {
                b[i].v.x *= -1;
                b[i].p.x = -b[i].p.x + 2 * (box_radius - b[i].r);
            }
            else if_unlikely(b[i].p.x < -(box_radius - b[i].r)) {
                b[i].v.x *= -1;
                b[i].p.x = -b[i].p.x + 2 * -(box_radius - b[i].r);
            }

            // Vertical collisions
            if_unlikely(b[i].p.y > (box_radius - b[i].r)) {
                b[i].v.y *= -1;
                b[i].p.y = -b[i].p.y + 2 * (box_radius - b[i].r);
            }
            else if_unlikely(b[i].p.y < -(box_radius - b[i].r)) {
                b[i].v.y *= -1;
                b[i].p.y = -b[i].p.y + 2 * -(box_radius - b[i].r);
            }
        }
    }
    //*/

    // Keep balls sorted
    sort_by_Y(b, n);
}
