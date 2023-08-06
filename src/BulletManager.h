#ifndef BULLETMANAGER_H_
#define BULLETMANAGER_H_

#include "./Bullet.h"
#include "./BulletHandler.h"
#include "./UpdateFuncRegistry.h"
#include <glm/glm.hpp>

class BulletManager {
public:
    static BulletManager* GetInstance();

    BulletManager();
    ~BulletManager();
    BulletManager(BulletManager const&) = delete;
    BulletManager& operator=(BulletManager const&) = delete;

    void Shoot(EnemyBulletShooter_t* bh);

    void ClearScreen(int mode, float r = -1.f, float x = 0.f, float y = 0.f);

    void set_bounce_rect(float w, float h) {
        et_bounce_rect_w = w;
        et_bounce_rect_h = h;
    }
    void set_protect_range(float r) { et_protect_range = r; }
    int get_nb_bul() const { return nb_bullets; }

// private:
    static constexpr size_t max_bullet = 2000;
    void AddBullet(Bullet* b);
    void RemoveBullet(Bullet* b);
    void ShootSingle(EnemyBulletShooter_t* bh, float a, float s, glm::vec2 pos);
    int on_tick();
    int on_draw();

    // undefined4 __unknown;
    UpdateFunc* f_on_tick;
    UpdateFunc* f_on_draw;
    // undefined4 __unknown;
    Bullet* layer_list_heads[6] = {};
    Bullet* layer_list_tails[6] = {};
    int32_t nb_bullets = 0;
    float et_protect_range = 0.f;
    float et_bounce_rect_w = 0.f;
    float et_bounce_rect_h = 0.f;
    // undefined4 __unknown;
    // undefined4 __unknown;
    int32_t graze_recent[20] = {};
    // undefined4 __unknown;
    BulletList_t freelist_head = {};
    BulletList_t tick_list_head = {};
    // undefined4 __unknown;
    // undefined4 __unknown;
    // undefined4 __unknown;
    // undefined4 __unknown;
    // undefined4 __unknown;
    // undefined4 __unknown;
    // undefined4 __unknown;
    // undefined4 __unknown;
    Bullet list_0_tail_dummy_bullet = {};
    Bullet bullets[max_bullet] = {};
    // AnmID anm_ids[max_bullet+1] = {};
    int32_t bullet_count_canceled_by_bombs = 0;
    int32_t __unknown_cancel_counter = 0;
    // undefined4 __unknown;
    BulletList_t* iter_current = nullptr;
    BulletList_t* iter_next = nullptr;
    AnmFile* bullet_anm = nullptr;

    // no lolk ?
    // struct zBullet __lolk_snapshot_bullets[max_bullet];
    // struct zAnmId __lolk_snapshot_anm_ids[max_bullet];
    // int32_t __lolk_snapshot__unknown_cancel_counter;

    friend class Bullet;
};

extern BulletManager* BULLET_MANAGER_PTR;

#endif // BULLETMANAGER_H_
