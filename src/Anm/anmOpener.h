#ifndef ANMOPENER_H_
#define ANMOPENER_H_

#include "./anmTypes.h"
#include "./image.h"
#include <vector>

namespace anm::opener {

    struct anm_script_t {
        anm_offset_t* offset;
        std::vector<anm_instr_t*> instrs;
    };

    struct opcode_fmt_t {
        u16 type;
        cstr format;
    };

    struct anm_entry_t {
        anm_header11_t* header;
        thtx_header_t* thtx;

        pstr name;
        pstr name2;

        std::vector<sprite_t*> sprites;
        std::vector<anm_script_t*> scripts;

        bytes data;
    };

    struct anm_archive_t {
        bytes map;
        isize map_size;

        std::vector<pstr> names;
        std::vector<anm_entry_t*> entries;
    };

    struct anm_instr_arg_t {
        union {
            i32 S;
            f32 f;
        } val;
        char type;
    };

    anm_archive_t* anm_read_file(cstr filename);

    void anm_free(anm_archive_t* anm);

    std::vector<anm_instr_arg_t> anm_instr_get_args(anm_instr_t* instr);

    const std::vector<std::pair<u32, cstr>>& getFormats();

    image_t anm_get_image(anm_entry_t* entry);
} // namespace anm::opener

#endif // ANMOPENER_H_
