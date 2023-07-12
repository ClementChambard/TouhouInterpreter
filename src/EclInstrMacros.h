#ifndef ECLINSTRMACROS_H_
#define ECLINSTRMACROS_H_

#include <string>

#define __doprint PRINT && !__noprint

#define _insNop                                                                \
    unsigned char* __arg = const_cast<unsigned char*>(instr->data);            \
    int __argmask = 1;                                                         \
    int __narg = 0;                                                            \
    bool __noprint = false;                                                    \
    if ((instr->rank_mask & 63 & cont->difficultyMask)) {                      \
        switch (instr->id) {                                                   \
        case 0: {                                                              \
            std::string __name = "nop";                                        \
            [[maybe_unused]] int __id = 0;

#define _ins(id, name)                                                         \
    if (__doprint)                                                             \
        std::cout << ")\n";                                                    \
    break;                                                                     \
    }                                                                          \
    case id: {                                                                 \
        [[maybe_unused]] int __id = id;                                        \
        std::string __name = #name;                                            \
        if (__doprint) {                                                       \
            std::cout << "\e[90m"                                              \
                << fileManager->getSubName(cont->currentLocation.sub_id)       \
                << ":  \e[0m";                                                 \
            if (instr->param_count == 0)                                       \
                std::cout << __name << "(";                                    \
        }

#define _insDefault                                                            \
    if (__doprint)                                                             \
        std::cout << ")\n";                                                    \
    std::cout << "\e[92m ERROR: undefined instruction opcode "                 \
              << instr->id << " \e[0m\n";                                      \
    break;                                                                     \
    }                                                                          \
    default: {                                                                 \
    }                                                                          \
        }                                                                      \
        /*std::cout << "exec ins_" << instr->id << "\n";*/                     \
        cont->stack.stackOffset -= stackToRemove;                              \
        stackToRemove = 0;                                                     \
        }                                                                      \
        cont->currentLocation.offset += instr->size;

#define _popS(x)                                                               \
    int32_t x;                                                                 \
    eclStackPop(&cont->stack, x);

#define _popf(x)                                                               \
    float x;                                                                   \
    eclStackPop(&cont->stack, x);

#define _push(x)                                                               \
    eclStackPush(&cont->stack, x);

#define _S(x)                                                                  \
    int32_t x = *reinterpret_cast<int32_t*>(__arg);                            \
    int32_t __##x##_var = 0;                                                   \
    bool __##x##_isvar = instr->param_mask & __argmask;                        \
    if (__##x##_isvar) {                                                       \
        __##x##_var = x;                                                       \
        x = checkVarI(x);                                                      \
    }                                                                          \
    __arg += 4;                                                                \
    __argmask *= 2;                                                            \
    if (__doprint) {                                                           \
        if (__narg > 0)                                                        \
            std::cout << ", ";                                                 \
        else                                                                   \
            std::cout << __name << "(";                                        \
        if (__##x##_isvar)                                                     \
            std::cout << "[" << __##x##_var << "] : ";                         \
        std::cout << x;                                                        \
    }                                                                          \
    __narg++;

#define _f(x)                                                                  \
    float x = *reinterpret_cast<float*>(__arg);                                \
    float __##x##_var = 0;                                                     \
    bool __##x##_isvar = instr->param_mask & __argmask;                        \
    if (__##x##_isvar) {                                                       \
        __##x##_var = x;                                                       \
        x = checkVarF(x);                                                      \
    }                                                                          \
    __arg += 4;                                                                \
    __argmask *= 2;                                                            \
    if (__doprint) {                                                           \
        if (__narg > 0)                                                        \
            std::cout << ", ";                                                 \
        else                                                                   \
            std::cout << __name << "(";                                        \
        if (__##x##_isvar)                                                     \
            std::cout << "[" << __##x##_var << "] : ";                         \
        std::cout << x;                                                        \
    }                                                                          \
    __narg++;

#define _rS(x)                                                                 \
    const int32_t __##x##_var = *reinterpret_cast<const int32_t*>(__arg);      \
    int32_t& x = checkVarRI(__##x##_var);                                      \
    __arg += 4;                                                                \
    __argmask *= 2;                                                            \
    if (__doprint) {                                                           \
        if (__narg > 0)                                                        \
            std::cout << ", ";                                                 \
        else                                                                   \
            std::cout << __name << "(";                                        \
        std::cout << "[" << __##x##_var << "]";                                \
    }                                                                          \
    __narg++;

#define _rf(x)                                                                 \
    float __##x##_var = *reinterpret_cast<float*>(__arg);                      \
    float& x = checkVarRF(__##x##_var);                                        \
    __arg += 4;                                                                \
    __argmask *= 2;                                                            \
    if (__doprint) {                                                           \
        if (__narg > 0)                                                        \
            std::cout << ", ";                                                 \
        else                                                                   \
            std::cout << __name << "(";                                        \
        std::cout << "[" << __##x##_var << "]";                                \
    }                                                                          \
    __narg++;

#define _z(x)                                                                  \
    int32_t __##x##_size = *reinterpret_cast<int32_t*>(__arg);                 \
    __arg += 4;                                                                \
    std::string x = "";                                                        \
    for (int i = 0; i < __##x##_size; i++)                                     \
        if (__arg[i] != 0)                                                     \
            x += __arg[i];                                                     \
    __arg += __##x##_size;                                                     \
    __argmask *= 2;                                                            \
    if (__doprint) {                                                           \
        if (__narg > 0)                                                        \
            std::cout << ", ";                                                 \
        else                                                                   \
            std::cout << __name << "(";                                        \
        std::cout << x;                                                        \
    }                                                                          \
    __narg++;

#define _args                                                                  \
    cont->stack.stackOffset -= stackToRemove;                                  \
    stackToRemove = 0;

#define I_(n) enemy.ecl_int_vars[n]
#define f_(n) enemy.ecl_float_vars[n]

#define _noprint __noprint = true;

#define _notImpl                                                               \
    std::cout << "\e[33m/!\\ " << __name                                       \
              << " (ins_" << __id << ") is not implemented\e[0m\n";            \
    __noprint = true;

#define _alert std::cout << "\e[34m/!\\ alert ---> " << __name                 \
                         << " (ins_" << __id << ") has been used\e[0m\n";

#define _warn(m) std::cout << "\e[32m/!\\ warning at " << __name               \
                           << " (ins_" << __id << ") : \e[0m" << m << "\n";

#define _ret                                                                   \
    if (__doprint)                                                             \
        std::cout << ")\n";                                                    \
    return 0;

#define INT_ NSEngine::InterpolateManager::Interpolate

#define isBoss_ (EnemyManager::GetInstance()->boss_ids[0] > 0 &&          \
                 EnemyManager::GetInstance()->                                 \
                     EnmFind(EnemyManager::GetInstance()->boss_ids[0]))

#define BOSS_ EnemyManager::GetInstance()->                                    \
                EnmFind(EnemyManager::GetInstance()->boss_ids[0])         \

#endif // ECLINSTRMACROS_H_
