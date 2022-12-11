#ifndef ANMBITFLAGS_H_
#define ANMBITFLAGS_H_

#include <cstdint>

#define ANMVM_BIT_VISIBLE (1 <<  0)
#define ANMVM_BIT_530_1   (1 <<  1)
#define ANMVM_BIT_ROTATED (1 <<  2)
#define ANMVM_BIT_SCALED  (1 <<  3)
#define ANMVM_BIT_ZOOMED  (1 <<  4)
#define ANMVM_BIT_BLEND0  (1 <<  5)
#define ANMVM_BIT_BLEND1  (1 <<  6)
#define ANMVM_BIT_BLEND2  (1 <<  7)
#define ANMVM_BIT_BLEND3  (1 <<  8)
#define ANMVM_BIT_530_9   (1 <<  9)
#define ANMVM_BIT_ALTPOS  (1 << 10)
#define ANMVM_BIT_FLIP_X  (1 << 11)
#define ANMVM_BIT_FLIP_Y  (1 << 12)
#define ANMVM_BIT_ZWRITDI (1 << 13)
#define ANMVM_BIT_530_14  (1 << 14)
#define ANMVM_BIT_530_15  (1 << 15)
#define ANMVM_BIT_530_16  (1 << 16)
#define ANMVM_BIT_COLMOD0 (1 << 17)
#define ANMVM_BIT_COLMOD1 (1 << 18)
#define ANMVM_BIT_530_19  (1 << 19)
#define ANMVM_BIT_530_20  (1 << 20)
#define ANMVM_BIT_ANCHOR0 (1 << 21)
#define ANMVM_BIT_ANCHOR1 (1 << 22)
#define ANMVM_BIT_ANCHOR2 (1 << 23)
#define ANMVM_BIT_ANCHOR3 (1 << 24)
#define ANMVM_BIT_MODE0   (1 << 25)
#define ANMVM_BIT_MODE1   (1 << 26)
#define ANMVM_BIT_MODE2   (1 << 27)
#define ANMVM_BIT_MODE3   (1 << 28)
#define ANMVM_BIT_MODE4   (1 << 29)
#define ANMVM_BIT_SCROLY0 (1 << 30)
#define ANMVM_BIT_SCROLY1 (1 << 31)

#define ANMVM_BIT_SCROLX0 (1 <<  0)
#define ANMVM_BIT_SCROLX1 (1 <<  1)
#define ANMVM_BIT_ROTMOD0 (1 <<  2)
#define ANMVM_BIT_ROTMOD1 (1 <<  3)
#define ANMVM_BIT_ROTMOD2 (1 <<  4)
#define ANMVM_BIT_ACTIVE1 (1 <<  5)
#define ANMVM_BIT_ACTIVE2 (1 <<  6)
#define ANMVM_BIT_AUTOROT (1 <<  7)
#define ANMVM_BIT_534_8   (1 <<  8)
#define ANMVM_BIT_534_9   (1 <<  9)
#define ANMVM_BIT_RANDMOD (1 << 10)
#define ANMVM_BIT_RESAMPL (1 << 11)
#define ANMVM_BIT_534_12  (1 << 12)
#define ANMVM_BIT_534_13  (1 << 13)
#define ANMVM_BIT_534_14  (1 << 14)
#define ANMVM_BIT_534_15  (1 << 15)
#define ANMVM_BIT_NOPAREN (1 << 16)
#define ANMVM_BIT_534_17  (1 << 17)
#define ANMVM_BIT_ORIGIN0 (1 << 18)
#define ANMVM_BIT_ORIGIN1 (1 << 19)
#define ANMVM_BIT_RESMOD0 (1 << 20)
#define ANMVM_BIT_RESMOD1 (1 << 21)
#define ANMVM_BIT_RESMOD2 (1 << 22)
#define ANMVM_BIT_PAR_ROT (1 << 23)
#define ANMVM_BIT_HAS_VEL (1 << 24)
#define ANMVM_BIT_COL_CHI (1 << 25)
#define ANMVM_BIT_534_26  (1 << 26)
#define ANMVM_BIT_534_27  (1 << 27)
#define ANMVM_BIT_534_28  (1 << 28)
#define ANMVM_BIT_534_29  (1 << 29)
#define ANMVM_BIT_534_30  (1 << 30)
#define ANMVM_BIT_534_31  (1 << 31)

struct AnmVM_flags_t {

    AnmVM_flags_t() { *(uint64_t*)this = 0; }
    uint32_t visible:1;    /* 0x00000001  |  0xfffffffe */
    uint32_t f530_1:1;     /* 0x00000002  |  0xfffffffd */
    uint32_t rotated:1;    /* 0x00000004  |  0xfffffffb */
    uint32_t scaled:1;     /* 0x00000008  |  0xfffffff7 */
    uint32_t zoomed:1;     /* 0x00000010  |  0xffffffef */
    uint32_t blendmode:4;  /* 0x000001e0  |  0xfffffe1f */
    uint32_t f530_9:1;     /* 0x00000200  |  0xfffffdff */
    uint32_t alt_pos:1;    /* 0x00000400  |  0xfffffbff */
    uint32_t flip_x:1;     /* 0x00000800  |  0xfffff7ff */
    uint32_t flip_y:1;     /* 0x00001000  |  0xffffefff */
    uint32_t zwritedis:1;  /* 0x00002000  |  0xffffdfff */
    uint32_t f530_14:1;    /* 0x00004000  |  0xffffbfff */
    uint32_t f530_15:1;    /* 0x00008000  |  0xffff7fff */
    uint32_t f530_16:1;    /* 0x00010000  |  0xfffeffff */
    uint32_t colmode:2;    /* 0x00060000  |  0xfff9ffff */
    uint32_t f530_19_20:2; /* 0x00180000  |  0xffe7ffff */
    uint32_t anchorX:2;    /* 0x00600000  |  0xff9fffff */
    uint32_t anchorY:2;    /* 0x01800000  |  0xfe7fffff */
    uint32_t rendermode:5; /* 0x3e000000  |  0xc1ffffff */
    uint32_t scrollY:2;    /* 0xc0000000  |  0x3fffffff */

