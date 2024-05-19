#ifndef __INCLUDE_PARTS__
#include "../EclContext.h"
#include "../EclInstrMacros.h"
#include "../Enemy.h"
#include <math/Random.h>
#include <vector>
inline int Enemy::execInstr(EclRunContext_t* cont, const EclRawInstr_t* instr) {
    _insNop
#endif

        _ins(1, delete)
            cont->currentLocation.sub_id
        = -1;
    cont->currentLocation.offset = -1;
    return -1;

    _ins(10, return) eclPopContext(cont);
    // TODO(ClementChambard)
    _ret

        int bo
        = cont->stack.baseOffset;
    eclStackPop(&cont->stack, cont->stack.baseOffset, true);
    cont->stack.stackOffset = bo;
    if (cont->stack.stackOffset == 0) {
        cont->currentLocation.sub_id = -1;
        cont->currentLocation.offset = -1;
        return -1;
    }
    eclStackPop(&cont->stack, cont->currentLocation.sub_id, true);
    eclStackPop(&cont->stack, cont->currentLocation.offset, true);
    eclStackPop(&cont->stack, cont->time, true);
    eclStackPop(&cont->stack, cont->stack.stackOffset, true);

    _ins(11, call) _z(sub) cont->currentLocation.offset += instr->size;
    // Should be ok
    // if (EclRunContext::init_context_from_args(cont,cont,0)) {
    //    cont->currentLocation.sub_id = -1;
    //    cont->currentLocation.offset = -1;
    //    return -1;
    //}

    std::vector<EclStackItem_t> args_to_put_on_stack;

    for (int i = 1; i < instr->param_count; i++) {
        if (*reinterpret_cast<const char*>(__arg) == 'i') {
            __arg += 4;
            _S(x);
            args_to_put_on_stack.push_back(x);
        } else if (*reinterpret_cast<const char*>(__arg) == 'f') {
            __arg += 4;
            _f(x);
            args_to_put_on_stack.push_back(x);
        }
    } _args

    eclPushContext(cont);
    cont->currentLocation.sub_id = fileManager->getSubId(sub.c_str());
    int i = 0;
    for (auto e : args_to_put_on_stack)
        cont->stack.data[cont->stack.stackOffset + i++] = e;

    _ret;

    _ins(12, jump) _S(offset) _f(time) _args /* OK */
        cont->time
        += time;
    cont->currentLocation.offset += offset;
    _ret;

    _ins(13, jumpEq) _S(offset) _f(time) _args _popS(top); /* OK */
    if (!top) {
        cont->currentLocation.offset += offset;
        cont->time += time;
        _ret;
    }

    _ins(14, jumpNeq) _S(offset) _f(time) _args _popS(top); /* OK */
    if (top) {
        cont->currentLocation.offset += offset;
        cont->time += time;
        _ret;
    }

    _ins(15, callAsync) _z(sub)

        EclRunContext_t* c
        = new EclRunContext_t();
    c->currentLocation.sub_id = fileManager->getSubId(sub.c_str());
    c->difficultyMask = context.primaryContext.difficultyMask;
    EclRunContextList_t* l = asyncListHead;
    asyncListHead = new EclRunContextList_t();
    asyncListHead->next = l;
    if (asyncListHead->next)
        asyncListHead->next->prev = asyncListHead;
    asyncListHead->entry = c;

    for (int i = 1; i < instr->param_count; i++) {
        if (*reinterpret_cast<const char*>(__arg) == 'i') {
            __arg += 4;
            _S(x);
            c->stack.data[c->stack.stackOffset + i - 1].asInt = x;
            c->stack.data[c->stack.stackOffset + i - 1].asFloat = x;
        } else if (*reinterpret_cast<const char*>(__arg) == 'f') {
            __arg += 4;
            _f(x);
            c->stack.data[c->stack.stackOffset + i - 1].asInt = x;
            c->stack.data[c->stack.stackOffset + i - 1].asFloat = x;
        }
    }
    _args;

    _ins(16, callAsyncId) _z(sub) _S(id)

        EclRunContext_t* c
        = new EclRunContext_t();
    c->currentLocation.sub_id = fileManager->getSubId(sub.c_str());
    c->difficultyMask = context.primaryContext.difficultyMask;
    c->asyncId = id;
    EclRunContextList_t* l = asyncListHead;
    asyncListHead = new EclRunContextList_t();
    asyncListHead->next = l;
    if (asyncListHead->next)
        asyncListHead->next->prev = asyncListHead;
    asyncListHead->entry = c;

    for (int i = 1; i < instr->param_count; i++) {
        if (*reinterpret_cast<const char*>(__arg) == 'i') {
            __arg += 4;
            _S(x);
            c->stack.data[c->stack.stackOffset + i - 1].asInt = x;
            c->stack.data[c->stack.stackOffset + i - 1].asFloat = x;
        } else if (*reinterpret_cast<const char*>(__arg) == 'f') {
            __arg += 4;
            _f(x);
            c->stack.data[c->stack.stackOffset + i - 1].asInt = x;
            c->stack.data[c->stack.stackOffset + i - 1].asFloat = x;
        }
    }
    _args;

    /////// OK FROM HERE /////////
    _ins(17, killAsync) _S(id) _args
    for (EclRunContextList_t* node = asyncListHead;
         node != nullptr; node = node->next) {
        if (node->entry->asyncId == id) {
            node->entry->currentLocation.sub_id = -1;
            node->entry->currentLocation.offset = -1;
            break;
        }
    }

    _ins(18, unknown18) _S(id) _args
    for (EclRunContextList_t* node = asyncListHead;
         node != nullptr; node = node->next) {
        if (node->entry->asyncId == id) {
            node->entry->set_by_ins_18_19 |= 1;
            break;
        }
    }

    _ins(19, unknown19) _S(id) _args
    for (EclRunContextList_t* node = asyncListHead;
        node != nullptr; node = node->next) {
        if (node->entry->asyncId == id) {
            node->entry->set_by_ins_18_19 &= 0xfffffffe;
            break;
        }
    }

    _ins(20, unknown20) _S(id) _S(b) _args
    for (EclRunContextList_t* node = asyncListHead;
         node != nullptr; node = node->next) {
        if (node->entry->asyncId == id) {
            node->entry->set_by_ins_20 = b;
            break;
        }
    }

    _ins(21, killAllAsync)
    for (EclRunContextList_t* node = asyncListHead;
         node != nullptr; node = node->next) {
        node->entry->currentLocation.sub_id = -1;
        node->entry->currentLocation.offset = -1;
    }

    _ins(22, debug22) _S(a) _z(b) _args
    // NOPed in the code

    _ins(23, wait) _S(time) _args
    cont->time -= time;

    _ins(24, waitf) _f(time) _args
    cont->time -= time;

    _ins(30, printf) _z(str) _args
    // NOPed in the code
    
    _ins(31, unknown31)
    // NOPed in the code

    _ins(40, stackAlloc) _S(s) _args
    eclAllocVariables(cont, s);
    // TODO(ClementChambard) see in the function

    _ins(41, stackDlloc)
    cont->stack.stackOffset--;
    int bo = cont->stack.baseOffset;
    cont->stack.baseOffset = cont->stack.data[cont->stack.stackOffset].asInt;
    cont->stack.stackOffset = bo;

    /*     BASIC MATHS     */
    _ins(42, pushi) _S(num) _args /*******/ /*******/ _push(num);
    _ins(43, seti) _rS(var) _args _popS(a); /*******/
    var = a;
    _ins(44, pushf) _f(num) _args /*******/ /*******/ _push(num);
    _ins(45, setf) _rf(var) _args _popf(a); /*******/
    var = a;
    _ins(50, addi) _popS(a);
    _popS(b);
    _push(b + a);
    _ins(51, addf) _popf(a);
    _popf(b);
    _push(b + a);
    _ins(52, subi) _popS(a);
    _popS(b);
    _push(b - a);
    _ins(53, subf) _popf(a);
    _popf(b);
    _push(b - a);
    _ins(54, muli) _popS(a);
    _popS(b);
    _push(b * a);
    _ins(55, mulf) _popf(a);
    _popf(b);
    _push(b * a);
    _ins(56, divi) _popS(a);
    _popS(b);
    if (a != 0)
        _push(b / a)
    else
        _push(0);
    _ins(57, divf) _popf(a);
    _popf(b);
    _push(b / a);
    _ins(58, modi) _popS(a);
    _popS(b);
    if (a != 0)
        _push(b % a)
    else
        _push(0);
    _ins(59, equi) _popS(a);
    _popS(b);
    _push(b == a);
    _ins(60, equf) _popf(a);
    _popf(b);
    _push(b == a);
    _ins(61, neqi) _popS(a);
    _popS(b);
    _push(b != a);
    _ins(62, neqf) _popf(a);
    _popf(b);
    _push(b != a);
    _ins(63, lesi) _popS(a);
    _popS(b);
    _push(b < a);
    _ins(64, lesf) _popf(a);
    _popf(b);
    _push(b < a);
    _ins(65, leqi) _popS(a);
    _popS(b);
    _push(b <= a);
    _ins(66, leqf) _popf(a);
    _popf(b);
    _push(b <= a);
    _ins(67, grei) _popS(a);
    _popS(b);
    _push(b > a);
    _ins(68, gref) _popf(a);
    _popf(b);
    _push(b > a);
    _ins(69, geqi) _popS(a);
    _popS(b);
    _push(b >= a);
    _ins(70, geqf) _popf(a);
    _popf(b);
    _push(b >= a);
    _ins(71, noti) _popS(a); /*******/
    _push(!a);
    _ins(72, notf) _popf(a); /*******/
    _push(!a);
    _ins(73, or) _popS(a);
    _popS(b);
    _push(b || a);
    _ins(74, and) _popS(a);
    _popS(b);
    _push(b && a);
    _ins(75, xor) _popS(a);
    _popS(b);
    _push(b ^ a);
    _ins(76, bor) _popS(a);
    _popS(b);
    _push(b | a);
    _ins(77, band) _popS(a);
    _popS(b);
    _push(b & a);
    _ins(78, deci) _rS(v) _args _push(v); /*******/
    v--;
    _ins(79, ssin) _popf(a); /*******/
    _push(sin(a));
    _ins(80, scos) _popf(a); /*******/
    _push(cos(a));
    _ins(83, negi) _popS(a); /*******/
    _push(-a);
    _ins(84, negf) _popf(a); /*******/
    _push(-a);
    _ins(88, sqrt) _popf(a); /*******/
    _push(sqrt(a));

    /*      ADVANCED MATH      */
    _ins(81, circlePos) _rf(vx) _rf(vy) _f(an) _f(ra) _args
        vx
        = ra * cos(an);
    vy = ra * sin(an);
    _ins(82, validRad) _rf(var) _args
        math::angle_normalize(var);
    _ins(85, sqSum) _rf(v) _f(x) _f(y) _args
        v
        = math::point_distance_sq(0, 0, x, y);
    _ins(86, sqSumRt) _rf(v) _f(x) _f(y) _args
        v
        = math::point_distance(0, 0, x, y);
    _ins(87, getAng) _rf(v) _f(x1) _f(y1) _f(x2) _f(y2) _args
        v
        = math::point_direction(x1, y1, x2, y2);
    _ins(89, linFunc) _rf(v) _f(a) _f(x) _args
    if (x - a <= PI) {
        if (PI < a - x)
            v = x - a + PI2;
        else
            v = x - a;
    } else {
        v = x - a - PI2;
    }
    _ins(90, ptRot) _rf(vx) _rf(vy) _f(x) _f(y) _f(a) _args
    vx = x * cos(a) - y * sin(a);
    vy = y * cos(a) + x * sin(a);
    _ins(91, floatTime) _S(slot) _rf(var) _S(t) _S(m) _f(i) _f(f) _args
    cont->float_interps_locs[slot] = __var_var;
    cont->float_i[slot].start(i, f, t, m);
    var = i;
    _ins(92, floatTimeEx) _S(slot) _rf(var) _S(t) _S(m) _f(i) _f(f)
                          _f(a) _f(b) _args
    cont->float_interps_locs[slot] = __var_var;
    cont->float_i[slot].start_ex(i, f, a, b, t, m);
    var = i;
    _ins(93, RandRadius) _rf(vx) _rf(vy) _f(r1) _f(r2) _args
    r1 += Random::Float01() * (r2 - r1);
    float a = Random::Angle();
    vx = r1 * cos(a);
    vy = r1 * sin(a);

#ifndef __INCLUDE_PARTS__
    _insDefault return 0;
}
#endif
