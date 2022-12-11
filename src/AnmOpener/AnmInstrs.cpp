#include "AnmBitflags.h"
#include "AnmVM.h"
#include <math/Random.h>
#include "AnmManagerN.h"

void AnmVM::exec_instruction(int8_t* ins)
{
    float tempvar1 = 0.f;
    float tempvar2 = 0.f;
    AnmVMList* tempNode = nullptr;
    uint16_t type = *reinterpret_cast<uint16_t*>(ins);
    #define argS(x) *reinterpret_cast<int32_t*>(&(ins[8+4*x]))
    #define argf(x) *reinterpret_cast<float*>(&(ins[8+4*x]))
    #define  S(x) check_val(argS(x))
    #define u8S(x) static_cast<uint8_t>(S(x))
    #define  f(x) check_val(argf(x))
    #define rS(x) check_ref(argS(x))
    #define rf(x) check_ref(argf(x))
    #define jump(n, t) {current_instr = n; time = t;}
    #define createChild(id, u, l)     \
                    tempNode = new AnmVMList();     \
                    tempNode->value = AnmManagerN::getVM(AnmManagerN::SpawnVM(anim_slot, id, u, l));  \
                    tempNode->value->parent = this;                         \
                    tempNode->value->nodeAsChildren = tempNode
    switch (type)
    {

        case   0: // nop
            return;
        case   1: // destroy
            bitflags.activeFlags = ANMVM_DELETE;
            return;
        case   2: // freeze
            bitflags.activeFlags = ANMVM_FROZEN;
            return;
        case   3: // stop
            if (time >= -10000) time = -10000-time;
            return;
        case   4: // stophide
            bitflags.visible = 0;
            if (time >= -10000) time = -10000-time;
            return;
        case   5: // interruptLabel
            // do nothing (just a label)
            return;
        case   6: // wait
            time -= S(0);
            return;
        case   7: // caseReturn
            time = case_return_time;
            current_instr = return_instr;
            return;

        case 100: // iset
            rS(0)  = S(1);
            return;
        case 101: // fset
            rf(0)  = f(1);
            return;
        case 102: // iadd
            rS(0) += S(1);
            return;
        case 103: // fadd
            rf(0) += f(1);
            return;
        case 104: // isub
            rS(0) -= S(1);
            return;
        case 105: // fsub
            rf(0) -= f(1);
            return;
        case 106: // imul
            rS(0) *= S(1);
            return;
        case 107: // fmul
            rf(0) *= f(1);
            return;
        case 108: // idiv
            rS(0) /= S(1);
            return;
        case 109: // fdiv
            rf(0) /= f(1);
            return;
        case 110: // imod
            rS(0) %= S(1);
            return;
        case 111: // fmod
            rf(0) = fmod(f(0), f(1));
            return;
        case 112: // isetadd
            rS(0) = S(1) + S(2);
            return;
        case 113: // fsetadd
            rf(0) = f(1) + f(2);
            return;
        case 114: // isetsub
            rS(0) = S(1) - S(2);
            return;
        case 115: // fsetsub
            rf(0) = f(1) - f(2);
            return;
        case 116: // isetmul
            rS(0) = S(1) * S(2);
            return;
        case 117: // fsetmul
            rf(0) = f(1) * f(2);
            return;
        case 118: // isetdiv
            rS(0) = S(1) / S(2);
            return;
        case 119: // fsetdiv
            rf(0) = f(1) / f(2);
            return;
        case 120: // isetmod
            rS(0) = S(1) % S(2);
            return;
        case 121: // fsetmod
            rf(0) = fmod(f(1), f(2));
            return;
        case 122: // isetrand
            rS(0) = Random::Float01() * S(1);
            return;
        case 123: // fsetrand
            rf(0) = Random::Float01() * f(1);
            return;
        case 124: // fsin
            rf(0) = sin(f(1));
            return;
        case 125: // fcos
            rf(0) = cos(f(1));
            return;
        case 126: // ftan
            rf(0) = tan(f(1));
            return;
        case 127: // facos
            rf(0) = acos(f(1));
            return;
        case 128: // fatan
            rf(0) = atan(f(1));
            return;
        case 129: // validAngle
            while (f(0) > PI) rf(0) -= PI2;
            while (f(0) < -PI) rf(0) += PI2;
            return;
        case 130: // circlePos
            rf(0) = f(3) * cos(f(2));
            rf(1) = f(3) * sin(f(2));
            return;
        case 131: // circlePosRand
            tempvar1 = f(2) + (f(3)-f(2)) * Random::Float01();
            tempvar2 = Random::Angle();
            rf(0) = tempvar1 * cos(tempvar2);
            rf(1) = tempvar1 * sin(tempvar2);
            return;

        case 200: // jmp
            jump(S(0), S(1))
            return;
        case 201: // jmpDec
            if (rS(0)-- > 0) jump(S(1), S(2))
            return;
        case 202: // ije
            if (S(0) == S(1)) jump(S(2), S(3))
            return;
        case 203: // fje
            if (f(0) == f(1)) jump(S(2), S(3))
            return;
        case 204: // ijne
            if (S(0) != S(1)) jump(S(2), S(3))
            return;
        case 205: // fjne
            if (f(0) != f(1)) jump(S(2), S(3))
            return;
        case 206: // ijl
            if (S(0) <  S(1)) jump(S(2), S(3))
            return;
        case 207: // fjl
            if (f(0) <  f(1)) jump(S(2), S(3))
            return;
        case 208: // ijle
            if (S(0) <= S(1)) jump(S(2), S(3))
            return;
        case 209: // fjle
            if (f(0) <= f(1)) jump(S(2), S(3))
            return;
        case 210: // ijg
            if (S(0) >  S(1)) jump(S(2), S(3))
            return;
        case 211: // fjg
            if (f(0) >  f(1)) jump(S(2), S(3))
            return;
        case 212: // ijge
            if (S(0) >= S(1)) jump(S(2), S(3))
            return;
        case 213: // fjge
            if (f(0) >= f(1)) jump(S(2), S(3))
            return;

        case 300: // sprite
            sprite_id = on_set_sprite(this, S(0));
            bitflags.visible = 1;
            return;
        case 301: // spriteRand
            tempvar1 = S(0) + (S(1)-S(0)) * Random::Float01();
            sprite_id = static_cast<uint32_t>(tempvar1);
            return;
        case 302: // type
            bitflags.rendermode = S(0);
            return;
        case 303: // blendmode
            bitflags.blendmode = S(0);
            return;
        case 304: // layer
            layer = S(0);
            return;
        case 305: // zWriteDisable
            bitflags.zwritedis = S(0);
            return;
        case 306: // ins_306 : std related bitflag
            bitflags.f530_15 = S(0);
            return;
        case 307: // randMode
            // do nothing : only one type of random
            return;
        case 308: // flipX
            scale.x *= -1;
            bitflags.flip_x = !bitflags.flip_x;
            return;
        case 309: // flipY
            scale.y *= -1;
            bitflags.flip_y = !bitflags.flip_y;
            return;
        case 310: // visible
            bitflags.visible = S(0);
            return;
        case 311: // resampleMode
            bitflags.resolutionMode = S(0);
            return;
        case 312: // scrollMode
            bitflags.scrollX = S(0);
            bitflags.scrollY = S(1);
            return;
        case 313: // resolutionMode
            bitflags.resolutionMode = S(0);
            return;
        case 314: // ins_314
            bitflags.parRotate = S(0);
            return;
        case 315: // colorizeChildrens
            bitflags.colorizeChildren = S(0);
            return;
        case 316: // ins_316
            bitflags.f530_1 = 1;
            return;
        case 317: // ins_317
            bitflags.f530_1 = 0;
            return;

        case 400: // pos
            pos = { f(0), f(1), f(2) };
            return;
        case 401: // rotate
            rotation = { f(0), f(1), f(2) };
            return;
        case 402: // scale
            scale = { f(0), f(1) };
            return;
        case 403: // alpha
            color1.a = u8S(0);
            return;
        case 404: // color
            color1 = { u8S(0), u8S(1), u8S(2), color1.a };
            return;
        case 405: // alpha2.current
            color2.a = u8S(0);
            return;
        case 406: // color2
            color2 = { u8S(0), u8S(1), u8S(2), color2.a };
            return;
        case 407: // posTime
            pos_i.start(pos, { f(2), f(3), f(4) }, S(0), S(1));
            return;
        case 408: // colorTime
            rgb1_i.start({color1.r, color1.g, color1.b}, { S(2), S(3), S(4) }, S(0), S(1));
            return;
        case 409: // alphaTime
            alpha1_i.start(color1.a, S(2), S(0), S(1));
            return;
        case 410: // rotateTime
            rotate_i.start(rotation, { f(2), f(3), f(4) }, S(0), S(1));
            return;
        case 411: // rotateTime2D
            rotate_2d_i.start(rotation.z, f(2), S(0), S(1));
            return;
        case 412: // scaleTime
            scale_i.start(scale, { f(2), f(3) }, S(0), S(1));
            return;
        case 413: // color2Time
            rgb2_i.start({color2.r, color2.g, color2.b}, { S(2), S(3), S(4) }, S(0), S(1));
            return;
        case 414: // alpha2.currentTime
            alpha2_i.start(color2.a, S(2), S(0), S(1));
            return;
        case 415: // rotVel
            angular_velocity = { f(0), f(1), f(2) };
            return;
        case 416: // scaleGrowth
            scale_growth = { f(0), f(1) };
            return;
        case 417: // alphaTimeLinear
            alpha1_i.start(color1.a, S(1), S(0), 0);
            return;
        case 418: // TODO: ins_418
            // get some uvs from position
            return;
        case 419: // ins_419
            bitflags.ins419 = S(0);
            return;
        case 420: // moveBezier
            pos_i.start_bezier(pos, {f(4), f(5), f(6)}, {f(1), f(2), f(3)}, {f(7), f(8), f(9)}, S(0));
            return;
        case 421: // anchor
            //std::cout << "anchor : " << S(0) << "\n";
            bitflags.anchorX = (S(0) & 3);
            bitflags.anchorY = ((S(0) >> 16) & 3);
            //std::cout << " --> " << bitflags.anchorX << " " << bitflags.anchorY << "\n";
            return;
        case 422: // ins_422
            pos = entity_pos;
            entity_pos = {0, 0, 0};
            return;
        case 423: // colorMode
            bitflags.colmode = S(0);
        case 424: // rotateAuto
            bitflags.autoRotate = S(0);
            return;
        case 425: // scrollX
            uv_scroll_vel.x = f(0);
            return;
        case 426: // scrollY
            uv_scroll_vel.y = f(0);
            return;
        case 427: // scrollXTime
            u_vel_i.start(uv_scroll_vel.x, f(2), S(0), S(1));
            return;
        case 428: // scrollYTime
            v_vel_i.start(uv_scroll_vel.y, f(2), S(0), S(1));
            return;
        case 429: // zoomOut
            uv_scale = { f(0), f(1) };
            return;
        case 430: // zoomOutTime
            uv_scale_i.start(uv_scale, { f(2), f(3) }, S(0), S(1));
            return;
        case 431: // ins_431
            bitflags.f534_8 = S(0);
            return;
        case 432: // ins_432
            bitflags.f534_9 = S(0);
            return;
        case 433: // ins_433
            pos_i.start(pos, { f(3)*cos(f(2)), f(3)*sin(f(2)), pos.z }, S(0), S(1));
            return;
        case 434: // scale2
            scale_2 = { f(0), f(1) };
            return;
        case 435: // scale2Time
            scale_2_i.start(scale_2, { f(2), f(3) }, S(0), S(1));
            return;
        case 436: // anchorOffset
            anchor_offset = { -f(0), f(1) };
            return;
        case 437: // rotationMode
            bitflags.rotationMode = S(0);
            return;
        case 438: // originMode
            bitflags.originMode = S(0);
            return;
        case 439: // TODO: fadeNearCamera
            return;

        case 500: // scriptNew
            tempNode = new AnmVMList();
            tempNode->value = AnmManagerN::getVM(AnmManagerN::SpawnVM(anim_slot, S(0), false, false));
            tempNode->value->parent = this;
            tempNode->value->nodeAsChildren = tempNode;
            if (!childrens) childrens = tempNode;
            else {
                tempNode->previous = childrens;
                tempNode->next = childrens->next;
                if (childrens->next) childrens->next->previous = tempNode;
                childrens->next = tempNode;
            }
            // TODO: childs copy some bitflags
            return;
        case 501: // scriptNewUI
            tempNode = new AnmVMList();
            tempNode->value = AnmManagerN::getVM(AnmManagerN::SpawnVM(anim_slot, S(0), true, false));
            tempNode->value->parent = this;
            tempNode->value->nodeAsChildren = tempNode;
            if (!childrens) childrens = tempNode;
            else {
                tempNode->previous = childrens;
                tempNode->next = childrens->next;
                if (childrens->next) childrens->next->previous = tempNode;
                childrens->next = tempNode;
            }
            return;
        case 502: // scriptNewFront
            tempNode = new AnmVMList();
            tempNode->value = AnmManagerN::getVM(AnmManagerN::SpawnVM(anim_slot, S(0), false, true));
            tempNode->value->parent = this;
            tempNode->value->nodeAsChildren = tempNode;
            if (!childrens) childrens = tempNode;
            else {
                tempNode->previous = childrens;
                tempNode->next = childrens->next;
                if (childrens->next) childrens->next->previous = tempNode;
                childrens->next = tempNode;
            }
            tempNode->value->update();
            return;
        case 503: // scriptNewFrontUI
            tempNode = new AnmVMList();
            tempNode->value = AnmManagerN::getVM(AnmManagerN::SpawnVM(anim_slot, S(0), true, true));
            tempNode->value->parent = this;
            tempNode->value->nodeAsChildren = tempNode;
            if (!childrens) childrens = tempNode;
            else {
                tempNode->previous = childrens;
                tempNode->next = childrens->next;
                if (childrens->next) childrens->next->previous = tempNode;
                childrens->next = tempNode;
            }
            tempNode->value->update();
            return;
        case 504: // scriptNewRoot
            AnmManagerN::SpawnVM(anim_slot, S(0));
            return;
        case 505: // scriptNewPos
            createChild(S(0), false, false);
            //ListUtil::listInsertAfter(childrens, tempNode);
            if (!childrens) childrens = tempNode;
            else {
                tempNode->previous = childrens;
                tempNode->next = childrens->next;
                if (childrens->next) childrens->next->previous = tempNode;
                childrens->next = tempNode;
            }
            childrens->next->value->pos2 = { f(1), f(2), 0.f };
            return;
        case 506: // scriptNewPosRoot
            tempNode = new AnmVMList();
            tempNode->value = AnmManagerN::getVM(AnmManagerN::SpawnVM(anim_slot, S(0)));
            tempNode->value->pos2 = { f(1), f(2), 0.f };
            delete tempNode;
            return;
        case 507: // ins_507 (ignore parent)
            bitflags.noParent = S(0);
            return;
        case 508: // TODO: effectNew
            std::cout << "[WARNING] anm: instruction 508 (effectNew) is not implemented\n";
            bitflags.activeFlags = ANMVM_DELETE;
            return;
        case 509: // copyVars
            if (parent == nullptr) return;
            for (unsigned int i = 0; i < 4; i++)
            {
                int_vars[i] = parent->int_vars[i];
                float_vars[i] = parent->float_vars[i];
            }
            rot_vars = parent->rot_vars;
            script_var_8 = parent->script_var_8;
            script_var_9 = parent->script_var_9;
            rand_scale_1f = parent->rand_scale_1f;
            rand_scale_pi = parent->rand_scale_pi;
            return;

        case 600: // texCircle
            mode_of_special_draw = 0;
            return;
        case 601: // texArcEven
            mode_of_special_draw = 1;
            return;
        case 602: // texArc
            mode_of_special_draw = 2;
            return;
        case 603: // drawRect
            mode_of_special_draw = 3;
            float_vars[2] = f(0);
            float_vars[3] = f(1);
            return;
        case 604: // drawPoly
            mode_of_special_draw = 4;
            float_vars[3] = f(0);
            int_vars[3] = S(1);
            return;
        case 605: // drawPolyBorder
            mode_of_special_draw = 5;
            float_vars[3] = f(0);
            int_vars[3] = S(1);
            return;
        case 606: // drawRectGrad
            mode_of_special_draw = 6;
            float_vars[2] = f(0);
            float_vars[3] = f(1);
            return;
        case 607: // drawRectRot
            mode_of_special_draw = 7;
            float_vars[2] = f(0);
            float_vars[3] = f(1);
            return;
        case 608: // drawRectRotGrad
            mode_of_special_draw = 8;
            float_vars[2] = f(0);
            float_vars[3] = f(1);
            return;
        case 609: // texCylinder3D
            mode_of_special_draw = 9;
            return;
        case 610: // texRing3D
            mode_of_special_draw = 10;
            return;
        case 611: // drawRing
            mode_of_special_draw = 11;
            float_vars[3] = f(0);
            float_vars[2] = f(1);
            int_vars[3] = S(2);
            return;
        case 612: // drawRectBorder
            mode_of_special_draw = 12;
            float_vars[2] = f(0);
            float_vars[3] = f(1);
            return;
        case 613: // drawLine
            mode_of_special_draw = 13;
            float_vars[3] = f(0);
            return;
        case 614: // ins_614
            mode_of_special_draw = 14;
            float_vars[3] = f(0);
            float_vars[2] = f(1);
            return;

    }
    #undef u8S
    #undef S
    #undef f
    #undef rS
    #undef rf
    #undef argf
    #undef argS
    #undef flagl
    #undef flagTl
    #undef flagh
    #undef flagTh
    #undef jump
    #undef createChild
}
