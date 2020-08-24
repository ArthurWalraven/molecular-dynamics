#include <stdio.h>
#include <assert.h>

#include "global.h"
#include "read_args.h"
#include "physics.h"
#include "render.h"


int main(const int argc, char * const argv[]) {
    const Params params = process_arguments(argc, argv);

    printf("Allocating %.2f MiB...\n", (params.n_frames * params.frame_H * params.frame_W) / 0x1p20f);
    uint8_t (*frames)[params.frame_H][params.frame_W] = calloc(params.n_frames * params.frame_H * params.frame_W, sizeof(frames[0][0][0]));
    if (!frames) {
        perror("Error on buffer creation");
        exit(EXIT_FAILURE);
    }


    //* Simulation
    BENCH("Simulation",
        atom a[params.n];
        physics__random_populate(a, params.n, params.box_radius);
        
        int frame_counter = 0;
        float frame_time_tracker = 0;
        for (int t = 0; t < params.n_updates; ++t) {
            printf("\rupdate: %4d/%d\tframe: %3d/%d", t+1, params.n_updates, frame_counter+1, params.n_frames);
            fflush(stdin);

            const float update_time_step = params.simulation_time / params.n_updates;
            const float frame_time_step = params.simulation_time / params.n_frames;

            physics__update(a, params.n, params.box_radius, update_time_step);

            if (((t+1) * update_time_step) - frame_time_tracker >= frame_time_step) {

                render__frame(a, params. n, params.frame_W, params.frame_H, frames[frame_counter], params.box_radius);

                frame_time_tracker += frame_time_step;
                ++frame_counter;
            }
        }
        putchar('\n');
    )
    //*/

    BENCH("Animation",
        render__animation(params.frame_W, params.frame_H, params.n_frames, frames, params.fps);
    )


    free(frames);
    putchar('\n');
    return EXIT_SUCCESS;
}
