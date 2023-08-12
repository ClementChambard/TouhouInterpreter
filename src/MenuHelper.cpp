#include "./MenuHelper.hpp"

void MenuHelper_t::select(int32_t i) {
    if (num_choices == 0) {
        selection = i;
        return;
    }
    if (i >= num_choices) {
        i = num_choices - 1;
    } else if (i < 0) {
        i = 0;
    }
    selection = i;
    // increment selection until it isn't on a disabled option
    while (true) {
        for (int i = 0; true; i++) {
            if (disabled_option_count <= i) return;
            if (disabled_options[i] == selection) break;
        }
        selection++;
        if (num_choices <= selection) selection = 0;
    }
}

void MenuHelper_t::select_rel(int32_t i) {
    if (num_choices < 1) return;
    // repeat move until it is not on a disabled option.
    // might cause an infinite loop if no wrap & first / last option disabled
    while (true) {
        selection += i;
        while (selection >= num_choices) {
            if (option_wrapping) {
                selection -= num_choices;
            } else {
                selection = num_choices - 1;
            }
        }
        while (selection < 0) {
            if (option_wrapping) {
                selection += num_choices;
            } else {
                selection = 0;
            }
        }
        for (int i = 0; true; i++) {
            if (disabled_option_count <= i) return;
            if (disabled_options[i] == selection) break;
        }
    }
}

void MenuHelper_t::push_selection() {
    stack_selection[stack_depth] = selection;
    stack_num_choices[stack_depth] = num_choices;
    stack_depth++;
    if (stack_depth > 15) stack_depth = 15;
    disabled_option_count = 0;
}

void MenuHelper_t::pop_selection() {
    if (stack_depth == 0) {
        selection = 0;
        disabled_option_count = 0;
        return;
    }
    stack_depth--;
    selection = stack_selection[stack_depth];
    num_choices = stack_num_choices[stack_depth];
    disabled_option_count = 0;
}

void MenuHelper_t::disable_option(int32_t i) {
    disabled_options[disabled_option_count] = i;
    disabled_option_count++;
    // increment selection until it isn't on a disabled option
    while (true) {
        for (int i = 0; true; i++) {
            if (disabled_option_count <= i) return;
            if (disabled_options[i] == selection) break;
        }
        selection++;
        if (selection >= num_choices) selection = 0;
    }
}
