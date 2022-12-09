#ifndef LASERMANAGER_H_
#define LASERMANAGER_H_

#include "../UpdateFuncRegistry.h"
#include "Laser.h"
#include "LaserLine.h"
#include "LaserCurve.h"
#include "LaserInfinite.h"
#include "LaserBeam.h"

class LaserManager {
    public:
        LaserManager();
        ~LaserManager();

        void destroy_all();


    private:
        int f_on_tick();
        int f_on_draw();

        // undefined4 maybe vtable so doesn't matter
        UpdateFunc* on_tick = nullptr;
        UpdateFunc* on_draw = nullptr;
        Laser dummy_laser_for_list_tail = {};
        Laser* list_head = nullptr;
        int32_t list_length = 0;
        int32_t current_id = 0;
        glm::vec3 __field_5ec = {};
        glm::vec3 __field_5f8 = {};
        // AnmLoaded
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

extern LaserManager* LASER_MANAGER_PTR;


#endif // LASERMANAGER_H_
