#include "read_args.h"

Params process_arguments(const int argc, char * const argv[]) {
    Params parameters = {
        .n = 5,
        .simulation_time = 5.0,
        .box_radius = 10.0,
        .ups = 100.0,
        .fps = 50.0,
        .ppm = 8.0,
        .avg_speed = 20.0
    };

    struct option long_options[] = {
        {"n",           required_argument, NULL, 'n'},
        {"t",           required_argument, NULL, 't'},
        {"box-radius",  required_argument, NULL, 'b'},
        {"avg-speed",   required_argument, NULL, 's'},
        {"ups",         required_argument, NULL, 'u'},
        {"fps",         required_argument, NULL, 'f'},
        {"resolution",  required_argument, NULL, 'r'},
        {0, 0, 0, 0}
    };

    int ch;
    int opt_index;
    while (-1 != (ch = getopt_long(argc, argv, "t:n:", long_options, &opt_index))) {
        int items_read;

        switch (ch)
        {
            case 'n':
                items_read = sscanf(optarg, " %d", &parameters.n);
                break;
            
            case 't':
                items_read = sscanf(optarg, " %f", &parameters.simulation_time);
                break;
            
            case 'b':
                items_read = sscanf(optarg, " %f", &parameters.box_radius);
                break;
            
            case 's':
                items_read = sscanf(optarg, " %f", &parameters.avg_speed);
                break;
            
            case 'u':
                items_read = sscanf(optarg, " %f", &parameters.ups);
                break;
            
            case 'f':
                items_read = sscanf(optarg, " %f", &parameters.fps);
                break;
            
            case 'r':
                items_read = sscanf(optarg, " %f", &parameters.ppm);
                break;
            
            case '?':
            default:
                items_read = 0;
                break;
        }

        if (items_read != 1) {
            fprintf(stderr, "Unknown argument '%s' for option '%s'.\n", optarg, long_options[opt_index].name);
            exit(EXIT_FAILURE);
        }
    }

    assert(parameters.simulation_time > 0);
    assert(parameters.box_radius > 0);
    assert(parameters.ups > 0);
    assert(parameters.fps > 0);
    assert(parameters.ppm > 0);

    parameters.frame_W = ceilf(parameters.box_radius * parameters.ppm);
    parameters.frame_H = ceilf(parameters.box_radius * parameters.ppm);
    parameters.n_frames = ceilf(parameters.simulation_time * parameters.fps);   // TODO: Make sure this is right
    parameters.n_updates = floorf(parameters.simulation_time * parameters.ups); // TODO: Make sure this is right

    return parameters;
}