    uint32_t scrollX:2;
    uint32_t rotationMode:3;
    uint32_t activeFlags:2;
    uint32_t autoRotate:1;
    uint32_t f534_8:1;
    uint32_t f534_9:1;
    uint32_t randomMode:1;
    uint32_t resampleMode:1;
    uint32_t f534_12:1;
    uint32_t ins419:1;
    uint32_t f534_14_15:2;
    uint32_t noParent:1;
    uint32_t f534_17:1;
    uint32_t originMode:2;
    uint32_t resolutionMode:3;
    uint32_t parRotate:1;
    uint32_t hasVel:1;
    uint32_t colorizeChildren:1;
    uint32_t f534_26:1;
    uint32_t f534_27_31:5;

};

#define RESET_FLAGS(f, r, n) f &= ~(n << r)

#define ANMVM_GET_VISIBLE ((bitflags_lo & ANMVM_BIT_VISIBLE) != 0)
#define ANMVM_GET_ROTATED ((bitflags_lo & ANMVM_BIT_ROTATED) != 0)
#define ANMVM_GET_SCALED  ((bitflags_lo & ANMVM_BIT_SCALED ) != 0)
#define ANMVM_GET_ZOOMED  ((bitflags_lo & ANMVM_BIT_ZOOMED ) != 0)
#define ANMVM_GET_ALTPOS  ((bitflags_lo & ANMVM_BIT_ALTPOS ) != 0)
#define ANMVM_GET_FLIPX   ((bitflags_lo & ANMVM_BIT_FLIPX  ) != 0)
#define ANMVM_GET_FLIPY   ((bitflags_lo & ANMVM_BIT_FLIPY  ) != 0)

#define ANMVM_GETL(flagname) ((bitflags_lo & ANMVM_BIT_ ## flagname ## ) != 0)
#define ANMVM_GETH(flagname) ((bitflags_hi & ANMVM_BIT_ ## flagname ## ) != 0)

#define ANMVM_SET_BLEND(v) {RESET_FLAGS(bitflags_lo, 5, 15); bitflags_lo |= ((v & 15) << 5);}
#define ANMVM_GET_BLEND ((bitflags_lo >> 5) & 15)

#define ANMVM_SET_COLMO(v) {RESET_FLAGS(bitflags_lo, 17, 3); bitflags_lo |= ((v & 3) << 17);}
#define ANMVM_GET_COLMO ((bitflags_lo >> 17) & 3)

#define ANMVM_SET_ANCHORH(v) {RESET_FLAGS(bitflags_lo, 21, 3); bitflags_lo |= ((v & 3) << 21);}
#define ANMVM_SET_ANCHORV(v) {RESET_FLAGS(bitflags_lo, 23, 3); bitflags_lo |= ((v & 3) << 23);}
#define ANMVM_GET_ANCHORH ((bitflags_lo >> 21) & 3)
#define ANMVM_GET_ANCHORV ((bitflags_lo >> 23) & 3)

#define ANMVM_SET_MODE(v) {RESET_FLAGS(bitflags_lo, 25, 31); bitflags_lo |= ((v & 31) << 25);}
#define ANMVM_GET_MODE ((bitflags_lo >> 25) & 31)

#define ANMVM_SET_SCROLY(v) {RESET_FLAGS(bitflags_lo, 30, 3); bitflags_lo |= ((v & 3) << 30);}
#define ANMVM_SET_SCROLX(v) {RESET_FLAGS(bitflags_hi,  0, 3); bitflags_hi |= ((v & 3) <<  0);}
#define ANMVM_GET_SCROLY ((bitflags_lo >> 30) & 3)
#define ANMVM_GET_SCROLX ((bitflags_hi >>  0) & 3)

#define ANMVM_SET_ROTMOD(v) {RESET_FLAGS(bitflags_hi,  2, 7); bitflags_hi |= ((v & 7) <<  2);}
#define ANMVM_GET_ROTMOD ((bitflags_hi >>  2) & 7)

#define ANMVM_SET_ACTIVE(v) {RESET_FLAGS(bitflags_hi,  5, 3); bitflags_hi |= ((v & 3) <<  5);}
#define ANMVM_GET_ACTIVE ((bitflags_hi >>  5) & 3)
#define ANMVM_DELETE 0b01
#define ANMVM_INACTIVE 0b00
#define ANMVM_ACTIVE 0b10
#define ANMVM_FROZEN 0b11

#define ANMVM_SET_ORIGIN(v) {RESET_FLAGS(bitflags_hi, 18, 3); bitflags_hi |= ((v & 3) << 18);}
#define ANMVM_GET_ORIGIN ((bitflags_hi >> 18) & 3)

#define ANMVM_SET_RESMOD(v) {RESET_FLAGS(bitflags_hi, 20, 7); bitflags_hi |= ((v & 7) << 20);}
#define ANMVM_GET_RESMOD ((bitflags_hi >> 20) & 7)

#endif // ANMBITFLAGS_H_
