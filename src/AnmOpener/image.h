#ifndef IMAGE_H_
#define IMAGE_H_

#include "anmTypes.h"

#define FORMAT_RGBA8888 ((format_t)-1)

namespace AnmOpener {

    unsigned int format_Bpp(format_t format);

    unsigned char* format_to_rgba( const unsigned char* data, unsigned int pixels, format_t format);

    struct image_t {
        unsigned char* data;
        unsigned int width;
        unsigned int height;
        format_t format;
    };

}

#endif // IMAGE_H_
