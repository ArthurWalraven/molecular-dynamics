#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include <math.h>

#include "global.h"



typedef struct
{
    int n;                  // Number of particles
    float simulation_time;  // In seconds
    float box_radius;       // In meters
    float ups;              // Updates per second
    float fps;              // Frames per second
    float ppm;              // Pixels per meter
    
    int frame_W;    // In pixels
    int frame_H;    // In pixels
    int n_frames;
    int n_updates;
} Params;


Params process_arguments(const int argc, char * const argv[]);