#ifndef IMAGE_H_
#define IMAGE_H_

#include "./anmTypes.h"

namespace anm::opener {

    u32 format_Bpp(format_t format);

    bytes format_to_rgba(robytes data, u32 pixels, format_t format);

    struct image_t {
        bytes data;
        u32 width;
        u32 height;
        format_t format;
    };

} // namespace anm::opener

#endif // IMAGE_H_
