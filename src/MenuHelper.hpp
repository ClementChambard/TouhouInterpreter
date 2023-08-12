#ifndef MENUHELPER_INCLUDED_H
#define MENUHELPER_INCLUDED_H

#include <cstdint>

struct MenuHelper_t {
    int32_t selection = 0;
    int32_t current_selection = 0;
    int32_t num_choices = 0;
    int32_t stack_selection[16] = {};
    int32_t stack_num_choices[16] = {};
    int32_t stack_depth = 0;
    int32_t disabled_options[16] = {};
    int32_t option_wrapping = 0;
    int32_t disabled_option_count = 0;

    void select(int32_t i);
    void select_rel(int32_t i);
    void push_selection();
    void pop_selection();
    void disable_option(int32_t i);
};

#endif // !MENUHELPER_INCLUDED_H

