#include "AnmBitflags.h"
#include "AnmVM.h"
#include "AnmFuncs.h"
#include <math/Random.h>
#include "AnmManager.h"

int AnmVM::exec_instruction(int8_t* ins)
{
    uint16_t type = *reinterpret_cast<uint16_t*>(ins);
    #define argS(x) *reinterpret_cast<int32_t*>(&(ins[8+4*x]))
    #define argf(x) *reinterpret_cast<float*>(&(ins[8+4*x]))
    #define  S(x) check_val(argS(x))
    #define u8S(x) static_cast<uint8_t>(S(x))
    #define  f(x) check_val(argf(x))
    #define rS(x) check_ref(argS(x))
    #define rf(x) check_ref(argf(x))
    #define jump(n, t) {instr_offset = n; time_in_script = t;}
    switch (type)
    {

        case   0: // nop
            return 0;
        case   1: // destroy
            AnmManager::deleteVM(this);
            // bitflags.activeFlags = ANMVM_DELETE;
            return 0;
        case   2: // freeze
            instr_offset = -1;
            return 2; // QUIT
        case   3: // stop
            //if (time_in_script >= -10000) time_in_script = -10000-time_in_script;
            return 1; // WAIT_AFTER_STOP
        case   4: // stophide
            bitflags.visible = 0;
            //if (time_in_script >= -10000) time_in_script = -10000-time_in_script;
            return 1; // WAIT_AFTER_STOP
        case   5: // interruptLabel
            // do nothing (just a label)
            return 0;
        case   6: // wait
            time_in_script -= S(0);
            return 0;
        case   7: // caseReturn
            time_in_script = interrupt_return_time; // not there ???
            interrupt_return_time = 0;
            instr_offset = interrupt_return_offset;
            return 0;

        case 100: // iset
            rS(0)  = S(1);
            return 0;
        case 101: // fset
            rf(0)  = f(1);
            return 0;
        case 102: // iadd
            rS(0) += S(1);
            return 0;
        case 103: // fadd
            rf(0) += f(1);
            return 0;
        case 104: // isub
            rS(0) -= S(1);
            return 0;
        case 105: // fsub
            rf(0) -= f(1);
            return 0;
        case 106: // imul
            rS(0) *= S(1);
            return 0;
        case 107: // fmul
            rf(0) *= f(1);
            return 0;
        case 108: // idiv
            rS(0) /= S(1);
            return 0;
        case 109: // fdiv
            rf(0) /= f(1);
            return 0;
        case 110: // imod
            rS(0) %= S(1);
            return 0;
        case 111: // fmod
            rf(0) = fmod(f(0), f(1));
            return 0;
        case 112: // isetadd
            rS(0) = S(1) + S(2);
            return 0;
        case 113: // fsetadd
            rf(0) = f(1) + f(2);
            return 0;
        case 114: // isetsub
            rS(0) = S(1) - S(2);
            return 0;
        case 115: // fsetsub
            rf(0) = f(1) - f(2);
            return 0;
        case 116: // isetmul
            rS(0) = S(1) * S(2);
            return 0;
        case 117: // fsetmul
            rf(0) = f(1) * f(2);
            return 0;
        case 118: // isetdiv
            rS(0) = S(1) / S(2);
            return 0;
        case 119: // fsetdiv
            rf(0) = f(1) / f(2);
            return 0;
        case 120: // isetmod
            rS(0) = S(1) % S(2);
            return 0;
        case 121: // fsetmod
            rf(0) = fmod(f(1), f(2));
            return 0;
        case 122: // isetrand
            rS(0) = S(1) == 0 ? 0 : rand() % S(1);
            return 0;
        case 123: // fsetrand
            rf(0) = Random::Float01() * f(1);
            return 0;
        case 124: // fsin
            rf(0) = sin(f(1));
            return 0;
        case 125: // fcos
            rf(0) = cos(f(1));
            return 0;
        case 126: // ftan
            rf(0) = tan(f(1));
            return 0;
        case 127: // facos
            rf(0) = acos(f(1));
            return 0;
        case 128: // fatan
            rf(0) = atan(f(1));
            return 0;
        case 129: // validAngle
            while (f(0) > PI) rf(0) -= PI2;
            while (f(0) <= -PI) rf(0) += PI2;
            return 0;
        case 130: // circlePos
            rf(0) = f(3) * cos(f(2));
            rf(1) = f(3) * sin(f(2));
            return 0;
        case 131: {// circlePosRand
            float tempvar1 = f(2) + (f(3)-f(2)) * Random::Float01();
            float tempvar2 = Random::Angle();
            rf(0) = tempvar1 * cos(tempvar2);
            rf(1) = tempvar1 * sin(tempvar2);
            return 0;}

        case 200: // jmp
            jump(S(0), S(1))
            return 0;
        case 201: // jmpDec
            if (rS(0)-- > 0) jump(S(1), S(2))
            return 0;
        case 202: // ije
            if (S(0) == S(1)) jump(S(2), S(3))
            return 0;
        case 203: // fje
            if (f(0) == f(1)) jump(S(2), S(3))
            return 0;
        case 204: // ijne
            if (S(0) != S(1)) jump(S(2), S(3))
            return 0;
        case 205: // fjne
            if (f(0) != f(1)) jump(S(2), S(3))
            return 0;
        case 206: // ijl
            if (S(0) <  S(1)) jump(S(2), S(3))
            return 0;
        case 207: // fjl
            if (f(0) <  f(1)) jump(S(2), S(3))
            return 0;
        case 208: // ijle
            if (S(0) <= S(1)) jump(S(2), S(3))
            return 0;
        case 209: // fjle
            if (f(0) <= f(1)) jump(S(2), S(3))
            return 0;
        case 210: // ijg
            if (S(0) >  S(1)) jump(S(2), S(3))
            return 0;
        case 211: // fjg
            if (f(0) >  f(1)) jump(S(2), S(3))
            return 0;
        case 212: // ijge
            if (S(0) >= S(1)) jump(S(2), S(3))
            return 0;
        case 213: // fjge
            if (f(0) >= f(1)) jump(S(2), S(3))
            return 0;

        case 300: { // sprite
            bitflags.visible = true;
            int s = S(0);
            if (s < 0) AnmManager::getLoaded(2)->setSprite(this, 258);
            else AnmManager::getLoaded(anm_loaded_index)->setSprite(this, s);
            __time_of_last_sprite_set__unused = time_in_script;
            return 0; }
        case 301: {// spriteRand
            bitflags.visible = true;
            int s = S(0);
            if (index_of_sprite_mapping_func == 0) s += rand() % S(1);
            else s = rand();
            if (s < 0) AnmManager::getLoaded(2)->setSprite(this, 258);
            else AnmManager::getLoaded(anm_loaded_index)->setSprite(this, s);
            __time_of_last_sprite_set__unused = time_in_script;
            return 0; }
        case 302: // type
            bitflags.rendermode = S(0);
            //if (bitflags.rendermode == 10) render_mode_10_471420(vm); // TODO
            return 0;
        case 303: // blendmode
            bitflags.blendmode = S(0);
            return 0;
        case 304: // layer
            setLayer(S(0));
            return 0;
        case 305: // zWriteDisable
            bitflags.zwritedis = S(0);
            return 0;
        case 306: // ins_306 : std related bitflag
            bitflags.f530_15 = S(0);
            return 0;
        case 307: // randMode
            bitflags.randomMode = S(0);
            return 0;
        case 308: // flipX
            scale.x *= -1;
            bitflags.flip_x = !bitflags.flip_x;
            return 0;
        case 309: // flipY
            scale.y *= -1;
            bitflags.flip_y = !bitflags.flip_y;
            return 0;
        case 310: // visible
            bitflags.visible = S(0);
            return 0;
        case 311: // resampleMode
            bitflags.resampleMode = S(0);
            return 0;
        case 312: // scrollMode
            bitflags.scrollX = S(0);
            bitflags.scrollY = S(1);
            return 0;
        case 313: // resolutionMode
            bitflags.resolutionMode = S(0);
            return 0;
        case 314: // ins_314
            bitflags.parRotate = S(0);
            return 0;
        case 315: // colorizeChildrens
            bitflags.colorizeChildren = S(0);
            return 0;
        case 316: // ins_316
            bitflags.f530_1 = 1;
            return 0;
        case 317: // ins_317
            bitflags.f530_1 = 0;
            return 0;

        case 400: // pos
            if (!bitflags.alt_pos) pos = { f(0), f(1), f(2) };
            else __pos_2 = { f(0), f(1), f(2) };
            return 0;
        case 401: // rotate
            rotation = { f(0), f(1), f(2) };
            bitflags.rotated = true;
            return 0;
        case 402: // scale
            scale = { f(0), f(1) };
            bitflags.scaled = true;
            return 0;
        case 403: // alpha
            color_1.a = u8S(0);
            return 0;
        case 404: // color
            color_1 = { u8S(0), u8S(1), u8S(2), color_1.a };
            return 0;
        case 405: // alpha2
            color_2.a = u8S(0);
            return 0;
        case 406: // color2
            color_2 = { u8S(0), u8S(1), u8S(2), color_2.a };
            return 0;
        case 407: // posTime
            pos_i.start(bitflags.alt_pos ? __pos_2 : pos, { f(2), f(3), f(4) }, S(0), S(1));
            return 0;
        case 408: // colorTime
            rgb1_i.start({color_1.r, color_1.g, color_1.b}, { S(2), S(3), S(4) }, S(0), S(1));
            return 0;
        case 409: // alphaTime
            alpha1_i.start(color_1.a, S(2), S(0), S(1));
            return 0;
        case 410: // rotateTime
            rotate_i.start(rotation, { f(2), f(3), f(4) }, S(0), S(1));
            bitflags.rotated = true;
            return 0;
        case 411: // rotateTime2D
            rotate_2d_i.start(rotation.z, f(2), S(0), S(1));
            bitflags.rotated = true;
            return 0;
        case 412: // scaleTime
            scale_i.start(scale, { f(2), f(3) }, S(0), S(1));
            bitflags.scaled = true;
            return 0;
        case 413: // color2Time
            rgb2_i.start({color_2.r, color_2.g, color_2.b}, { S(2), S(3), S(4) }, S(0), S(1));
            return 0;
        case 414: // alpha2Time
            alpha2_i.start(color_2.a, S(2), S(0), S(1));
            return 0;
        case 415: // rotVel
            angular_velocity = { f(0), f(1), f(2) };
            bitflags.hasGrowth = true;
            return 0;
        case 416: // scaleGrowth
            scale_growth = { f(0), f(1) };
            bitflags.hasGrowth = true;
            return 0;
        case 417: // alphaTimeLinear
            alpha1_i.start(color_1.a, S(1), S(0), 0);
            return 0;
        case 418: {// ins_418
            glm::vec4 temp_quad[4];
            write_sprite_corners_2d(temp_quad);
            uv_quad_of_sprite[0].x = (temp_quad[0].x < 0.0) ? 0 : temp_quad[0].x / 640.0;
            uv_quad_of_sprite[0].y = (temp_quad[0].y < 0.0) ? 0 : temp_quad[0].y / 480.0;
            uv_quad_of_sprite[1].x = (temp_quad[1].x < 0.0) ? 0 : temp_quad[1].x / 640.0;
            uv_quad_of_sprite[1].y = (temp_quad[1].y < 0.0) ? 0 : temp_quad[1].y / 480.0;
            uv_quad_of_sprite[2].x = (temp_quad[2].x < 0.0) ? 0 : temp_quad[2].x / 640.0;
            uv_quad_of_sprite[2].y = (temp_quad[2].y < 0.0) ? 0 : temp_quad[2].y / 480.0;
            uv_quad_of_sprite[3].x = (temp_quad[3].x < 0.0) ? 0 : temp_quad[3].x / 640.0;
            uv_quad_of_sprite[3].y = (temp_quad[3].y < 0.0) ? 0 : temp_quad[3].y / 480.0;
            return 0;}
        case 419: // ins_419
            bitflags.ins419 = S(0);
            return 0;
        case 420: // moveBezier
            pos_i.start_bezier(bitflags.alt_pos ? __pos_2 : pos, {f(4), f(5), f(6)}, {f(1), f(2), f(3)}, {f(7), f(8), f(9)}, S(0));
            return 0;
        case 421: // anchor
            bitflags.anchorX = (S(0) & 3);
            bitflags.anchorY = ((S(0) >> 16) & 3);
            return 0;
        case 422: // ins_422
            pos = entity_pos;
            entity_pos = {0, 0, 0};
            return 0;
        case 423: // colorMode
            bitflags.colmode = S(0);
            return 0;
        case 424: // rotateAuto
            bitflags.autoRotate = S(0);
            return 0;
        case 425: // scrollX
            uv_scroll_vel.x = f(0);
            bitflags.hasGrowth = true;
            return 0;
        case 426: // scrollY
            uv_scroll_vel.y = f(0);
            bitflags.hasGrowth = true;
            return 0;
        case 427: // scrollXTime
            u_vel_i.start(uv_scroll_vel.x, f(2), S(0), S(1));
            return 0;
        case 428: // scrollYTime
            v_vel_i.start(uv_scroll_vel.y, f(2), S(0), S(1));
            return 0;
        case 429: // zoomOut
            uv_scale = { f(0), f(1) };
            bitflags.zoomed = true;
            return 0;
        case 430: // zoomOutTime
            uv_scale_i.start(uv_scale, { f(2), f(3) }, S(0), S(1));
            bitflags.zoomed = true;
            return 0;
        case 431: // ins_431
            bitflags.f534_8 = S(0);
            return 0;
        case 432: // ins_432
            bitflags.noSlowdown = S(0);
            return 0;
        case 433: // ins_433
            pos_i.start(bitflags.alt_pos ? __pos_2 : pos, { f(3)*cos(f(2)), f(3)*sin(f(2)), pos.z }, S(0), S(1));
            return 0;
        case 434: // scale2
            scale_2 = { f(0), f(1) };
            bitflags.scaled = true;
            return 0;
        case 435: // scale2Time
            scale_2_i.start(scale_2, { f(2), f(3) }, S(0), S(1));
            bitflags.scaled = true;
            return 0;
        case 436: // anchorOffset
            anchor_offset = { f(0), f(1) };
            return 0;
        case 437: // rotationMode
            bitflags.rotationMode = S(0);
            return 0;
        case 438: // originMode
            bitflags.originMode = S(0);
            return 0;
        case 439: // TODO: fadeNearCamera (doesn't exist in th17 ???)
            return 0;

        case 500: // scriptNew
            AnmManager::getLoaded(anm_loaded_index)
                ->create_child_vm(S(0), this, 0);
            return 0;
        case 501: // scriptNewUI
            AnmManager::getLoaded(anm_loaded_index)
                ->create_child_vm(S(0), this, 4);
            return 0;
        case 502: // scriptNewFront
            AnmManager::getLoaded(anm_loaded_index)
                ->create_child_vm(S(0), this, 2);
            return 0;
        case 503: // scriptNewFrontUI
            AnmManager::getLoaded(anm_loaded_index)
                ->create_child_vm(S(0), this, 6);
            return 0;
        case 504: {// scriptNewRoot
            AnmManager::getLoaded(anm_loaded_index)->new_root(S(0), this);
            return 0;}
        case 505: {// scriptNewPos
            auto id = AnmManager::getLoaded(anm_loaded_index)
                ->create_child_vm(S(0), this, 0);
            auto vm = AnmManager::getVM(id);
            vm->__pos_2 = { f(1), f(2), 0.f };
            return 0; }
        case 506: {// scriptNewPosRoot
            auto id = AnmManager::getLoaded(anm_loaded_index)
                ->new_root(S(0), this);
            auto vm = AnmManager::getVM(id);
            vm->__pos_2 = { f(1), f(2), 0.f };
            return 0;}
        case 507: // ins_507 (ignore parent)
            bitflags.noParent = S(0);
            return 0;
        case 508: // effectNew
            AnmManager::createVM508(S(0), this);
            return 0;
        case 509: // copyVars
            if (parent_vm == nullptr) return 0;
            for (unsigned int i = 0; i < 4; i++)
            {
                int_script_vars[i] = parent_vm->int_script_vars[i];
                float_script_vars[i] = parent_vm->float_script_vars[i];
            }
            __script_vars_33_34_35 = parent_vm->__script_vars_33_34_35;
            __script_var_8 = parent_vm->__script_var_8;
            __script_var_9 = parent_vm->__script_var_9;
            rand_param_one = parent_vm->rand_param_one;
            rand_param_pi = parent_vm->rand_param_pi;
            rand_param_int = parent_vm->rand_param_int;
            return 0;

        case 600: // texCircle
            bitflags.rendermode = 9;
            alloc_special_vertex_buffer(S(0) * 2 * sizeof(RenderVertex_t));
            return 0;
        case 601: // texArcEven
            bitflags.rendermode = 13;
            alloc_special_vertex_buffer(S(0) * 2 * sizeof(RenderVertex_t));
            return 0;
        case 602: // texArc
            bitflags.rendermode = 14;
            alloc_special_vertex_buffer(S(0) * 2 * sizeof(RenderVertex_t));
            return 0;
        case 603: // drawRect
            bitflags.rendermode = 16;
            sprite_size.x = f(0);
            sprite_size.y = f(1);
            return 0;
        case 604: // drawPoly
            bitflags.rendermode = 17;
            sprite_size.x = f(0);
            int_script_vars[0] = S(1);
            return 0;
        case 605: // drawPolyBorder
            bitflags.rendermode = 18;
            sprite_size.x = f(0);
            int_script_vars[0] = S(1);
            return 0;
        case 606: // drawRectGrad
            bitflags.rendermode = 20;
            sprite_size.x = f(0);
            sprite_size.y = f(1);
            return 0;
        case 607: // drawRectRot
            bitflags.rendermode = 21;
            sprite_size.x = f(0);
            sprite_size.y = f(1);
            return 0;
        case 608: // drawRectRotGrad
            bitflags.rendermode = 22;
            sprite_size.x = f(0);
            sprite_size.y = f(1);
            return 0;
        case 609: // texCylinder3D
            bitflags.rendermode = 24;
            // actually, it uses special vertex without w component
            alloc_special_vertex_buffer(S(0) * 2 * sizeof(RenderVertex_t));
            return 0;
        case 610: // texRing3D
            bitflags.rendermode = 25;
            // actually, it uses special vertex without w component
            alloc_special_vertex_buffer(S(0) * 2 * sizeof(RenderVertex_t));
            return 0;
        case 611: // drawRing
            bitflags.rendermode = 19;
            sprite_size.x = f(0);
            sprite_size.y = f(1);
            int_script_vars[0] = S(2);
            return 0;
        case 612: // drawRectBorder
            bitflags.rendermode = 27;
            sprite_size.x = f(0);
            sprite_size.y = f(1);
            return 0;
        case 613: // drawLine
            bitflags.rendermode = 26;
            sprite_size.x = f(0);
            sprite_size.y = f(1);
            return 0;
        case 614: // ins_614 (not in th17 ???)
            //mode_of_create_child = 14;
            //sprite_size.x = f(0);
            //sprite_size.y = f(1);
            return 0;

    }
    #undef u8S
    #undef S
    #undef f
    #undef rS
    #undef rf
    #undef argf
    #undef argS
    #undef jump
    return 0;
}
