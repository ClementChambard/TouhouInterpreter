#include "./BulletManager.h"
#include "./Player.h"
#include <NSEngine.h>
#include <math/Random.h>

BulletManager::BulletManager() {
    BulletList_t* pred = &freelist_head;
    BulletList_t* current = nullptr;
    for (size_t i = 0; i < max_bullet; i++) {
        current = &bullets[i].freelist_node;
        current->value = &bullets[i];
        pred->next = current;
        current->previous = pred;
        pred = current;
        bullets[i].tick_list_node.value = &bullets[i];
    }
    bullet_anm = AnmManager::LoadFile(7, "bullet.anm");
    f_on_tick = new UpdateFunc([this]() { return this->on_tick(); });
    f_on_draw = new UpdateFunc([this]() { return this->on_draw(); });
    UPDATE_FUNC_REGISTRY->register_on_tick(f_on_tick, 29);
    UPDATE_FUNC_REGISTRY->register_on_draw(f_on_draw, 38);
}

BulletManager::~BulletManager() {
    UPDATE_FUNC_REGISTRY->unregister(f_on_tick);
    UPDATE_FUNC_REGISTRY->unregister(f_on_draw);
}

BulletManager* BulletManager::GetInstance() {
    static BulletManager* inst = new BulletManager();
    return inst;
}

void BulletManager::ClearScreen(int mode, float r, float x, float y) {
    if (r == -1) {
        while (tick_list_head.next) {
            tick_list_head.next->value->cancel(mode & 1);
            RemoveBullet(tick_list_head.next->value);
        }
        return;
    }
    std::vector<Bullet*> toRemove;
    for (auto n = tick_list_head.next; n != nullptr; n = n->next) {
        Bullet* b = n->value;
        if (!b)
            continue;
        if ((mode & 2) && b->ex_invuln__remaining_frames > 0)
            continue;

        if (math::point_distance_sq(x, y, b->pos.x, b->pos.y) < r * r) {
            b->cancel(mode & 1);
            toRemove.push_back(b);
        }
    }
    for (auto b : toRemove)
        RemoveBullet(b);
}


int BulletManager::on_tick() {
    Bullet* bullet;
    /* shift the graze_recent array, then
       set the graze for the current frame to 0 */
    for (int i = 19; i > 0; i--) {
        graze_recent[i] = graze_recent[i - 1];
    }
    graze_recent[0] = 0;
    /* set iterators */
    iter_current = tick_list_head.next;
    if (iter_current == nullptr) {
        bullet = nullptr;
        iter_next = nullptr;
    } else {
        bullet = iter_current->value;
        iter_next = iter_current->next;
    }
    /* reset some variables */
    nb_bullets = 0;
    for (int i = 0; i < 6; i++) {
        layer_list_heads[i] = nullptr;
        layer_list_tails[i] = nullptr;
    }
    /* loop for all bullets in the tick list */
    do {
        if (bullet == nullptr) {
            return 1;
        }
        // if game is not paused ?
        // if ((GAME_THREAD_PTR == NULL) ||
        //     ((GAME_THREAD_PTR->flags & 0x400U) == 0)) {
        if ((bullet->flags & 0x100) != 0) {
            if (bullet->state == 2) {
                if (7 < bullet->__timer_e54) {
                    // Bullet::check_player_collision(bullet,1);
                    goto end_tick;
                }
            } else if (bullet->state == 1) {
                // Bullet::check_player_collision(bullet,1);
                goto end_tick;
            }
        }
        if (bullet->on_tick() == 0)
            goto end_tick;
        //}
        // else {
    /* if bullet is visible --> add it to its layer list */
    end_tick:
        if ((bullet->flags & 0x200) == 0) {
            if (layer_list_heads[bullet->layer] == nullptr) {
                layer_list_heads[bullet->layer] = bullet;
            } else {
                layer_list_tails[bullet->layer]->next_in_layer = bullet;
            }
            layer_list_tails[bullet->layer] = bullet;
            bullet->next_in_layer = NULL;
        }
        nb_bullets = nb_bullets + 1;
        bullet->__timer_e54++;
        //}
        /* update iterators */
        iter_current = iter_next;
        if (iter_next == nullptr) {
            iter_next = nullptr;
            bullet = nullptr;
        } else {
            bullet = iter_current->value;
            iter_next = iter_next->next;
        }
    } while (true);
}

