#ifndef ASCIIPOPUPMANAGER_INCLUDED_H
#define ASCIIPOPUPMANAGER_INCLUDED_H

#include "./Anm/AnmFile.h"
#include "./UpdateFuncRegistry.h"

struct AsciiPopupString_t {
    char nums[8] = {};
    ns::vec3* pos_ptr = nullptr;
    ns::vec3 pos = {0, 0, 0};
    float __field_18;
    ns::Color color = {0, 0, 0, 0};
    ns::Timer_t time = 0;
    // undefined4
    int32_t __field_32__was_1281 = 0;
    char active = 0;
    char nb_nums = 0;
    // undefined2 padding ?
    float ddc_bonus_mult = 0.f;
    int32_t ddc_bonus = 0;
};

class AsciiPopupManager {
public:
    AsciiPopupManager();
    ~AsciiPopupManager();

    void generate_small_score_popup(ns::vec3 const &pos, int nb,
                                    ns::Color color);
private:
    i32 f_on_tick();
    i32 f_on_draw();

public:
    u32 flags = 2;
    UpdateFunc* on_tick = nullptr;
    UpdateFunc* on_draw = nullptr;
    anm::File* anm_ascii = nullptr;
    i32 next_index = 0;
    i32 next_index2 = 0;
    anm::VM vm = {};
    AsciiPopupString_t popups[36] = {};
};

extern AsciiPopupManager* POPUP_MANAGER_PTR;

#endif // !ASCIIPOPUPMANAGER_INCLUDED_H
