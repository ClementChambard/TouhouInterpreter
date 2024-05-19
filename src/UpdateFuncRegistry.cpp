#include "./UpdateFuncRegistry.h"

UpdateFuncRegistry* const UPDATE_FUNC_REGISTRY = new UpdateFuncRegistry();

i32 UpdateFuncRegistry::run_all_on_tick() {
    UpdateFunc* current_update_func;
    UpdateFuncList_t* cur_node = on_game_tick_list_head.list_node.next;
    i32 ret = 0;
    do {
        do {
            /* check the next non-empty func */
            if (!cur_node)
                return ret;
            this->current_node = cur_node->next;
            current_update_func = cur_node->entry;
            cur_node = cur_node->next;
        } while (!current_update_func->function_ptr);
    // if (current_update_func->flags & 2) {
    Passtest:
        if (!is_cleaning_up) {
            /* Do something depending on the function output */
            switch (current_update_func->function_ptr()) {
            case 0:
                unregister(current_update_func);
                cur_node = current_node;
                ret++;
                break;
            default:
                cur_node = current_node;
                ret++;
                break;
            case 2:
                if (current_update_func->flags & 2)
                    goto Passtest;
                cur_node = current_node;
                ret++;
                break;
            case 3:
                return 1;
            case 4:
            case 8:
                return 0;
            case 5:
                return -1;
            case 6:
                cur_node = on_game_tick_list_head.list_node.next;
                ret = 0;
                break;
            case 7:
                if (current_update_func->on_cleanup)
                    current_update_func->on_cleanup();
                cur_node = current_node;
                ret++;
                break;
            }
            //}
        }
    } while (true);
}

i32 UpdateFuncRegistry::run_all_on_draw() {
    UpdateFunc* func;
    i32 ret = 0;
    UpdateFuncList_t* cur_node = on_draw_list_head.list_node.next;
    do {
        do {
            /* check the next non-empty func */
            if (!cur_node)
                return ret;
            current_node = cur_node->next;
            func = cur_node->entry;
            cur_node = cur_node->next;
        } while (!func->function_ptr);
    // if (func->flags & 2) {
    PassTestdraw:
        switch (func->function_ptr()) {
        case 0:
            unregister(func);
        default:
            break;
        case 2:
            if (func->flags & 2)
                goto PassTestdraw;
            break;
        case 3:
            return 1;
        case 4:
            return 0;
        case 5:
            return -1;
        }
        //}
        ret += 1;
        cur_node = current_node;
    } while (true);
}

i32 UpdateFuncRegistry::register_on_tick(UpdateFunc* func, i32 priority) {
    i32 ret = 0;
    if (func->on_registration != nullptr) {
        ret = func->on_registration();
        func->on_registration = nullptr;
    }
    func->priority = priority;
    UpdateFuncList_t* prev_node = &on_game_tick_list_head.list_node;
    UpdateFuncList_t* cur_node = on_game_tick_list_head.list_node.next;
    while ((cur_node != nullptr && (cur_node->entry->priority < priority))) {
        prev_node = cur_node;
        cur_node = cur_node->next;
    }
    if (prev_node->next != nullptr) {
        func->list_node.next = prev_node->next;
        prev_node->next->prev = &func->list_node;
    }
    prev_node->next = &func->list_node;
    func->list_node.prev = prev_node;
    return ret;
}

i32 UpdateFuncRegistry::register_on_draw(UpdateFunc* func, i32 priority) {
    i32 ret = 0;
    if (func->on_registration != nullptr) {
        ret = func->on_registration();
        func->on_registration = nullptr;
    }
    func->priority = priority;
    UpdateFuncList_t* prev_node = &on_draw_list_head.list_node;
    UpdateFuncList_t* cur_node = on_draw_list_head.list_node.next;
    while ((cur_node != nullptr && (cur_node->entry->priority < priority))) {
        prev_node = cur_node;
        cur_node = cur_node->next;
    }
    if (prev_node->next != nullptr) {
        func->list_node.next = prev_node->next;
        prev_node->next->prev = &func->list_node;
    }
    prev_node->next = &func->list_node;
    func->list_node.prev = prev_node;
    return ret;
}

void UpdateFuncRegistry::unregister(UpdateFunc* func) {
    if (!func)
        return;
    if (func->list_node.prev)
        func->list_node.prev->next = func->list_node.next;
    if (func->list_node.next)
        func->list_node.next->prev = func->list_node.prev;
    delete func;
}

void UpdateFuncRegistry::unregister_from(UpdateFuncList_t* node
                                         [[maybe_unused]]) {
    // UpdateFuncList_t* prev_node = node[1].prev;
    // while (node) {
    //     UpdateFunc* func = prev_node->entry;
    //     prev_node = prev_node->next;
    //     if (func != nullptr)
    //         unregister(func);
    // }
}
