#ifndef SPELLCARD_INCLUDED_H
#define SPELLCARD_INCLUDED_H

#include <string>
#include "UpdateFuncRegistry.h"

class Spellcard {
    public:
        static Spellcard* GetInstance();
        Spellcard();
        ~Spellcard();

        void update();

        void Init(int id, int time, int mode, std::string name);
        void Stop();
        void SetTimeout() {}

    private:
        static constexpr int spellcard_anm_1 = 13;
        static constexpr int spellcard_anm_2 = 20;

        int on_tick() { update(); return 1; } // TODO
        int on_draw() { return 1; } //TODO

        UpdateFunc* f_on_tick;
        UpdateFunc* f_on_draw;
        int spell_circle_anmid = 0;
        float spell_circle_x, spell_circle_y, spell_circle_z;
};

#endif
