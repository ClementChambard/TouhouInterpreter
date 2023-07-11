#ifndef ASCIIMANAGER_INCLUDED_H
#define ASCIIMANAGER_INCLUDED_H

#include "./UpdateFuncRegistry.h"
#include "./AnmOpener/AnmFile.h"
#include <string>

struct AsciiStr_t {
    char text[256];
    glm::vec3 pos;
    NSEngine::Color color;
    glm::vec2 scale;
    int32_t __unused;
    int32_t __field_11c;
    int32_t font_id;
    int32_t draw_shadows;
    int32_t render_group;
    int32_t remaining_time;
    int32_t align_h;
    int32_t align_v;
};

class AsciiManager {
public:
    AsciiManager();
    ~AsciiManager();

    int render_group(int i);
    void render_string(AsciiStr_t const& s);
    void create_string(glm::vec3 const& pos, std::string const& str);
    void create_string_f(glm::vec3 const& pos, const char* fmt, ...);
    void comma_separated_number(glm::vec3 const& pos, int number);
    void draw_score(glm::vec3 const &pos, int num, int units);
private:
    int f_on_tick();
public:
    int32_t __field0_0x0 = 0;
    int32_t __field1_0x4 = 2;
    UpdateFunc *on_tick = nullptr;
    UpdateFunc *on_draw = nullptr;
    AnmVM __vm_1 = {};
    AnmVM __vm_2 = {};
    AsciiStr_t strings[320] = {};
    int32_t num_strings = 0;
    NSEngine::Color color = {255, 255, 255, 255};
    glm::vec2 scale = {1.0, 1.0};
    int32_t __str_field_11c = 0;
    int32_t __unused = 0;
    int32_t draw_shadows = 0;
    int32_t font_id = 0;
    int32_t group = 0;
    int32_t duration = 0;
    int32_t alignment_mode_h = 0;
    int32_t alignment_mode_v = 0;
    int32_t character_spacing_for_font_0 = 9;
    int32_t num_ticks_alive = 0;
    AnmFile* ascii_anm = nullptr;
    AnmID __unknown_anm_id = 0;
    AnmID id_for__now_loading = 0;
    UpdateFunc* on_draw_2 = nullptr;
    UpdateFunc* on_draw_3 = nullptr;
};

extern AsciiManager* ASCII_MANAGER_PTR;

#endif // !ASCIIMANAGER_INCLUDED_H
