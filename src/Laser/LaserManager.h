#ifndef LASERMANAGER_H_
#define LASERMANAGER_H_

#include "../UpdateFuncRegistry.h"
#include "../Anm/AnmFile.h"
#include "./Laser.h"
#include "./LaserBeam.h"
#include "./LaserCurve.h"
#include "./LaserInfinite.h"
#include "./LaserLine.h"

class LaserManager {
public:
    LaserManager();
    ~LaserManager();
    LaserManager(LaserManager const&) = delete;
    LaserManager& operator=(LaserManager const&) = delete;

    void destroy_all();
    int cancel_all(bool as_bomb);
    int cancel_in_radius(ns::vec3 const& pos, bool item,
                         int a, float r);
    int cancel_radius_as_bomb(ns::vec3 const& pos, bool item, float r);
    int cancel_in_rectangle(ns::vec2 const& pos, ns::vec2 const& scale, float angle);

private:
    int f_on_tick();
    int f_on_draw();

public:
    // undefined4 maybe vtable so doesn't matter
    UpdateFunc* on_tick = nullptr;
    UpdateFunc* on_draw = nullptr;
    Laser dummy_laser_for_list_tail = {};
    Laser* list_head = nullptr;
    int32_t list_length = 0;
    int32_t current_id = 0;
    ns::vec3 cancel_rectangle_p1 = {};
    ns::vec3 cancel_rectangle_p2 = {};
    anm::File* bullet_anm = nullptr;
    // undefined4
    // undefined4

    friend class Laser;
    friend class LaserLine;
    friend class LaserInfinite;
    friend class LaserCurve;
    friend class LaserBeam;
    friend Laser* find_laser_by_id(int id);
    friend int allocate_new_laser(int32_t type, void* init_arg);
};

int allocate_new_laser(int32_t type, void* init_arg);
Laser* find_laser_by_id(int id);

constexpr int LASER_ON_SPRITE_SET_FUNC = 2;
constexpr int LASER_CURVE_ON_SPRITE_SET_FUNC = 3;

extern LaserManager* LASER_MANAGER_PTR;

#endif // LASERMANAGER_H_
