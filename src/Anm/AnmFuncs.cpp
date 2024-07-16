#include "./AnmFuncs.h"

namespace anm {

std::vector<pVMFuncI_t> Funcs::m_on_switch;
std::vector<pVMFuncI_t> Funcs::m_on_sprite_set;
std::vector<pVMFunc_t> Funcs::m_on_draw;
std::vector<pVMFunc_t> Funcs::m_on_tick;
std::vector<pVMFunc_t> Funcs::m_on_destroy;
std::vector<pVMFunc_t> Funcs::m_on_wait;
std::vector<pVMFunc_t> Funcs::m_on_create;

pVMFuncI_t Funcs::on_switch(size_t i) {
    if (i >= m_on_switch.size()) return nullptr;
    return m_on_switch[i];
}

pVMFuncI_t Funcs::on_sprite_set(size_t i) {
    if (i >= m_on_sprite_set.size()) return nullptr;
    return m_on_sprite_set[i];
}

pVMFunc_t Funcs::on_draw(size_t i) {
    if (i >= m_on_draw.size()) return nullptr;
    return m_on_draw[i];
}

pVMFunc_t Funcs::on_tick(size_t i) {
    if (i >= m_on_tick.size()) return nullptr;
    return m_on_tick[i];
}

pVMFunc_t Funcs::on_destroy(size_t i) {
    if (i >= m_on_destroy.size()) return nullptr;
    return m_on_destroy[i];
}

pVMFunc_t Funcs::on_wait(size_t i) {
    if (i >= m_on_wait.size()) return nullptr;
    return m_on_wait[i];
}

pVMFunc_t Funcs::on_create(size_t i) {
    if (i >= m_on_create.size()) return nullptr;
    return m_on_create[i];
}

void Funcs::set_on_switch(size_t i, pVMFuncI_t f) {
    if (i >= m_on_switch.size()) m_on_switch.resize(i + 1, nullptr);
    m_on_switch[i] = f;
}

void Funcs::set_on_sprite_set(size_t i, pVMFuncI_t f) {
    if (i >= m_on_sprite_set.size()) m_on_sprite_set.resize(i + 1, nullptr);
    m_on_sprite_set[i] = f;
}

void Funcs::set_on_draw(size_t i, pVMFunc_t f) {
    if (i >= m_on_draw.size()) m_on_draw.resize(i + 1, nullptr);
    m_on_draw[i] = f;
}

void Funcs::set_on_tick(size_t i, pVMFunc_t f) {
    if (i >= m_on_tick.size()) m_on_tick.resize(i + 1, nullptr);
    m_on_tick[i] = f;
}

void Funcs::set_on_destroy(size_t i, pVMFunc_t f) {
    if (i >= m_on_destroy.size()) m_on_destroy.resize(i + 1, nullptr);
    m_on_destroy[i] = f;
}

void Funcs::set_on_wait(size_t i, pVMFunc_t f) {
    if (i >= m_on_wait.size()) m_on_wait.resize(i + 1, nullptr);
    m_on_wait[i] = f;
}

void Funcs::set_on_create(size_t i, pVMFunc_t f) {
    if (i >= m_on_create.size()) m_on_create.resize(i + 1, nullptr);
    m_on_create[i] = f;
}

} // namespace anm
