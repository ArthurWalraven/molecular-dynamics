#include "render.h"


#define WITCH_CONSTANT 6.f

#define FRAMES_DIRECTORY_PATH "frames/"
#define FRAME_FILE_NAME_PREFIX  FRAMES_DIRECTORY_PATH "frame_"


#if BYTE_ORDER == BIG_ENDIAN
#define to_little_endian16(x)   __builtin_bswap16((uint16_t) (x))
#else
#define to_little_endian16(x)   (x)
#endif


static inline float witch_of_Agnesi(const vec v, const vec c) {
    return 1/(1 + sq(WITCH_CONSTANT) * dist_sq(v, c));
}

static void to_BMP(const int W, const int H, const uint8_t canvas[][W]) {
    assert(!(W & 0b11) && "Not implemented: canvas width is not a multiple of 4");

    static int frame_counter = -1;
    ++frame_counter;

    // TODO: Make endian independent
    struct BMP_Header {
        // From https://en.wikipedia.org/wiki/BMP_file_format
        char BM[2];                 // Used to identify the BMP file
        uint32_t file_size;         // In bytes
        unsigned : 16;
        unsigned : 16;
        uint32_t  offset;           // Offset to image data in bytes from beginning of file (54 bytes)
        uint32_t  dib_header_size;  // DIB Header size in bytes (40 bytes)
        int32_t   width_px;
        int32_t   height_px;
        uint16_t  num_planes;       // Must be 1
        uint16_t  bits_per_pixel;
        uint32_t  compression_type;
        uint32_t  image_size_bytes; // Image size in bytes
        signed : 32;
        signed : 32;
        uint32_t num_colours;
        unsigned : 32;
    } __attribute__((packed))
    header = {
        .BM = {'B', 'M'},
        .file_size = sizeof(struct BMP_Header) + (256 * 4 * 1) + (W * H * 1),
        .offset = sizeof(struct BMP_Header) + (256 * 4 * 1),
        .dib_header_size = 40,
        .width_px = W,
        .height_px = H,
        .num_planes = 1,
        .bits_per_pixel = 8, // Grayscale
        .compression_type = 0,
        .image_size_bytes = W * H * 1,
        .num_colours = 1 << 8
    };

    assert(sizeof(header) == 54 && "BMP header has proper size");


    static char file_name[] = FRAME_FILE_NAME_PREFIX "000000.bmp";
    assert(frame_counter <= 999999 && "Frame number can be represented within 6 decimal digits");
    sprintf(file_name + sizeof(FRAME_FILE_NAME_PREFIX) - 1, "%06d.bmp", frame_counter);

    FILE *fp = fopen(file_name, "wb");
    if (!fp) {
        perror("Error on file creation");
        exit(EXIT_FAILURE);
    }

    fwrite(&header, 1, sizeof(header), fp);

    uint8_t shades_of_gray[256][4];
    for (int i = 0; i < 256; ++i) {
        shades_of_gray[i][0] = i;
        shades_of_gray[i][1] = i;
        shades_of_gray[i][2] = i;
        shades_of_gray[i][3] = 0;
    }
    fwrite(shades_of_gray, 4, 256, fp);

    fwrite(canvas, 1, W * H, fp);


    fclose(fp);
}

