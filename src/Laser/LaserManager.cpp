#include "./LaserManager.h"

LaserManager* LASER_MANAGER_PTR = nullptr;

LaserManager::LaserManager() {
    if (LASER_MANAGER_PTR)
        delete LASER_MANAGER_PTR;
    current_id = 0x10000;
    LASER_MANAGER_PTR = this;

    bullet_anm = AnmManager::LoadFile(7, "bullet.anm");
    // if (bullet_anm == NULL) {
    // sub_4025a0_logs_debug_message(this_00,(int)&OUTPUT_BUF_FOR_STRINGS,&DAT_004a145c);
    // delete this;
    // return;
    //}

    on_tick = new UpdateFunc([this]() { return this->f_on_tick(); });
    on_tick->flags &= 0xfffffffd;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 28);

    on_draw = new UpdateFunc([this]() { return this->f_on_draw(); });
    on_draw->flags &= 0xfffffffd;
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, 28);

    list_head = &dummy_laser_for_list_tail;
}

LaserManager::~LaserManager() {
    if (on_tick)
        UPDATE_FUNC_REGISTRY->unregister(on_tick);
    if (on_draw)
        UPDATE_FUNC_REGISTRY->unregister(on_draw);
    destroy_all();

    LASER_MANAGER_PTR = nullptr;
}

Laser* find_laser_by_id(int id) {
    if (id == 0)
        return nullptr;
    for (auto laser = LASER_MANAGER_PTR->dummy_laser_for_list_tail.prev;
        laser; laser = laser->prev) {
        if (laser->laser_id == id)
            return laser;
    }

    return nullptr;
}

void LaserManager::destroy_all() {
    Laser* laser = dummy_laser_for_list_tail.prev;
    while (laser != NULL) {
        Laser* prev = laser->prev;
        delete laser;
        laser = prev;
    }
    this->list_length = 0;
    this->list_head = &this->dummy_laser_for_list_tail;
    return;
}

int LaserManager::cancel_all(bool as_bomb) {
    Laser* prev = (LASER_MANAGER_PTR->dummy_laser_for_list_tail).prev;
    for (Laser* laser = prev; laser != nullptr; laser = prev) {
        prev = laser->prev;
        if (laser->__field_10__set_to_3_by_ex_delete != 1)
            laser->cancel(as_bomb, 0);
    }
    return 1;
}

int LaserManager::cancel_in_radius(glm::vec3 const& pos, bool item, int a, float r) {
    int cnt = 0;
    auto l = dummy_laser_for_list_tail.prev;
    Laser* l2;
    cancel_rectangle_p1 = pos;
    while (l2 = l, l2) {
        l = l2->prev;
        if (l2->__field_10__set_to_3_by_ex_delete != 1) {
            cnt += l2->cancel_as_bomb_circle(pos, r, item, a);
        }
    }
    return cnt;
}
    
int LaserManager::cancel_in_rectangle(glm::vec2 const& pos, glm::vec2 const& scale, float angle) {
    auto prev = dummy_laser_for_list_tail.prev;
    int cnt = 0;
    cancel_rectangle_p1 = {pos, 0};
    cancel_rectangle_p2 = {scale, 0};
    Laser* laser;
    while (laser = prev, laser) {
        prev = laser->prev;
        if (laser->__field_10__set_to_3_by_ex_delete != 1 &&
           (laser->some_flags & 1)) {
            cnt += laser->cancel_as_bomb_rectangle(
                {pos, 0}, {scale, 0}, angle, 0, 1);
        }
    }
    return cnt;
}

int LaserManager::f_on_tick() {
    // Temprorarily change game speed to 0 ????

    uint uVar4;

    Laser* laser = (this->dummy_laser_for_list_tail).prev;
    Laser* prev;
    do {
        while (true) {
            if (laser == NULL) {
                return 1;
            }
            uVar4 = laser->some_flags;
            prev = laser->prev;
            if ((uVar4 & 6) == 0)
                break;
            uVar4 = (((uVar4 + 2) ^ uVar4) & 6) ^ uVar4;
            laser->some_flags = uVar4;
            if (((uint8_t)uVar4 & 6) < 4)
                break;
        LAB_00435455:
            this->list_length = this->list_length + -1;
            if (this->list_head == laser) {
                this->list_head = laser->next;
            }

            delete laser;
            laser = prev;
        }
        if (laser->__field_10__set_to_3_by_ex_delete == 1)
            goto LAB_00435455;
        if ((uVar4 & 8) == 0) {
            if (laser->on_tick() != 0)
                goto LAB_00435455;
            laser->__timer_18++;
            laser->some_flags |= 1;
            laser = prev;
        } else {
            laser->check_graze_or_kill(1);
            laser = prev;
        }
    } while (true);
}

int LaserManager::f_on_draw() {
    // if gamethread flag 0x4 is 0
    for (auto l = dummy_laser_for_list_tail.prev; l != nullptr; l = l->prev)
        l->on_draw();
    return 1;
}

int allocate_new_laser(int32_t type, void* init_arg) {
    Laser* newLaser;
    if (LASER_MANAGER_PTR->list_length > 511)
        return 0;

    LASER_MANAGER_PTR->current_id++;
    if (LASER_MANAGER_PTR->current_id < 0x10000)
        LASER_MANAGER_PTR->current_id = 0x10000;

    switch (type) {
    case 0:
        newLaser = new LaserLine();
        break;
    case 1:
        newLaser = new LaserInfinite();
        break;
    case 2:
        newLaser = new LaserCurve();
        break;
    case 3:
        newLaser = new LaserBeam();
        break;
    default:
        return LASER_MANAGER_PTR->current_id;
    }

    newLaser->laser_id = LASER_MANAGER_PTR->current_id;
    newLaser->next = LASER_MANAGER_PTR->list_head;
    LASER_MANAGER_PTR->list_head->prev = newLaser;
    LASER_MANAGER_PTR->list_length++;
    LASER_MANAGER_PTR->list_head = newLaser;
    newLaser->initialize(init_arg);
    return LASER_MANAGER_PTR->current_id;
}
