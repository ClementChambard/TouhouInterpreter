#include "./EclContext.h"
#include "./EclFileManager.h"
#include "EclFileManager.h"
#include <logger.h>
#include <cstdio>

void eclStackPush(EclStack_t* stack, int32_t val) {
    if (stack->stackOffset >= 199) {
        NS_FATAL("Stack overflow");
        drawStackFrame(stack);
        exit(1);
    }
    stack->data[stack->stackOffset].asInt = val;
    stack->data[stack->stackOffset].asFloat = static_cast<float>(val);
    stack->stackOffset++;
}

void eclStackPush(EclStack_t* stack, float val) {
    if (stack->stackOffset >= 199) {
        NS_FATAL("Stack overflow");
        drawStackFrame(stack);
        exit(1);
    }
    stack->data[stack->stackOffset].asFloat = val;
    stack->data[stack->stackOffset].asInt = static_cast<int>(val);
    stack->stackOffset++;
}

void eclStackPop(EclStack_t* stack, int32_t& val, bool f) {
    if (!f && stack->stackOffset <= stack->baseOffset) {
        NS_ERROR("Pop int: no value in stack, returning 0");
        val = 0; return;
    }
    stack->stackOffset--;
    val = stack->data[stack->stackOffset].asInt;
}
void eclStackPop(EclStack_t* stack, float& val, bool f) {
    if (!f && stack->stackOffset <= stack->baseOffset) {
        NS_ERROR("Pop float: no value in stack, returning 0");
        val = 0; return;
    }
    stack->stackOffset--;
    val = stack->data[stack->stackOffset].asFloat;
}

void eclAllocVariables(EclRunContext_t *cont, int32_t nb) {
    // if (cont->stack.stackOffset + nb/4 < 200) {
        // int stackOffset = cont->stack.stackOffset;
        // cont->stack.stackOffset += nb/4;
        // cont->stack.data[cont->stack.stackOffset] = cont->stack.baseOffset;
        // cont->stack.stackOffset ++;
        // cont->stack.baseOffset = stackOffset;
    // } // TODO: use this
    // for (int i = 0; i < nb/4; i++)
    // {
        // cont->variable_stack_offset[i] = cont->stack.baseOffset + i;
    // }
    cont->stack.stackOffset = cont->stack.baseOffset + nb/4;
}

void eclPushContext(EclRunContext_t* ctx) {
    /* Push every context variable */
    eclStackPush(&ctx->stack, ctx->stack.baseOffset);
    eclStackPush(&ctx->stack, ctx->time);
    eclStackPush(&ctx->stack, ctx->currentLocation.offset);
    eclStackPush(&ctx->stack, ctx->currentLocation.sub_id);
    // for (int i = 0; i < 8; i++)
    //     eclStackPush(&ctx->stack, ctx->variable_stack_offset[i]);

    /* Move stack offset */
    ctx->stack.baseOffset = ctx->stack.stackOffset;

    /* Set values to default */
    ctx->currentLocation.offset = 0;
    ctx->currentLocation.sub_id = -1;
    ctx->time = 0.f;
}

void eclPopContext(EclRunContext_t* ctx) {
    /* If there is nothing to pop, disable the context */
    if (ctx->stack.baseOffset == 0) {
        ctx->currentLocation.sub_id = -1;
        return;
    }

    /* Discard every items in scope */
    ctx->stack.stackOffset = ctx->stack.baseOffset;

    /* Pop every context variable */
    // for (int i = 7; i >= 0; i--)
    //     eclStackPop(&ctx->stack, ctx->variable_stack_offset[i], true);
    eclStackPop(&ctx->stack, ctx->currentLocation.sub_id, true);
    eclStackPop(&ctx->stack, ctx->currentLocation.offset, true);
    eclStackPop(&ctx->stack, ctx->time, true);
    eclStackPop(&ctx->stack, ctx->stack.baseOffset, true);
}

void eclContextInit(EclRunContext_t *ctx, int32_t sub) {
    ctx->stack.stackOffset = 0;
    ctx->stack.baseOffset = 0;
    ctx->currentLocation.offset = 0;
    ctx->currentLocation.sub_id = sub;
    ctx->time = 0;
}

static void print_stack_trace_rec(EclStack_t* st, int bp) {
    if (bp == 0) {
        NS_TRACE("bottom of call stack");
        return;
    }

    // bp-1: sub_id
    int sub_id = st->data[bp-1].asInt;
    // bp-2: offset in sub
    int offset = st->data[bp-2].asInt;
    // bp-3: time in sub
    int time = st->data[bp-3].asInt;
    // bp-4: old bp
    int oldBp = st->data[bp-4].asInt;

    auto name = EclFileManager::GetInstance()->getSubName(sub_id);

    NS_TRACE("%d@%d in sub %s", offset, time, name);

    print_stack_trace_rec(st, oldBp);
}

void print_stack_trace(EclStack_t* stack) {
    NS_TRACE("Stack trace:");

    if (stack->baseOffset != 0)
        print_stack_trace_rec(stack, stack->baseOffset);
    else
        NS_TRACE("...");
}

void drawStackFrame(EclStack_t* stack) {
    int start = 0;
    if (stack->baseOffset > 4) start = stack->baseOffset-4;
    int end = start + 15;
    if (end >= 200) end = 199;
    for (int i = end; i >= start; i--) {
        if (i == stack->baseOffset) {
            NS_TRACE("%3d: base  --> %8d %4.4f", i, stack->data[i].asInt, stack->data[i].asFloat);
        } else if (i == stack->stackOffset) {
            NS_TRACE("%3d: top   --> %8d %4.4f", i, stack->data[i].asInt, stack->data[i].asFloat);
        } else {
            NS_TRACE("%3d:           %8d %4.4f\n", i, stack->data[i].asInt, stack->data[i].asFloat);
        }
    }
    NS_TRACE("");
    print_stack_trace(stack);
}
