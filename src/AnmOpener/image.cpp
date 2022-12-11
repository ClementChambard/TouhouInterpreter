#include "image.h"


namespace AnmOpener {

    unsigned int format_Bpp(format_t format)
    {
        switch ((int)format) {
        case FORMAT_RGBA8888:
        case FORMAT_BGRA8888:
            return 4;
        case FORMAT_ARGB4444:
        case FORMAT_RGB565:
            return 2;
        case FORMAT_GRAY8:
            return 1;
        default:
            std::cout << "unknown format " << format << '\n';
            abort();
            return 1;
        }
    }

    unsigned char* format_to_rgba(const unsigned char* data, unsigned int pixels, format_t format)
    {
        unsigned int i;
        uint32_t* out = new uint32_t[pixels];

        if (format == FORMAT_GRAY8) {
            for (i = 0; i < pixels; ++i) {
                out[i] = 0xff000000
                    | (data[i] << 16 & 0xff0000)
                    | (data[i] <<  8 &   0xff00)
                    | (data[i] <<  0 &     0xff);
            }
        } else if (format == FORMAT_BGRA8888) {
            unsigned char* out8 = (unsigned char*)out;
            for (i = 0; i < pixels; ++i) {
                out8[i * sizeof(uint32_t) + 0] = data[i * sizeof(uint32_t) + 2];
                out8[i * sizeof(uint32_t) + 1] = data[i * sizeof(uint32_t) + 1];
                out8[i * sizeof(uint32_t) + 2] = data[i * sizeof(uint32_t) + 0];
                out8[i * sizeof(uint32_t) + 3] = data[i * sizeof(uint32_t) + 3];
            }
        } else if (format == FORMAT_ARGB4444) {
            for (i = 0; i < pixels; ++i) {
                /* Extends like this: 0x0 -> 0x00, 0x3 -> 0x33, 0xf -> 0xff.
                * It's required for proper alpha. */
                out[i] = ((data[i * sizeof(uint16_t) + 1] & 0xf0) << 24 & 0xf0000000)
                    | ((data[i * sizeof(uint16_t) + 1] & 0xf0) << 20 & 0x0f000000)
                    | ((data[i * sizeof(uint16_t) + 0] & 0x0f) << 20 &   0xf00000)
                    | ((data[i * sizeof(uint16_t) + 0] & 0x0f) << 16 &   0x0f0000)
                    | ((data[i * sizeof(uint16_t) + 0] & 0xf0) <<  8 &     0xf000)
                    | ((data[i * sizeof(uint16_t) + 0] & 0xf0) <<  4 &     0x0f00)
                    | ((data[i * sizeof(uint16_t) + 1] & 0x0f) <<  4 &       0xf0)
                    | ((data[i * sizeof(uint16_t) + 1] & 0x0f) <<  0 &       0x0f);
            }
        } else if (format == FORMAT_RGB565) {
            uint16_t* u16 = (uint16_t*)data;
            for (i = 0; i < pixels; ++i) {
                /* Bit-extends channels: 00001b -> 00001111b. */
                out[i] = 0xff000000
                    | ((u16[i] & 0x001f) << 19 & 0xf80000)
                    | ((u16[i] & 0x0001) << 16 & 0x040000)
                    | ((u16[i] & 0x0001) << 16 & 0x020000)
                    | ((u16[i] & 0x0001) << 16 & 0x010000)

                    | ((u16[i] & 0x07e0) <<  5 & 0x00fc00)
                    | ((u16[i] & 0x0020) <<  4 & 0x000200)
                    | ((u16[i] & 0x0020) <<  3 & 0x000100)

                    | ((u16[i] & 0xf800) >>  8 & 0x0000f8)
                    | ((u16[i] & 0x0800) >>  9 & 0x000004)
                    | ((u16[i] & 0x0800) >> 10 & 0x000002)
                    | ((u16[i] & 0x0800) >> 11 & 0x000001);
            }
        } else if (format == FORMAT_RGBA8888) {
            uint32_t* u32 = (uint32_t*)data;
            for (i = 0; i < pixels; i++) {
                out[i] = u32[i];
            }
        } else {
            fprintf(stderr, "unknown format: %u\n", format);
            abort();
        }

        return (unsigned char*)out;
    }

}
