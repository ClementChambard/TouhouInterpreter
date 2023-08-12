#ifndef GOASTMANAGER_H_
#define GOASTMANAGER_H_

#include "./UpdateFuncRegistry.h"
#include <glm/glm.hpp>
#include "./AnmOpener/AnmFile.h"
#include "AnmOpener/AnmVM.h"

struct Token_t;
struct TokenList_t {
    Token_t* entry = nullptr;
    TokenList_t* next = nullptr;
    TokenList_t* prev = nullptr;
};

struct Token_t {
    TokenList_t node {};
    int32_t anm_id = 0;
    int32_t token_type = 0;
    glm::vec3 pos {};
    glm::vec3 speed {};
    int32_t __timer_30 = 0;
    int32_t __timer_44 = 0;
    int32_t __timer_58 = 0;
    int32_t flags = 0;

    int update();
};

class GoastManager {
public:
    GoastManager();
    ~GoastManager();
    GoastManager(GoastManager const&) = delete;
    GoastManager& operator=(GoastManager const&) = delete;

    void delete_tokens();

    void choose_angle(Token_t* token, float angle);

    int spawn_token(glm::vec3 const& pos, int param_3, float param_4);

    // private:

    int _on_tick();

    int FUN_0040e920();
    void FUN_0040ee70();
    void create_otter_hyper_anms();
    void hyper_die(bool param_1);
    void update_gui(int tid);
    void hyper_end_spawn_items();
    void hyper_spawn_item(int i, glm::vec3 const& pos);
    int get_token(int typ);
    int hyper_start();
    void update_hyper_bar();
    void reset();

    int32_t flags = 0;
    UpdateFunc* on_tick = nullptr;
    UpdateFunc* on_draw = nullptr;
    AnmFile* anm_file = nullptr;
    int32_t token_count = 0;
    TokenList_t list_head {};
    AnmID gui_anmid = 0;
    AnmID hyper_time_bar_anmid1 = 0;
    AnmID hyper_time_bar_anmid2 = 0;
    AnmID hyper_bg_anmid = 0;
    int32_t field_0x34 = 0;
    AnmID hyper_aura_anmid = 0;
    AnmID extra_beast_anmid = 0;
    int32_t field_0x40 = 0;
    AnmID otter_hyper_anms[3] = {};
    int32_t field_0x50 = 0;
};

extern GoastManager* GOAST_MANAGER_PTR;

#endif // GOASTMANAGER_H_
