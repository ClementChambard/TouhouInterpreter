#ifndef ASCIIPOPUPMANAGER_INCLUDED_H
#define ASCIIPOPUPMANAGER_INCLUDED_H

#include "./AnmOpener/AnmFile.h"
#include "./UpdateFuncRegistry.h"

struct AsciiPopupString_t {
    char nums[8] = {};
    glm::vec3* pos_ptr = nullptr;
    glm::vec3 pos = {0, 0, 0};
    float __field_18;
    NSEngine::Color color = {0, 0, 0, 0};
    int32_t time = 0;
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

    void generate_small_score_popup(glm::vec3 const &pos, int nb,
                                    NSEngine::Color color);
private:
    int f_on_tick();
    int f_on_draw();

public:
    uint32_t flags = 2;
    UpdateFunc* on_tick = nullptr;
    UpdateFunc* on_draw = nullptr;
    AnmFile* anm_ascii = nullptr;
    int32_t next_index = 0;
    int32_t next_index2 = 0;
    AnmVM vm = {};
    AsciiPopupString_t popups[36] = {};
};

extern AsciiPopupManager* POPUP_MANAGER_PTR;

#endif // !ASCIIPOPUPMANAGER_INCLUDED_H
