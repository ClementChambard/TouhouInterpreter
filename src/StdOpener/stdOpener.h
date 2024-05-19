#ifndef STDOPENER_H
#define STDOPENER_H

#include <vector>
#include <string>
#include <cstdint>

namespace StdOpener {

struct std_header_t {
    uint16_t nb_objects;
    uint16_t nb_faces;
};

struct std_header_06_t {
    uint16_t nb_objects;
    uint16_t nb_faces;
    uint32_t faces_offset;
    uint32_t script_offset;
    uint32_t unknown;
    char stage_name[128];
    char song1_name[128];
    char song2_name[128];
    char song3_name[128];
    char song4_name[128];
    char song1_path[128];
    char song2_path[128];
    char song3_path[128];
    char song4_path[128];
};

struct std_object_t {
    uint16_t unknown;
    uint16_t size;
    uint16_t script_index;
    uint16_t vmid;
    float x;
    float y;
    float z;
    float width;
    float height;
};

struct std_header_10_t {
    uint16_t nb_objects;
    uint16_t nb_faces;
    uint32_t faces_offset;
    uint32_t script_offset;
    uint32_t unknown;
    char anm_name[128];
    uint32_t offsets[];
};

struct std_object_instance_t {
    uint16_t object_id;
    uint16_t unknown1;
    float x;
    float y;
    float z;
};

struct std_entry_header_t {
    uint16_t id;
    uint8_t layer;
    uint8_t vmAliveBits;
    float x;
    float y;
    float z;
    float width;
    float height;
    float depth;
    std_object_t quads[];
};

struct std_entry_t {
    std_entry_header_t* header;
    std::vector<std_object_t*> quads;
};

struct std_instr_t {
    uint32_t time;
    uint16_t type;
    uint16_t size;
    uint8_t args[];
};

struct thstd_t {
    unsigned char* map;
    int64_t map_size;

    std_header_t* header;
    std_header_06_t* header_06;
    std_header_10_t* header_10;

    std::vector<std_entry_t*> entries;
    std::vector<std_object_instance_t*> instances;
    std::vector<std_instr_t*> instrs;
};

extern thstd_t* std_read_file(std::string const& filename);
extern void std_free(thstd_t* std);

}  // namespace StdOpener

#endif /* STDOPENER_H */
