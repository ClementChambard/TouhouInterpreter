#ifndef ANMBITFLAGS_H_
#define ANMBITFLAGS_H_

#include <defines.h>

#define ANMVM_DELETE 0b01
#define ANMVM_DELETING 0b10
#define ANMVM_ACTIVE 0b00
#define ANMVM_FROZEN 0b11

namespace anm {

struct VM_flags_t {
  VM_flags_t() { flagsvalue = 0; }
  union {
    u64 flagsvalue;
    struct {
      u32 visible : 1;    /* 0x00000001  |  0xfffffffe */
      u32 f530_1 : 1;     /* 0x00000002  |  0xfffffffd */
      u32 rotated : 1;    /* 0x00000004  |  0xfffffffb */
      u32 scaled : 1;     /* 0x00000008  |  0xfffffff7 */
      u32 zoomed : 1;     /* 0x00000010  |  0xffffffef */
      u32 blendmode : 4;  /* 0x000001e0  |  0xfffffe1f */
      u32 f530_9 : 1;     /* 0x00000200  |  0xfffffdff */
      u32 alt_pos : 1;    /* 0x00000400  |  0xfffffbff */
      u32 flip_x : 1;     /* 0x00000800  |  0xfffff7ff */
      u32 flip_y : 1;     /* 0x00001000  |  0xffffefff */
      u32 zwritedis : 1;  /* 0x00002000  |  0xffffdfff */
      u32 f530_14 : 1;    /* 0x00004000  |  0xffffbfff */
      u32 f530_15 : 1;    /* 0x00008000  |  0xffff7fff */
      u32 f530_16 : 1;    /* 0x00010000  |  0xfffeffff */
      u32 colmode : 2;    /* 0x00060000  |  0xfff9ffff */
      u32 f530_19 : 1;    /* 0x00080000  |  0xfff7ffff */
      u32 f530_20 : 1;    /* 0x00100000  |  0xffefffff */
      u32 anchorX : 2;    /* 0x00600000  |  0xff9fffff */
      u32 anchorY : 2;    /* 0x01800000  |  0xfe7fffff */
      u32 rendermode : 5; /* 0x3e000000  |  0xc1ffffff */
      u32 scrollY : 2;    /* 0xc0000000  |  0x3fffffff */

      u32 scrollX : 2;           /* 0x00000003  |  0xfffffffc */
      u32 rotationMode : 3;      /* 0x0000001c  |  0xffffffe3 */
      u32 activeFlags : 2;       /* 0x00000060  |  0xffffff9f */
      u32 autoRotate : 1;        /* 0x00000080  |  0xffffff7f */
      u32 f534_8 : 1;            /* 0x00000100  |  0xfffffeff */
      u32 noSlowdown : 1;        /* 0x00000200  |  0xfffffdff */
      u32 randomMode : 1;        /* 0x00000400  |  0xfffffbff */
      u32 resampleMode : 1;      /* 0x00000800  |  0xfffff7ff */
      u32 f534_12 : 1;           /* 0x00001000  |  0xffffefff */
      u32 ins419 : 1;            /* 0x00002000  |  0xffffdfff */
      u32 f534_14_15 : 2;        /* 0x0000c000  |  0xffff3fff */
      u32 noParent : 1;          /* 0x00010000  |  0xfffeffff */
      u32 f534_17 : 1;           /* 0x00020000  |  0xfffdffff */
      u32 originMode : 2;        /* 0x000c0000  |  0xfff3ffff */
      u32 resolutionMode : 3;    /* 0x00700000  |  0xff8fffff */
      u32 parRotate : 1;         /* 0x00800000  |  0xff7fffff */
      u32 hasGrowth : 1;         /* 0x01000000  |  0xfeffffff */
      u32 colorizeChildren : 1;  /* 0x02000000  |  0xfdffffff */
      u32 f534_26 : 1;           /* 0x04000000  |  0xfbffffff */
      u32 f534_27_31 : 4;        /* 0x78000000  |  0x87ffffff */
      u32 fadeNearCamera : 1;    /* 0x80000000  |  0x7fffffff */
    };
  };
};

} // namespace anm

#endif // ANMBITFLAGS_H_
