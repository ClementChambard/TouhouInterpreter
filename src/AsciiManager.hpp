#ifndef ASCIIMANAGER_INCLUDED_H
#define ASCIIMANAGER_INCLUDED_H

#include "./UpdateFuncRegistry.h"
#include "./Anm/AnmFile.h"

struct AsciiStr_t {
    char text[256];
    ns::vec3 pos;
    ns::Color color;
    ns::vec2 scale;
    i32 __unused;
    i32 __field_11c;
    i32 font_id;
    i32 draw_shadows;
    i32 render_group;
    i32 remaining_time;
    i32 align_h;
    i32 align_v;
};

class AsciiManager {
public:
    AsciiManager();
    ~AsciiManager();

    i32 render_group(i32 i);
    void render_string(AsciiStr_t const& s);
    void create_string(ns::vec3 const& pos, cstr str);
    void create_string_f(ns::vec3 const& pos, cstr fmt, ...);
    void comma_separated_number(ns::vec3 const& pos, i32 number);
    void draw_score(ns::vec3 const &pos, i32 num, i32 units);
private:
    i32 f_on_tick();
public:
    i32 __field0_0x0 = 0;
    i32 __field1_0x4 = 2;
    UpdateFunc *on_tick = nullptr;
    UpdateFunc *on_draw = nullptr;
    anm::VM __vm_1 = {};
    anm::VM __vm_2 = {};
    AsciiStr_t strings[320] = {};
    i32 num_strings = 0;
    ns::Color color = {255, 255, 255, 255};
    ns::vec2 scale = {1.0, 1.0};
    i32 __str_field_11c = 0;
    i32 __unused = 0;
    i32 draw_shadows = 0;
    i32 font_id = 0;
    i32 group = 0;
    i32 duration = 0;
    i32 alignment_mode_h = 0;
    i32 alignment_mode_v = 0;
    i32 character_spacing_for_font_0 = 9;
    i32 num_ticks_alive = 0;
    anm::File* ascii_anm = nullptr;
    anm::ID __unknown_anm_id = 0;
    anm::ID id_for__now_loading = 0;
    UpdateFunc* on_draw_2 = nullptr;
    UpdateFunc* on_draw_3 = nullptr;
};

extern AsciiManager* ASCII_MANAGER_PTR;

#endif // !ASCIIMANAGER_INCLUDED_H
