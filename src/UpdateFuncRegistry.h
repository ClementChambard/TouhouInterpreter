#ifndef UPDATEFUNCREGISTRY_H_
#define UPDATEFUNCREGISTRY_H_

#include <functional>
#include <defines.h>

struct UpdateFunc;
struct UpdateFuncList_t {
    UpdateFunc* entry = nullptr;
    UpdateFuncList_t* next = nullptr;
    UpdateFuncList_t* prev = nullptr;
};

struct UpdateFunc {
    i32 priority = 0;
    i32 flags = 1;
    std::function<i32(void)> function_ptr = nullptr;
    std::function<i32(void)> on_registration = nullptr;
    std::function<i32(void)> on_cleanup = nullptr;
    UpdateFuncList_t list_node = { this, nullptr, nullptr };

    explicit UpdateFunc(std::function<i32(void)> f)
        : function_ptr(f) {}
};

struct UpdateFuncRegistry {
    UpdateFunc on_game_tick_list_head = UpdateFunc(nullptr);
    UpdateFunc on_draw_list_head = UpdateFunc(nullptr);
    UpdateFuncList_t* current_node = nullptr;
    i32 is_cleaning_up = 0;

    i32 run_all_on_tick();
    i32 run_all_on_draw();
    i32 register_on_tick(UpdateFunc* f, i32 priority);
    i32 register_on_draw(UpdateFunc* f, i32 priority);
    void unregister(UpdateFunc* f);
    void unregister_from(UpdateFuncList_t* node);
};

extern UpdateFuncRegistry* const UPDATE_FUNC_REGISTRY;

#endif // UPDATEFUNCREGISTRY_H_
