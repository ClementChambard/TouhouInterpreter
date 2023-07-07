#ifndef UPDATEFUNCREGISTRY_H_
#define UPDATEFUNCREGISTRY_H_

#include <functional>

struct UpdateFunc;
struct UpdateFuncList_t {
    UpdateFunc* entry = nullptr;
    UpdateFuncList_t* next = nullptr;
    UpdateFuncList_t* prev = nullptr;
};

struct UpdateFunc {
    int32_t priority = 0;
    int32_t flags = 1;
    std::function<int(void)> function_ptr = nullptr;
    std::function<int(void)> on_registration = nullptr;
    std::function<int(void)> on_cleanup = nullptr;
    UpdateFuncList_t list_node = { this, nullptr, nullptr };

    explicit UpdateFunc(std::function<int(void)> f)
        : function_ptr(f) {}
};

struct UpdateFuncRegistry {
    UpdateFunc on_game_tick_list_head = UpdateFunc(nullptr);
    UpdateFunc on_draw_list_head = UpdateFunc(nullptr);
    UpdateFuncList_t* current_node = nullptr;
    int32_t is_cleaning_up = 0;

    int run_all_on_tick();
    int run_all_on_draw();
    int register_on_tick(UpdateFunc* f, int32_t priority);
    int register_on_draw(UpdateFunc* f, int32_t priority);
    void unregister(UpdateFunc* f);
    void unregister_from(UpdateFuncList_t* node);
};

extern UpdateFuncRegistry* const UPDATE_FUNC_REGISTRY;

#endif // UPDATEFUNCREGISTRY_H_
