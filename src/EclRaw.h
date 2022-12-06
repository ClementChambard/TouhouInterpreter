#ifndef ECLRAW_H_
#define ECLRAW_H_

#include <iostream>
#include <vector>

struct EclRawHeader_t {
    char magic[4];
    uint16_t unknown1; /* 1 */
    uint16_t include_length; /* include_offset + ANIM+ECLI length */
    uint32_t include_offset; /* sizeof(EclRawHeader_t) */
    uint32_t zero1;
    uint32_t sub_count;
    uint32_t zero2[4];
};

struct EclRawIncList_t {
    char magic[4];
    uint32_t count;
    unsigned char data[];
};

struct EclRawInstr_t {
    uint32_t time;
    uint16_t id;
    uint16_t size;
    uint16_t param_mask;
    /* The rank bitmask.
     *   1111LHNE
     * Bits mean: easy, normal, hard, lunatic. The rest are always set to 1. */
    uint8_t rank_mask;
    /* There doesn't seem to be a way of telling how many parameters there are
     * from the additional data. */
    uint8_t param_count;
    /* From TH13 on, this field stores the number of current stack references
     * in the parameter list. */
    uint32_t zero;
    unsigned char data[];
};

struct EclRawSub_t {
    char magic[4];
    uint32_t data_offset; /* sizeof(EclRawSub_t) */
    uint32_t zero[2];
    unsigned char data[];
};

struct EclSubPtr_t {
    std::string name;
    const EclRawSub_t* subHeader;
    std::vector<const EclRawInstr_t*> instrs;
};

struct EclRaw_t {
    const EclRawHeader_t* header;
    std::vector<std::string> anim_list;
    std::vector<std::string> ecli_list;
    std::vector<EclSubPtr_t> subs;
};

EclRaw_t* ecl_open(std::string filename);
void ecl_close(EclRaw_t* ecl);

#endif // ECLRAW_H_
