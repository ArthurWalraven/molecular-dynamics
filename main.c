#include <stdio.h>
#include <assert.h>

#include "global.h"
#include "read_args.h"
#include "physics.h"
#include "render.h"
#include "random.h"

#ifndef NRENDER
#define DORENDER
#endif


int main(const int argc, char * const argv[]) {
    // Tests
    TEST(
        test_random();

        return 0;
    )


    const Params params = process_arguments(argc, argv);

#ifdef DORENDER
    printf("Allocating animation buffer: [%d] %dx%d GIF (pixel stream of %.2f MiB)\n", params.n_frames, params.frame_W, params.frame_H, (params.n_frames * params.frame_H * params.frame_W) / 0x1p20f);
    uint8_t (*frames)[params.frame_H][params.frame_W] = calloc(params.n_frames * params.frame_H * params.frame_W, sizeof(frames[0][0][0]));
    if (!frames) {
        perror("Error on buffer creation");
        exit(EXIT_FAILURE);
    }
#endif

    //* Simulation
    BENCH("Simulation",
        atom a[params.n];
        physics__lattice_populate(a, params.n, params.box_radius);
        
        int frame_counter = 0;
        float frame_time_tracker = 0;
        for (int t = 0; t < params.n_updates; ++t) {
            const float update_time_step = params.simulation_time / params.n_updates;
            const float frame_time_step = params.simulation_time / params.n_frames;

            physics__update(a, params.n, update_time_step, params.box_radius);

#ifdef DORENDER
            if (((t+1) * update_time_step) - frame_time_tracker >= frame_time_step) {
                printf("\rupdate: %4d/%d\tframe: %3d/%d", t+1, params.n_updates, frame_counter+1, params.n_frames);
                // fflush(stdin);

                render__frame(a, params.n, params.frame_W, params.frame_H, frames[frame_counter], params.box_radius);

                frame_time_tracker += frame_time_step;
                ++frame_counter;
            }
#endif
        }
        printf("\rupdate: %4d/%d\tframe: %3d/%d\n", params.n_updates, params.n_updates, frame_counter+1, params.n_frames);
    )
    //*/

#ifdef DORENDER
    render__animation(params.frame_W, params.frame_H, params.n_frames, frames, params.fps, params.output_filename);
#endif


#ifdef DORENDER
    free(frames);
#endif
    putchar('\n');
    return EXIT_SUCCESS;
}
