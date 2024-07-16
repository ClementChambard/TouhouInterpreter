#ifndef ECLCONTEXT_H_
#define ECLCONTEXT_H_

#include <Interp.hpp>
#include <array>

class Enemy;
struct EclRunContext_t;

struct EclLocation_t {
    i32 sub_id = 0;
    i32 offset = 0;
};

struct EclStackItem_t {
    f32 asFloat = 0;
    i32 asInt = 0;
    EclStackItem_t(i32 i)
        : asFloat(i)
        , asInt(i) {}
    EclStackItem_t(f32 f)
        : asFloat(f)
        , asInt(f) {}
    EclStackItem_t()
        : asFloat(0)
        , asInt(0) {}
};

struct EclStack_t {
    std::array<EclStackItem_t, 200> data = {};
    i32 stackOffset = 0;
    i32 baseOffset = 0;
};

struct EclRunContextList_t {
    EclRunContext_t* entry = nullptr;
    EclRunContextList_t* next = nullptr;
    EclRunContextList_t* prev = nullptr;
};

struct EclRunContext_t {
    f32 time = 0.f;
    EclLocation_t currentLocation = {};
    EclStack_t stack = {};
    i32 asyncId = -999999;
    Enemy* enemy = nullptr;
    i32 set_by_ins_20 = 0;
    u8 difficultyMask = 255;
    i32 variable_stack_offset[8];
    i32 int_vars[8];
    f32 float_vars[8];
    ns::Interp<f32> float_i[8] {}; // XXX
    f32 float_interps_locs[8] {}; // XXX
    // f32_i
    // f32_i_locs
    i32 set_by_ins_18_19 = 0;
};

struct EclRunContextHolder_t {
    EclRunContext_t* currentContext = nullptr;
    EclRunContext_t primaryContext = {};
};

void eclAllocVariables(EclRunContext_t* cont, i32 nb);
void eclStackPush(EclStack_t* stack, i32 val);
void eclStackPush(EclStack_t* stack, f32 val);
void eclStackPop(EclStack_t* stack, i32& val, bool f = false);
void eclStackPop(EclStack_t* stack, f32& val, bool f = false);
void eclPushContext(EclRunContext_t* ctx);
void eclPopContext(EclRunContext_t* ctx);
void eclContextInit(EclRunContext_t* ctx, i32 sub);

void drawStackFrame(EclStack_t* stack);

#endif // ECLCONTEXT_H_
