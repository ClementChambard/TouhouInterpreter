#ifndef ECLCONTEXT_H_
#define ECLCONTEXT_H_

#include "./Interp.h"
#include <array>
#include <iostream>

class Enemy;
struct EclRunContext_t;

struct EclLocation_t {
    int32_t sub_id = 0;
    int32_t offset = 0;
};

struct EclStackItem_t {
    float asFloat = 0;
    int32_t asInt = 0;
    EclStackItem_t(int32_t i)
        : asFloat(i)
        , asInt(i) {}
    EclStackItem_t(float f)
        : asFloat(f)
        , asInt(f) {}
    EclStackItem_t()
        : asFloat(0)
        , asInt(0) {}
};

struct EclStack_t {
    std::array<EclStackItem_t, 200> data = {};
    int32_t stackOffset = 0;
    int32_t baseOffset = 0;
};

struct EclRunContextList_t {
    EclRunContext_t* entry = nullptr;
    EclRunContextList_t* next = nullptr;
    EclRunContextList_t* prev = nullptr;
};

struct EclRunContext_t {
    float time = 0.f;
    EclLocation_t currentLocation = {};
    EclStack_t stack = {};
    int32_t asyncId = -999999;
    Enemy* enemy = nullptr;
    int32_t set_by_ins_20 = 0;
    uint8_t difficultyMask = 255;
    int32_t variable_stack_offset[8];
    int32_t int_vars[8];
    float float_vars[8];
    Interp<float> float_i[8] {}; // XXX
    float float_interps_locs[8] {}; // XXX
    // float_i
    // float_i_locs
    int32_t set_by_ins_18_19 = 0;
};

struct EclRunContextHolder_t {
    EclRunContext_t* currentContext = nullptr;
    EclRunContext_t primaryContext = {};
};

void eclAllocVariables(EclRunContext_t* cont, int32_t nb);
void eclStackPush(EclStack_t* stack, int32_t val);
void eclStackPush(EclStack_t* stack, float val);
void eclStackPop(EclStack_t* stack, int32_t& val, bool f = false);
void eclStackPop(EclStack_t* stack, float& val, bool f = false);
void eclPushContext(EclRunContext_t* ctx);
void eclPopContext(EclRunContext_t* ctx);
void eclContextInit(EclRunContext_t* ctx, int32_t sub);

void drawStackFrame(EclStack_t* stack);

#endif // ECLCONTEXT_H_
