#ifndef ANMTYPES_H_
#define ANMTYPES_H_

#include <defines.h>

namespace anm::opener {

    enum class format_t : i16 {
        BGRA8888 = 1,
        RGB565 = 3,
        ARGB4444 = 5,
        GRAY8 = 7,
        RGBA8888 = -1,
    };

    struct sprite_t {
        u32 id = 0;
        f32 x = 0,
            y = 0,
            w = 0,
            h = 0;
    };

    struct anm_instr0_t {
        i16 time;
        u8 type;
        u8 length;
        byte data[];
    };

    struct anm_instr_t {
        u16 type;
        u16 length;
        i16 time;
        u16 param_mask;
        byte data[];
    };

    struct anm_offset_t {
        i32 id;
        u32 offset;
    };

    struct anm_header06_t {
        u32 sprites;
        u32 scripts;
        /* Set to the texture slot at runtime, zero in the files itself. */
        u32 rt_textureslot;
        u32 w, h;
        u32 format;
        /* ARGB. Mostly zero, but a few are 0xff000000 (opaque black). */
        u32 colorkey;
        u32 nameoffset;
        /* XXX: X is unused here. */
        /* XXX: Y stores the secondary name offset for TH06.
        *      There is no secondary name when it is zero. */
        u32 x, y;
        /* 0: TH06
        * 2: TH07
        * 3: TH08, TH09
        * 4: TH09.5, TH10
        * 7: TH11, TH12, TH12.5
        * 8: TH13+ */
        u32 version;
        /* Memory priority for Direct3D textures created for this entry. */
        /* Supported from TH07 up until alcostg. */
        /* 0  - Random things, everything in TH06 where priority is unsupported */
        /* 1  - data/ascii/loading.png for TH08 and TH09. */
        /* 10 - Mostly sprites? */
        /* 11 - Used mainly for backgrounds and ascii.png. */
        u32 memorypriority;
        u32 thtxoffset;
        /* Can also be communicated by setting the first char of the file name to
        * '@'. In TH06, this is the only way, that game doesn't use this field. */
        u16 hasdata;
        /* From TH14 on, used to mark images as "needing to be scaled down when
        * running in lower resolutions". Which is something different than
        * "high-res" since images with separate versions for each resolution
        * (most notably ascii*.png) have 0 rather than 1 here. */
        u16 lowresscale;
        u32 nextoffset;
        u32 zero3;
    };

    struct anm_header11_t
    {
        u32 version;
        u16 sprites;
        u16 scripts;
        /* Actually zero. */
        u16 zero1;
        u16 w, h;
        u16 format;
        u32 nameoffset;
        u16 x, y;
        /* As of TH16.5, unused in every game that uses this stucture, but still
        * present in the files. */
        u32 memorypriority;
        u32 thtxoffset;
        u16 hasdata;
        u16 lowresscale;
        u32 nextoffset;
        u32 zero2[6];
    };

    struct thtx_header_t {
        char magic[4];
        u16 zero;
        u16 format;
        u16 w, h;
        u32 size;
        byte data[];
    };

} // namsepace anm::opener

#endif // ANMTYPES_H_
