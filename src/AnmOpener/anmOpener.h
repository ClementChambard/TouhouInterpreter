#ifndef ANMOPENER_H_
#define ANMOPENER_H_

#include <vector>
#include "anmTypes.h"
#include "image.h"

namespace AnmOpener {

    struct anm_script_t {
        anm_offset_t* offset;
        std::vector<anm_instr_t*> instrs;
    };

    struct opcode_fmt_t {
        uint16_t type;
        const char* format;
    };

    struct anm_entry_t {
        anm_header11_t* header;
        thtx_header_t* thtx;

        char* name;
        char* name2;

        std::vector<sprite_t*> sprites;
        std::vector<anm_script_t*> scripts;

        unsigned char* data;
    };

    struct anm_archive_t {
        unsigned char* map;
        long map_size;

        std::vector<char*> names;
        std::vector<anm_entry_t*> entries;
    };

    struct anm_instr_arg_t {
        union {
            int32_t S;
            float f;
        } val;
        char type;
    };

    anm_archive_t* anm_read_file(std::string filename);

    void anm_free(anm_archive_t* anm);

    std::vector<anm_instr_arg_t> anm_instr_get_args(anm_instr_t* instr);

    void anm_entry_descriptor(anm_entry_t* entry, unsigned int& entry_num);
    void anm_sprite_descriptor(sprite_t* sprite);
    void anm_script_descriptor(anm_script_t* script);

    const std::vector<std::pair<uint32_t, std::string>>& getFormats();

    image_t anm_get_image(anm_entry_t* entry);

    void display_img(image_t* img);

    void anm_dump(const anm_archive_t* anm);
}

#endif // ANMOPENER_H_