int BulletManager::on_draw() {
    // if ((GAME_THREAD_PTR == NULL) ||
    //     ((*(byte *)&GAME_THREAD_PTR->flags & 4) == 0)) {
    auto layer_head = layer_list_heads;
    for (int i = 0; i < 6; i++) {
        for (Bullet* bullet = *layer_head; bullet != NULL;
             bullet = bullet->next_in_layer) {
            bullet->vm.entity_pos = bullet->pos;
            if (bullet->vm.bitflags.autoRotate) {
                float a = bullet->angle + 1.570796;
                math::angle_normalize(a);
                bullet->vm.rotation.z = a;
                bullet->vm.bitflags.rotated = true;
            }
            if (bullet->flags & 0x40) {
                bullet->vm.bitflags.scaled = true;
                bullet->vm.scale_2 = { bullet->scale, bullet->scale };
            }
            bullet->vm.setLayer(17); // TODO(ClementChambard): remove this
            bullet->vm.draw();
        }
        layer_head++;
    }
    //}
    return 1;
}

///// aim modes /////
// AT -> at player // ST -> static
#define AIM_AT 0
#define AIM_ST 1
#define AIM_AT_RING 2
#define AIM_ST_RING 3
#define AIM_AWAY_RING 4
#define AIM_ST_RING2 5
#define AIM_RAND 6
#define AIM_RAND_RING 7
#define AIM_MEEK 8
#define AIM_AT_PYRAMID 9
#define AIM_ST_PYRAMID 10
#define AIM_PEANUT 11
#define AIM_PEANUT2 12

void BulletManager::Shoot(EnemyBulletShooter_t* bh) {
    // use cnt & aim_mode to shoot single bullets (and play sound)
    glm::vec2 pos = bh->__vec3_8;
    float AngToPlayer = math::point_direction(pos.x, pos.y,
                            PLAYER_PTR->inner.pos.x, PLAYER_PTR->inner.pos.y);
    float aRing = PI2 / static_cast<float>(bh->cnt_count);
    // fire bullet :
    if (bh->aim_type < 9) {
        for (int j = 0; j < bh->cnt_count; j++) {
            for (float i = 0; i < bh->cnt_layers; i++) {
                float a = 0.f;
                float jj = static_cast<int>(j - (bh->cnt_count / 2)) +
                           (1 - (bh->cnt_count % 2)) * 0.5f;
                switch (bh->aim_type) {
                case AIM_AT:
                    a += AngToPlayer;
                    [[fallthrough]];
                case AIM_ST:
                    a += bh->ang_aim + jj * bh->ang_bullet_dist;
                    break;
                case AIM_RAND:
                case AIM_MEEK:
                    a = Random::Floatm11() * bh->ang_bullet_dist + bh->ang_aim;
                    break;
                case AIM_ST_RING2:
                    a -= AngToPlayer;
                    [[fallthrough]];
                case AIM_AWAY_RING:
                    a += aRing / 2;
                    [[fallthrough]];
                case AIM_AT_RING:
                    a += AngToPlayer;
                    [[fallthrough]];
                case AIM_ST_RING:
                case AIM_RAND_RING:
                    a += (j * aRing) + bh->ang_aim + (i * bh->ang_bullet_dist);
                    break;
                }
                float s = bh->spd1 + (bh->spd2 - bh->spd1) *
                    static_cast<float>(i) / static_cast<float>(bh->cnt_layers);
                if (bh->aim_type == AIM_RAND_RING || bh->aim_type == AIM_MEEK)
                    s = bh->spd1 + (bh->spd2 - bh->spd1) * Random::Float01();
                ShootSingle(bh, a, s, pos);
            }
        }
    } else if (bh->aim_type < 11) {
        for (int i = 0; i < bh->cnt_count; i++) {
            for (int j = 0; j < bh->cnt_layers / 2 + 1; j++) {
                float a = 0;
                a = aRing * i + bh->ang_aim;
                if (bh->aim_type == 9)
                    a += AngToPlayer;
                float a1 = a - j * bh->ang_bullet_dist;
                float a2 = a + j * bh->ang_bullet_dist;
                float s = bh->spd2 + (bh->spd1 - bh->spd2) *
                    (1 - static_cast<float>(j) / (bh->cnt_layers / 2.f + 1));
                ShootSingle(bh, a1, s, pos);
                if (j != 0)
                    ShootSingle(bh, a2, s, pos);
            }
        }
    }
    // NSEngine::AudioEngine::PlaySound(snd1);
    // std::cout << "Shooting " << bh->cnt_count * bh->cnt_layers << " : {\n";
}

