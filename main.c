#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "global.h"
#include "read_args.h"
#include "physics.h"
#include "render.h"
#include "random.h"


// To help IDE highlighting
#ifndef NRENDER
#define DORENDER
#endif


int main(const int argc, char * const argv[]) {
#ifndef NDEBUG
    puts("Built in DEBUG mode.");
#endif

    const Params params = process_arguments(argc, argv);

    vec r[params.n] __attribute__ ((aligned(64)));
    vec v[params.n] __attribute__ ((aligned(64)));
    vec a[params.n] __attribute__ ((aligned(64)));
    physics__lattice_populate(r, v, params.n, params.box_radius, params.avg_speed);

    vec (*r_snapshots)[params.n] = memalign(64, params.n_frames * params.n * sizeof(r_snapshots[0][0]));
    if (!r_snapshots) {
        perror("Error on position snapshot buffer creation");
        exit(EXIT_FAILURE);
    }
    

    BENCH("Simulation",
        int frame_counter = -1;
        for (int t = 0; t < params.n_updates; ++t) {
            const float update_time_step = params.simulation_time / params.n_updates;
            const float frame_time_step = params.simulation_time / params.n_frames;

            physics__update(r, v, a, params.n, update_time_step, params.box_radius);

            if unlikely((t * update_time_step) - (frame_counter * frame_time_step) >= frame_time_step) {
                ++frame_counter;

                printf("\rT: %7.3f\tP: %7.3f\tupdate: %4d/%d\tsnapshot: %3d/%d", physics__thermometer(v, params.n), physics__barometer(v, params.n, params.box_radius), t+1, params.n_updates, frame_counter+1, params.n_frames);

                physics__sort_by_Y(r, v, params.n);
                for (int i = 0; i < params.n; ++i) {
                    r_snapshots[frame_counter][i] = r[i];
                }
            }
        }
        putchar('\n');
    )


#ifdef DORENDER
    BENCH("Rendering",
        printf("Allocating animation buffer: [%d] %dx%d GIF (pixel stream of %.2f MiB)\n", params.n_frames, params.frame_W, params.frame_H, (params.n_frames * params.frame_H * params.frame_W) / 0x1p20f);
        uint8_t (*frames)[params.frame_H][params.frame_W] = calloc(params.n_frames * params.frame_H * params.frame_W, sizeof(frames[0][0][0]));
        if (!frames) {
            perror("Error on animation buffer creation");
            exit(EXIT_FAILURE);
        }

        render__frames(params.n, r_snapshots, params.n_frames, params.frame_W, params.frame_H, frames, params.box_radius);

        render__animation(params.frame_W, params.frame_H, params.n_frames, frames, params.fps, params.output_filename);

        free(frames);
    )
#endif

    free(r_snapshots);

    putchar('\n');
    return EXIT_SUCCESS;
}
