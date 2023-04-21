#ifndef SPELLCARD_INCLUDED_H
#define SPELLCARD_INCLUDED_H

#include "UpdateFuncRegistry.h"
#include <string>

class Spellcard {
public:
    static Spellcard* GetInstance();
    Spellcard();
    ~Spellcard();
    Spellcard(Spellcard const&) = delete;
    Spellcard& operator=(Spellcard const&) = delete;

    void update();

    void Init(int id, int time, int mode, std::string name, int bossId = 0);
    void Stop();
    void SetTimeout() { }

private:
    static constexpr int spellcard_anm_1 = 13;
    static constexpr int spellcard_anm_2 = 20;

    int on_tick()
    {
        update();
        return 1;
    } // TODO
    int on_draw() { return 1; } // TODO

public:
    int spell_bg_anm_id = 0;
    int bossId = 0;
    int flags = 0;

    UpdateFunc* f_on_tick;
    UpdateFunc* f_on_draw;
    int spell_circle_anmid = 0;
    float spell_circle_x, spell_circle_y, spell_circle_z;
};

extern Spellcard* SPELLCARD_PTR;

#endif
