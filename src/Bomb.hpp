#ifndef BOMB_INCLUDED_H
#define BOMB_INCLUDED_H

#include <glm/glm.hpp>
#include "AnmOpener/AnmVM.h"
#include "Enemy.h"
#include "PosVel.h"
#include <Timer.hpp>
#include "UpdateFuncRegistry.h"

class Bomb {
public:
    Bomb();
    virtual ~Bomb();

    static void initialize();
    static void start();

    virtual void begin() = 0;
    virtual void cleanup() = 0;
    virtual int method_c() = 0;
    virtual int method_10() = 0;
protected:
    virtual int f_on_tick_() = 0;
    virtual int f_on_draw_() = 0;
private:
    int f_on_tick();
    int f_on_draw();
public:

    // undefined4
    UpdateFunc* on_tick = nullptr;
    UpdateFunc* on_draw = nullptr;
    int32_t field_0x10 = 0;
    glm::vec3 field_0x14 = {};
    int32_t field_0x20 = 0;
    int32_t field_0x24 = 0;
    int32_t field_0x28 = 0;
    float field_0x2c = 0.0;
    int32_t active = 0;
    ns::Timer_t timer_0x34 = {};
    int32_t field_0x48 = 0;
    int32_t field_0x4c = 0;
    int32_t field_0x50 = 0;
    int32_t field_0x54 = 0;
    int32_t field_0x58 = 0;
    anm::ID anmid_0x5c = 0;
    anm::ID anmid_0x60 = 0;
    anm::ID anmid_0x64 = 0;
    int32_t field_0x68 = 0;
    int32_t field_0x6c = 0;
    void* ptr_0x70 = nullptr;
    int32_t field_0x74 = 0;
    float field_0x78 = 0;
    ns::Timer_t timer_0x7c = {};
    int32_t field_0x90 = 0;
    glm::vec3 field_0x94 = {};
};

extern Bomb* BOMB_PTR;

extern bool player_is_trying_to_bomb();

class BombReimu : public Bomb {
public:
    BombReimu() {}
    ~BombReimu() override {}

    void begin() override;
    void cleanup() override;
    int method_c() override { return 0; }
    int method_10() override;

    struct Buffer_t {
        void explode_all();
        struct Orb_t {
            void init(int id, glm::vec3 const& center, int dmg);
            void explode();
            void update();
            anm::ID anmid = 0;
            PosVel pos = {};
            int32_t field12 = 0;
            int32_t field13 = 0;
            int32_t field14 = 0;
            int32_t field15 = 0;
            int32_t field16 = 0;
            int32_t field17 = 0;
            int32_t field18 = 0;
            int32_t field19 = 0;
            int32_t field1a = 0;
            int32_t field1b = 0;
            int32_t field1c = 0;
            int32_t field1d = 0;
            int32_t field1e = 0;
            int32_t field1f = 0;
            int32_t field20 = 0;
            int32_t field21 = 0;
            int32_t field22 = 0;
            int32_t field23 = 0;
            int32_t field24 = 0;
            int32_t field25 = 0;
            int32_t field26 = 0;
            int32_t field27 = 0;
            int32_t field28 = 0;
            ns::Timer_t time_alive = {};
            glm::vec3 velocity = {};
            int32_t field2f = 0;
            int32_t field30 = 0;
            int32_t closest_enemy_id = 0;
            Enemy* enm = nullptr;
            int32_t id = 0;
            int32_t ds = 0;
            int32_t field35 = 0;
            float field36 = 0;
        } orbs[0x18] = {};
    };
protected:
    int f_on_tick_() override;
    int f_on_draw_() override { return 1; }
};

class BombMarisa : public Bomb {
public:
    BombMarisa() {}
    ~BombMarisa() override {}

    void begin() override;
    void cleanup() override {}
    int method_c() override { return 0; }
    int method_10() override;
protected:
    int f_on_tick_() override;
    int f_on_draw_() override { return 1; }
};

class BombYoumu : public Bomb {
public:
    BombYoumu() {}
    ~BombYoumu() override {}

    void begin() override;
    void cleanup() override {}
    int method_c() override { return 0; }
    int method_10() override { return 0; }
protected:
    int f_on_tick_() override;
    int f_on_draw_() override { return 1; }
public:
    anm::ID anmids[0x104] = {};
    float floats[0x104] = {};    // 0x0410
    float floats2[0x104] = {};   // 0x0820
    ns::Timer_t timers[0x104] = {};  // 0x0c30
    int32_t field2080 = 0;       // 0x2080
    int32_t field2084 = 0;       // 0x2084
    ns::Timer_t timer2088 = 0;       // 0x2088
};

class BombEmpty : public Bomb {
public:
    BombEmpty() {}
    ~BombEmpty() override {}

    void begin() override {}
    void cleanup() override {}
    int method_c() override { return 0; }
    int method_10() override { return 0; }
protected:
    int f_on_tick_() override { return 1; }
    int f_on_draw_() override { return 1; }
};

#endif // !BOMB_INCLUDED_H
