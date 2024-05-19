#include "./image.h"

namespace anm::opener {

u32 format_Bpp(format_t format) {
    switch (format) {
    case format_t::RGBA8888:
    case format_t::BGRA8888:
        return 4;
    case format_t::ARGB4444:
    case format_t::RGB565:
        return 2;
    case format_t::GRAY8:
        return 1;
    }
    return 1;
}

bytes format_to_rgba(robytes data, u32 pixels, format_t format) {
    u32 i;
    u32* out = new u32[pixels];
    bytes outbytes = reinterpret_cast<bytes>(out);
    const u16* asu16 = reinterpret_cast<const u16*>(data);
    const u32* asu32 = reinterpret_cast<const u32*>(data);

    switch (format) {
    case format_t::GRAY8:
        for (i = 0; i < pixels; ++i) {
            out[i] = 0xff000000
                | (data[i] << 16 & 0xff0000)
                | (data[i] << 8 & 0xff00)
                | (data[i] << 0 & 0xff);
        }
        break;
    case format_t::BGRA8888:
        for (i = 0; i < pixels; ++i) {
            outbytes[i * sizeof(u32) + 0] = data[i * sizeof(u32) + 2];
            outbytes[i * sizeof(u32) + 1] = data[i * sizeof(u32) + 1];
            outbytes[i * sizeof(u32) + 2] = data[i * sizeof(u32) + 0];
            outbytes[i * sizeof(u32) + 3] = data[i * sizeof(u32) + 3];
        }
        break;
    case format_t::ARGB4444:
        for (i = 0; i < pixels; ++i) {
            /* Extends like this: 0x0 -> 0x00, 0x3 -> 0x33, 0xf -> 0xff.
             * It's required for proper alpha. */
            out[i] = ((data[i * sizeof(u16) + 1] & 0xf0) << 24 & 0xf0000000)
                | ((data[i * sizeof(u16) + 1] & 0xf0) << 20 & 0x0f000000)
                | ((data[i * sizeof(u16) + 0] & 0x0f) << 20 & 0xf00000)
                | ((data[i * sizeof(u16) + 0] & 0x0f) << 16 & 0x0f0000)
                | ((data[i * sizeof(u16) + 0] & 0xf0) << 8 & 0xf000)
                | ((data[i * sizeof(u16) + 0] & 0xf0) << 4 & 0x0f00)
                | ((data[i * sizeof(u16) + 1] & 0x0f) << 4 & 0xf0)
                | ((data[i * sizeof(u16) + 1] & 0x0f) << 0 & 0x0f);
        }
        break;
    case format_t::RGB565:
        for (i = 0; i < pixels; ++i) {
            /* Bit-extends channels: 00001b -> 00001111b. */
            out[i] = 0xff000000
                | ((asu16[i] & 0x001f) << 19 & 0xf80000)
                | ((asu16[i] & 0x0001) << 16 & 0x040000)
                | ((asu16[i] & 0x0001) << 16 & 0x020000)
                | ((asu16[i] & 0x0001) << 16 & 0x010000)

                | ((asu16[i] & 0x07e0) << 5 & 0x00fc00)
                | ((asu16[i] & 0x0020) << 4 & 0x000200)
                | ((asu16[i] & 0x0020) << 3 & 0x000100)

                | ((asu16[i] & 0xf800) >> 8 & 0x0000f8)
                | ((asu16[i] & 0x0800) >> 9 & 0x000004)
                | ((asu16[i] & 0x0800) >> 10 & 0x000002)
                | ((asu16[i] & 0x0800) >> 11 & 0x000001);
        }
        break;
    case format_t::RGBA8888:
        for (i = 0; i < pixels; i++) {
            out[i] = asu32[i];
        }
        break;
    }

    return outbytes;
}

}  // namespace anm::opener
