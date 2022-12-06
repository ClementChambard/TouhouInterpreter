#include "EclContext.h"
#include <Error.h>

void eclStackPush(EclStack_t* stack, int32_t val)
{
    if (stack->stackOffset >= 199) { NSEngine::pr("\n\e[31mFATAL ERROR:\e[0m", "Stack overflow", "\n"); NSEngine::FatalErrorQuit(); }
    stack->data[stack->stackOffset].asInt = val;
    stack->data[stack->stackOffset].asFloat = (float)val;
    stack->stackOffset++;
}

void eclStackPush(EclStack_t* stack, float val)
{
    if (stack->stackOffset >= 199) { NSEngine::pr("\n\e[31mFATAL ERROR:\e[0m", "Stack overflow", "\n"); NSEngine::FatalErrorQuit(); }
    stack->data[stack->stackOffset].asFloat = val;
    stack->data[stack->stackOffset].asInt = (int)val;
    stack->stackOffset++;
}

void eclStackPop(EclStack_t* stack, int32_t& val, bool f)
{
    if (!f && stack->stackOffset <= stack->baseOffset)
    {
        std::cerr << "Pop int: no value in stack, returning 0\n";
        val = 0; return;
    }
    stack->stackOffset--;
    val = stack->data[stack->stackOffset].asInt;
}
void eclStackPop(EclStack_t* stack, float& val, bool f)
{
    if (!f && stack->stackOffset <= stack->baseOffset)
    {
        std::cerr << "Pop float: no value in stack, returning 0\n";
        val = 0; return;
    }
    stack->stackOffset--;
    val = stack->data[stack->stackOffset].asFloat;
}

void eclAllocVariables(EclRunContext_t *cont, int32_t nb)
{
    for (int i = 0; i < nb/4; i++)
    {
        cont->variable_stack_offset[i] = cont->stack.baseOffset + i;
    }
    cont->stack.stackOffset = cont->stack.baseOffset + nb/4;
}

void eclPushContext(EclRunContext_t* ctx)
{
    /* Push every context variable */
    eclStackPush(&ctx->stack, ctx->stack.baseOffset);
    eclStackPush(&ctx->stack, ctx->time);
    eclStackPush(&ctx->stack, ctx->currentLocation.offset);
    eclStackPush(&ctx->stack, ctx->currentLocation.sub_id);
    for (int i = 0; i < 8; i++) eclStackPush(&ctx->stack, ctx->variable_stack_offset[i]);

    /* Move stack offset */
    ctx->stack.baseOffset = ctx->stack.stackOffset;

    /* Set values to default */
    ctx->currentLocation.offset = 0;
    ctx->currentLocation.sub_id = -1;
    ctx->time = 0.f;
}

void eclPopContext(EclRunContext_t* ctx)
{
    /* If there is nothing to pop, disable the context */
    if (ctx->stack.baseOffset == 0)
    {
        ctx->currentLocation.sub_id = -1;
        return;
    }

    /* Discard every items in scope */
    ctx->stack.stackOffset = ctx->stack.baseOffset;

    /* Pop every context variable */
    for (int i = 7; i >= 0; i--) eclStackPop(&ctx->stack, ctx->variable_stack_offset[i], true);
    eclStackPop(&ctx->stack, ctx->currentLocation.sub_id, true);
    eclStackPop(&ctx->stack, ctx->currentLocation.offset, true);
    eclStackPop(&ctx->stack, ctx->time, true);
    eclStackPop(&ctx->stack, ctx->stack.baseOffset, true);
}

void eclContextInit(EclRunContext_t *ctx, int32_t sub)
{
    ctx->stack.stackOffset = 0;
    ctx->stack.baseOffset = 0;
    ctx->currentLocation.offset = 0;
    ctx->currentLocation.sub_id = sub;
    ctx->time = 0;
}
