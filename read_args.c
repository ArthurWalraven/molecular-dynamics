#include "read_args.h"

Params process_arguments(const int argc, char * const argv[]) {
    Params parameters = {
        .n = 10,
        .simulation_time = 5.0,
        .box_radius = 10.0,
        .ups = 1000.0,
        .fps = 50.0,
        .resolution = 100,
        .avg_speed = 20.0,
        .output_filename = "animation.gif"
    };

    struct option long_options[] = {
        // TODO: Add '--version'
        {"help",        no_argument,       NULL, 'h'},
        {"n",           required_argument, NULL, 'n'},
        {"time",        required_argument, NULL, 't'},
        {"box-radius",  required_argument, NULL, 'b'},
        {"avg-speed",   required_argument, NULL, 's'},
        {"ups",         required_argument, NULL, 'u'},
        {"fps",         required_argument, NULL, 'f'},
        {"resolution",  required_argument, NULL, 'r'},
        {"output-file", required_argument, NULL, 'o'},
        {0, 0, 0, 0}
    };

    int ch;
    int opt_index = sizeof(long_options) / sizeof(*long_options);
    while (-1 != (ch = getopt_long(argc, argv, "hn:t:o:", long_options, &opt_index))) {
        int items_read = 1;

        switch (ch) {
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
                items_read = sscanf(optarg, " %d", &parameters.resolution);
                break;
            
            case 'o':
                parameters.output_filename = optarg;
                items_read = 1;
                break;
            
            case '?':
            default:
                items_read = 0;
                // fall through

            case 'h':
                fprintf(stderr,
                    "USAGE: %s [options]\n\n"
                    "OPTIONS:\n\n"
                    "%-24s\t%s\n"
                    "%-24s\t%s\n"
                    "%-24s\t%s\n"
                    "%-24s\t%s\n"
                    "%-24s\t%s\n"
                    "%-24s\t%s\n"
                    "%-24s\t%s\n"
                    "%-24s\t%s\n"
                    "%-24s\t%s\n"
                    ,
                    argv[0]
                    ,"-h, --help", "Show this message"
                    ,"-o, --output-file=<filename>", "Output animation of the simulation to file <filename>" "(default: 'animation.gif')"
                    ,"-n, --n=<value>", "Simulate <value> balls" "(default: 10)"
                    ,"-t, --time=<value>", "Simulate <value> seconds" "(default: 5.0)"
                    ,"--box-radius=<value>", "Run the simulation inside of a square box with radius of <value> meters" "(default: 10.0)"
                    ,"--avg-speed=<value>", "Initilize balls' velocity radomly with average magnitude of <value> m/s" "(default: 20.0)"
                    ,"--resolution=<value>", "Generate frames with <value>x<value> pixels" "(default: 100)"
                    ,"--fps=<value>", "Generate animation with (approximately) <value> frames per second" "(default: 50.0)"
                    ,"--ups=<value>", "Run physics calculations <value> times per second" " (default: 1000.0)"
                );
                if (ch == 'h') {
                    exit(EXIT_SUCCESS);
                }
                break;
        }

        if (
               parameters.simulation_time <= 0
            || parameters.box_radius <= 0
            || parameters.ups <= 0
            || parameters.fps <= 0
            || parameters.resolution <= 0
        ) {
            fprintf(stderr,
                "%s: Option '%s' takes a positive value. Bad argument: '%s'.\n"
                "Try '%s --help' for more information.\n",
                argv[0], long_options[opt_index].name, optarg, argv[0]
            );
            exit(EXIT_FAILURE);
        }

        if (items_read != 1) {
            fprintf(stderr,
                "%s: Unrecognized argument '%s' for option '%s'.\n"
                "Try '%s --help' for more information.\n",
                argv[0], optarg, long_options[opt_index].name, argv[0]
            );
            exit(EXIT_FAILURE);
        }
    }

    parameters.frame_W = parameters.resolution;
    parameters.frame_H = parameters.resolution;
    parameters.n_frames = floorf(parameters.simulation_time * parameters.fps);  // TODO: Make sure this is right
    parameters.n_updates = floorf(parameters.simulation_time * parameters.ups); // TODO: Make sure this is right

    return parameters;
}
