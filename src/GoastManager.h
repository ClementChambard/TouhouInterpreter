#ifndef GOASTMANAGER_H_
#define GOASTMANAGER_H_

#include "./UpdateFuncRegistry.h"
#include <glm/glm.hpp>

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

    int32_t flags = 0;
    UpdateFunc* on_tick = nullptr;
    UpdateFunc* on_draw = nullptr;
    // anm file
    int32_t token_count = 0;
    TokenList_t list_head {};
    int32_t field_0x24 = 0;
    int32_t field_0x28 = 0;
    int32_t field_0x2c = 0;
    int32_t field_0x30 = 0;
    int32_t field_0x34 = 0;
    int32_t field_0x38 = 0;
    int32_t field_0x3c = 0;
    int32_t field_0x40 = 0;
    int32_t field_0x44 = 0;
    int32_t field_0x48 = 0;
    int32_t field_0x4c = 0;
    int32_t field_0x50 = 0;
};

extern GoastManager* GOAST_MANAGER_PTR;

#endif // GOASTMANAGER_H_