static void to_GIF(const int W, const int H, const int T, const uint8_t frame[][H][W], const float FPS, const char * filename) {
    assert(W > 0);
    assert(H > 0);
    assert(T > 0);
    assert(FPS > 0);
    if (roundf(100.f / FPS) < 2) {
        puts("Warning: high fps GIFs are not widely supported.");
    }

    // Uncompressed GIF file
    // See https://en.wikipedia.org/wiki/GIF#Uncompressed_GIF for reference and
    // https://en.wikipedia.org/wiki/File:Quilt_design_as_46x46_uncompressed_GIF.gif for an example

    // In order to keep one byte per pixel
    // NOTE: Used macros to keep GCC happy
#define BIT_DEPTH   7
#define NUMBER_OF_COLOURS   (1 << BIT_DEPTH)
    
    struct __attribute__((packed)) GIF_Header {
        // From https://www.w3.org/Graphics/GIF/spec-gif89a.txt
        char     signature[3];
        char     version[3];
        uint16_t width_px;
        uint16_t height_px;
        unsigned global_colour_table_size : 3;
        unsigned _sort : 1;
        unsigned colour_resolution : 3;
        unsigned global_colour_table : 1;
        uint8_t  background_colour_index;
        uint8_t  _aspect_ratio;
        uint8_t  colour_table[NUMBER_OF_COLOURS][3];
    } header = {
        .signature = {'G', 'I', 'F'},
        .version = {'8', '9', 'a'},
        .width_px = to_little_endian16(W),
        .height_px = to_little_endian16(H),
        .global_colour_table_size = BIT_DEPTH - 1,
        .colour_resolution = 8 - 1,
        .global_colour_table = true,
        .background_colour_index = 0,
        // Unused fields
        ._sort = 0,
        ._aspect_ratio = 0
    };
    // Grayscale (128 colours)
    for (int i = 0; i < NUMBER_OF_COLOURS; ++i) {
        header.colour_table[i][0] = i * 2;
        header.colour_table[i][1] = i * 2;
        header.colour_table[i][2] = i * 2;
    }

    struct __attribute__((packed)) Application_Extension {
        // From https://en.wikipedia.org/wiki/GIF#Animated_GIF
        uint8_t extension_introducer;
        uint8_t application_label;
        uint8_t block_size;
        char    application_identifier[8];
        char    application_authentication_code[3];
    } const netscape_application_block = {
        .extension_introducer = '!',
        .application_label = 0xFF,
        .block_size = 11,
        .application_identifier = {'N', 'E', 'T', 'S', 'C', 'A', 'P', 'E'},
        .application_authentication_code = {'2', '.', '0'}
    };

    struct __attribute__((packed)) Netscape_Data_Subblock {
        // From https://en.wikipedia.org/wiki/GIF#Animated_GIF
        uint8_t  block_size;
        uint8_t  subblock_index;
        uint16_t number_of_repetitions;
        uint8_t  block_terminator;
    } const netscape_subblock = {
        .block_size = 3,
        .subblock_index = 1,
        .number_of_repetitions = to_little_endian16(0),
        .block_terminator = 0
    };

    struct __attribute__((packed)) Graphics_Control_Extension {
        // From https://www.w3.org/Graphics/GIF/spec-gif89a.txt
        char     extension_introducer;
        char     graphic_control_label;
        uint8_t  block_size;
        unsigned transparency : 1;
        unsigned wait_for_user_input : 1;
        unsigned diposal_method : 3;
        unsigned _reserved: 3;
        uint16_t delay;
        uint8_t  transparent_colour_index;
        uint8_t block_terminator;
    } GCE = {
        .extension_introducer = '!',
        .graphic_control_label = 0xF9,
        .block_size = 4,
        .transparency = false,
        .wait_for_user_input = false,
        .diposal_method = 0,
        .delay = to_little_endian16((uint16_t) roundf(100.f / FPS)),
        .transparent_colour_index = 0,
        .block_terminator = 0,
        // Unused fields
        ._reserved = 0
    };

    struct __attribute__((packed)) Image_Descriptor {
        // From https://www.w3.org/Graphics/GIF/spec-gif89a.txt
        char     image_separator;
        uint16_t left;
        uint16_t top;
        uint16_t width;
        uint16_t height;
        unsigned _local_colour_table_size : 3;
        unsigned _reserved : 2;
        unsigned _sort : 1;
        unsigned _interlace : 1;
        unsigned local_colour_table : 1;
    } const image_descriptor = {
        .image_separator = ',',
        .left = to_little_endian16(0),
        .top = to_little_endian16(0),
        .width = to_little_endian16(W),
        .height = to_little_endian16(H),
        .local_colour_table = false,
        // Unused fields
        ._local_colour_table_size = 0,
        ._reserved = 0,
        ._sort = 0,
        ._interlace = 0
    };
    const uint8_t LZW_minimum_code_size = BIT_DEPTH;
    
    const char GIF_file_terminator = ';';


    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("Error on file creation");
        exit(EXIT_FAILURE);
    }

    fwrite(&header, 1, sizeof(header), fp);
    fwrite(&netscape_application_block, 1, sizeof(netscape_application_block), fp);
    fwrite(&netscape_subblock, 1, sizeof(netscape_subblock), fp);

    for (int t = 0; t < T; ++t) {
        // GCE.delay = (t+1 < T) ? GCE.delay : 1.0f * 100;
        fwrite(&GCE, 1, sizeof(GCE), fp);
        fwrite(&image_descriptor, 1, sizeof(image_descriptor), fp);
        fwrite(&LZW_minimum_code_size, 1, sizeof(LZW_minimum_code_size), fp);
        
        // One byte will be used to store the length and another for the 'CLEAR' code
        const uint8_t data_block_length = (0b1 << BIT_DEPTH) - 2;
        const uint8_t CLEAR = 0b1 << BIT_DEPTH;

        int i;
        for (i = 0; i < W * H - data_block_length; i += data_block_length) {
            fwrite((uint8_t []) {data_block_length+1, CLEAR}, 2, sizeof(uint8_t), fp);
            fwrite(&frame[t][0][0] + i, data_block_length, sizeof(uint8_t), fp);
        }
        fwrite((uint8_t []) {W * H - i + 1, CLEAR}, 2, sizeof(uint8_t), fp);
        fwrite(&frame[t][0][0] + i, W * H - i, sizeof(uint8_t), fp);
        
        const uint8_t STOP = CLEAR + 1;
        fwrite((char []) {1, STOP, 0}, 3, sizeof(char), fp);
    }
    fwrite((char *) &GIF_file_terminator, 1, sizeof(char), fp);


    if (fclose(fp)) {
        perror("Error while closing file");
        exit(EXIT_FAILURE);
    }

