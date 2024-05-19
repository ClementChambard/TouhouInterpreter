#ifndef SPELLCARD_INCLUDED_H
#define SPELLCARD_INCLUDED_H

#include "./UpdateFuncRegistry.h"
#include "AnmOpener/AnmVM.h"
#include <string>
#include <glm/glm.hpp>

class Spellcard {
public:
    static Spellcard* GetInstance();
    Spellcard();
    ~Spellcard();
    Spellcard(Spellcard const&) = delete;
    Spellcard& operator=(Spellcard const&) = delete;

    void update();

    void Init(int id, int time, int mode, std::string name);
    void Stop();
    void SetTimeout() { }

private:
    static constexpr int spellcard_anm_1 = 13;
    static constexpr int spellcard_anm_2 = 20;

    int on_tick();
    int on_draw();

public:
    UpdateFunc* f_on_tick;
    UpdateFunc* f_on_draw;
    anm::ID ascii_anmid_10 = 0;
    anm::ID text_anmid_14 = 0;
    anm::ID ascii_anmid_18 = 0;

    ns::Timer_t __timer_20 = 0;
    int duration = 1;

    int spell_id = 0;

    int spell_bg_anm_id = 0;
    int flags = 0;
    int bonus = 0;
    int bonus_max = 0;
    int field_0x88 = 0;
    int field_0x8c = 0;

    int spell_circle_anmid = 0;
    glm::vec3 boss0_pos = {};
};

extern Spellcard* SPELLCARD_PTR;

#endif
