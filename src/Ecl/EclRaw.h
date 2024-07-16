#ifndef ECLRAW_H_
#define ECLRAW_H_

#include <vector>
#include <defines.h>

struct EclRawHeader_t {
    char magic[4];
    u16 unknown1; /* 1 */
    u16 include_length; /* include_offset + ANIM+ECLI length */
    u32 include_offset; /* sizeof(EclRawHeader_t) */
    u32 zero1;
    u32 sub_count;
    u32 zero2[4];
};

struct EclRawIncList_t {
    char magic[4];
    u32 count;
    byte data[];
};

struct EclRawInstr_t {
    u32 time;
    u16 id;
    u16 size;
    u16 param_mask;
    /* The rank bitmask.
     *   1111LHNE
     * Bits mean: easy, normal, hard, lunatic. The rest are always set to 1. */
    u8 rank_mask;
    /* There doesn't seem to be a way of telling how many parameters there are
     * from the additional data. */
    u8 param_count;
    /* From TH13 on, this field stores the number of current stack references
     * in the parameter list. */
    u32 zero;
    byte data[];
};

struct EclRawSub_t {
    char magic[4];
    u32 data_offset; /* sizeof(EclRawSub_t) */
    u32 zero[2];
    byte data[];
};

struct EclSubPtr_t {
    cstr name;
    const EclRawSub_t* subHeader;
    std::vector<const EclRawInstr_t*> instrs;
};

struct EclRaw_t {
    const EclRawHeader_t* header;
    std::vector<cstr> anim_list;
    std::vector<cstr> ecli_list;
    std::vector<EclSubPtr_t> subs;
};

EclRaw_t* ecl_open(cstr filename);
void ecl_close(EclRaw_t* ecl);

#endif // ECLRAW_H_