#undef NUMBER_OF_COLOURS
#undef BIT_DEPTH
}


void render__frame(atom a[], const int n, const int W, const int H, uint8_t frame[][W], const float box_radius) {
    physics__sort_by_Y(a, n);

    const vec canvas_origin = {
        .x = W / 2.f,
        .y = H / 2.f
    };

    int s = 0;
    int t = 0;
    #pragma omp parallel for firstprivate(s, t) schedule(static)
    for (int i = 0; i < H; ++i) {
        vec v = {.y = -(i - canvas_origin.y) * box_radius / (W / 2.f)};

        const float rendering_radius = 15.93738f / WITCH_CONSTANT;
        while ((s < n) && (a[s].r.y - rendering_radius > v.y)) {
            ++s;
        }
        while ((t < n) && (a[t].r.y + rendering_radius > v.y)) {
            ++t;
        }

        for (int j = 0; j < W; ++j) {
            v.x = (j - canvas_origin.x) * box_radius / (W / 2.f);

            for (int k = s; k < t; ++k) {
#ifdef NANTIALIAS
                frame[i][j] = (uint8_t) fminf(127, frame[i][j] + 127 * (dist_sq(v, a[k].r) <= 1));
#else
                frame[i][j] = (uint8_t) roundf(fminf(127.f, frame[i][j] + 127.f * witch_of_Agnesi(v, a[k].r)));
#endif
            }
        }
    }
}

void render__animation(const int W, const int H, const int T, const uint8_t frame[][H][W], const float FPS, const char * filename) {
    // system("convert -delay 2 " FRAME_FILE_NAME_PREFIX "*.bmp animation.gif");

    to_GIF(W, H, T, frame, FPS, filename);
}

inline void render__test_BMP() {
    const int N = 1 << 11;
    uint8_t (*canvas)[N] = malloc(N*N);
    if (!canvas) {
        perror("Error on frame creation");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            canvas[i][j] = roundf(((i ^ j) & (N-1)) * 256.f / N);
        }
    }
    to_BMP(N, N, canvas);

    free(canvas);
}
