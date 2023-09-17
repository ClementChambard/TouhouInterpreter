#include "./AnmFuncs.h"

std::vector<pAnmFuncI_t> AnmFuncs::m_on_switch;
std::vector<pAnmFuncI_t> AnmFuncs::m_on_sprite_set;
std::vector<pAnmFunc_t> AnmFuncs::m_on_draw;
std::vector<pAnmFunc_t> AnmFuncs::m_on_tick;
std::vector<pAnmFunc_t> AnmFuncs::m_on_destroy;
std::vector<pAnmFunc_t> AnmFuncs::m_on_wait;
std::vector<pAnmFunc_t> AnmFuncs::m_on_create;

pAnmFuncI_t AnmFuncs::on_switch(size_t i) {
    if (i >= m_on_switch.size()) return nullptr;
    return m_on_switch[i];
}

pAnmFuncI_t AnmFuncs::on_sprite_set(size_t i) {
    if (i >= m_on_sprite_set.size()) return nullptr;
    return m_on_sprite_set[i];
}

pAnmFunc_t AnmFuncs::on_draw(size_t i) {
    if (i >= m_on_draw.size()) return nullptr;
    return m_on_draw[i];
}

pAnmFunc_t AnmFuncs::on_tick(size_t i) {
    if (i >= m_on_tick.size()) return nullptr;
    return m_on_tick[i];
}

pAnmFunc_t AnmFuncs::on_destroy(size_t i) {
    if (i >= m_on_destroy.size()) return nullptr;
    return m_on_destroy[i];
}

pAnmFunc_t AnmFuncs::on_wait(size_t i) {
    if (i >= m_on_wait.size()) return nullptr;
    return m_on_wait[i];
}

pAnmFunc_t AnmFuncs::on_create(size_t i) {
    if (i >= m_on_create.size()) return nullptr;
    return m_on_create[i];
}

void AnmFuncs::set_on_switch(size_t i, pAnmFuncI_t f) {
    if (i >= m_on_switch.size()) m_on_switch.resize(i + 1, nullptr);
    m_on_switch[i] = f;
}

void AnmFuncs::set_on_sprite_set(size_t i, pAnmFuncI_t f) {
    if (i >= m_on_sprite_set.size()) m_on_sprite_set.resize(i + 1, nullptr);
    m_on_sprite_set[i] = f;
}

void AnmFuncs::set_on_draw(size_t i, pAnmFunc_t f) {
    if (i >= m_on_draw.size()) m_on_draw.resize(i + 1, nullptr);
    m_on_draw[i] = f;
}

void AnmFuncs::set_on_tick(size_t i, pAnmFunc_t f) {
    if (i >= m_on_tick.size()) m_on_tick.resize(i + 1, nullptr);
    m_on_tick[i] = f;
}

void AnmFuncs::set_on_destroy(size_t i, pAnmFunc_t f) {
    if (i >= m_on_destroy.size()) m_on_destroy.resize(i + 1, nullptr);
    m_on_destroy[i] = f;
}

void AnmFuncs::set_on_wait(size_t i, pAnmFunc_t f) {
    if (i >= m_on_wait.size()) m_on_wait.resize(i + 1, nullptr);
    m_on_wait[i] = f;
}

void AnmFuncs::set_on_create(size_t i, pAnmFunc_t f) {
    if (i >= m_on_create.size()) m_on_create.resize(i + 1, nullptr);
    m_on_create[i] = f;
}