void BulletManager::ShootSingle(EnemyBulletShooter_t* bh,
                                float a, float s, glm::vec2 pos) {
    if (!freelist_head.next)
        return;
    Bullet* b = freelist_head.next->value;
    AddBullet(b);

    // angle normalize shoot angle
    b->Reset(); // TODO(ClementChambard): reset should occur when destroyed

    b->pos = glm::vec3(pos +
                glm::vec2 { bh->distance * cos(a), bh->distance * sin(a) }, 0);
    // remove if too close to player TODO

    b->angle = a;
    b->speed = s;
    b->flags |= 1;
    b->state = 1;
    b->ex_invuln__remaining_frames = 0;
    b->scale = 1.0;
    b->scale_i.end_time = 0;
    b->pos_i.end_time = 0;
    b->__timer_e24 = 0;
    b->__timer_e38 = 0;
    b->__timer_e54 = 0;
    b->__timer_e68 = 0;
    b->velocity = { s * cos(a), s * sin(a), 0.f };
    b->color = bh->__color;
    b->type = bh->type;
    // b->field21_0x680 = 0;
    // b->field32_0xe4c = 60;
    b->flags = 18;
    b->transform_sound = bh->__shot_transform_sfx;
    b->__field_664_had_5 = 5;
    b->__field_678_had_35 = bh->sfx_flag;
    b->active_ex_flags = 0;
    b->__ex_goto_b_loop_count = 0;
    b->sprite_data = BULLET_TYPE_TABLE[bh->type];
    b->__hitbox_diameter_copy = b->sprite_data["default_radius"].asFloat();
    b->layer = b->sprite_data["default_layer"].asInt();
    b->hitbox_diameter = b->__hitbox_diameter_copy;
    b->ex_index = bh->__start_transform;

    // copy ex
    for (int i = 0; i < 20; i++)
        b->et_ex[i] = bh->ex[i];

    // init vm
    b->vm(AnmManager::getLoaded(7)->getPreloaded(
        b->sprite_data["script"].asInt()));
    b->vm.setEntity(static_cast<void*>(b));
    b->vm.setLayer(15);
    b->vm.index_of_sprite_mapping_func = 1;
    AnmManager::getLoaded(7)->setSprite(&b->vm, b->vm.sprite_id);
    b->vm.update();
    b->vm.bitflags.originMode = 0b01;

    // Spawn anim wierd and cancel id
    /*if (b->sprite_data["__field_114"].asInt() != 0) {
      b->anmExtraId = BULLET_MANAGER_PTR->bullet_anm->create(&bul_mgr2, b->sprite_data["__field_114"].asInt(), &bullet->pos,0,-1,nullptr);
    }*/

    // Set cancel_sprite_id ... TODO investigate
    switch (b->sprite_data["__field_10c"].asInt()) {
    case 0:
        b->cancel_sprite_id = bh->__color * 2 + 4;
        break;
    case 1:
        b->cancel_sprite_id = -1;
        // BULLET_ADDITIONAL_CANCEL_SCR[color];
        break;
    case 2:
        b->cancel_sprite_id = -1;
        break;
    case 3:
        b->cancel_sprite_id = 16;
        break;
    case 4:
        b->cancel_sprite_id = 6;
    default:
        break;
    case 6:
        b->cancel_sprite_id = b->sprite_data["colors"][bh->__color]
                ["cancel_script"].asInt();
        break;
    case 7:
        b->cancel_sprite_id = 0x104;
        break;
    case 8:
        b->cancel_sprite_id = 0x107;
        break;
    case 9:
        b->cancel_sprite_id = 0x10a;
        break;
    case 10:
        b->cancel_sprite_id = 0x113;
    }

    // select the right interrupt of the anm depending on spawn anim ETEX
    if (bh->ex[bh->__start_transform].type == 2) {
        if (bh->ex[bh->__start_transform].a != 1)
            b->vm.interrupt(bh->ex[bh->__start_transform].a + 7);
        b->state = 2;
        b->pos -= glm::vec3(b->speed * cos(b->angle),
                            b->speed * sin(b->angle), 0.f) * 4.f;
        b->ex_index = bh->__start_transform + 1;
    } else {
        b->vm.interrupt(2);
    }

    // run once
    b->run_et_ex();
    b->vm.update();
}

void BulletManager::AddBullet(Bullet* b) {
    if (b->freelist_node.previous)
        b->freelist_node.previous->next = b->freelist_node.next;
    if (b->freelist_node.next)
        b->freelist_node.next->previous = b->freelist_node.previous;
    b->freelist_node.next = nullptr;
    b->freelist_node.previous = nullptr;

    b->tick_list_node.next = tick_list_head.next;
    b->tick_list_node.previous = &tick_list_head;
    if (tick_list_head.next)
        tick_list_head.next->previous = &b->tick_list_node;
    tick_list_head.next = &b->tick_list_node;
}

void BulletManager::RemoveBullet(Bullet* b) {
    b->Reset();

    if (b->tick_list_node.previous)
        b->tick_list_node.previous->next = b->tick_list_node.next;
    if (b->tick_list_node.next)
        b->tick_list_node.next->previous = b->tick_list_node.previous;
    b->tick_list_node.previous = nullptr;
    b->tick_list_node.next = nullptr;

    b->freelist_node.next = freelist_head.next;
    b->freelist_node.previous = &freelist_head;
    if (freelist_head.next)
        freelist_head.next->previous = &b->freelist_node;
    freelist_head.next = &b->freelist_node;
}
