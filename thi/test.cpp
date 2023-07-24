typedef int int32_t;
float checkVarF(float);
float checkVarI(int32_t);

#include <iostream>

int main() {
    int stackToRemove = 0;
    struct {
        int rank_mask;
        int id;
        int param_mask;
        int size;
        const unsigned char data[];
    } *instr = nullptr;
    struct {
        int difficultyMask;
        struct {
            int stackOffset;
        } stack;
        struct {
            int offset;
        } currentLocation;
    } *cont = nullptr;
    struct {
        struct {
            struct {
                float x, y, z;
            } __vec3_8;
            float __laser_new_arg_4;
        } BSs[8];
    } enemy;

    if ((instr->rank_mask & 63 & cont->difficultyMask)) {
        switch (instr->id) {
        case 0: {
            break;
        }
        case 700: {
            int32_t etid = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) etid = checkVarI(etid);
            float i_l = *reinterpret_cast<const float*>(&instr->data[4]);
            if (instr->param_mask&2) i_l = checkVarF(i_l);
            float f_l = *reinterpret_cast<const float*>(&instr->data[8]);
            if (instr->param_mask&4) f_l = checkVarF(f_l);
            float arg3 = *reinterpret_cast<const float*>(&instr->data[12]);
            if (instr->param_mask&8) arg3 = checkVarF(arg3);
            float w = *reinterpret_cast<const float*>(&instr->data[16]);
            if (instr->param_mask&16) w = checkVarF(w);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            enemy.BSs[etid].__vec3_8 = {i_l, f_l, arg3};
            enemy.BSs[etid].__laser_new_arg_4 = w;
            break;
        }
        case 701: {
            int32_t etid = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) etid = checkVarI(etid);
            int32_t start_t = *reinterpret_cast<const int32_t*>(&instr->data[4]);
            if (instr->param_mask&2) start_t = checkVarI(start_t);
            int32_t tr1_t = *reinterpret_cast<const int32_t*>(&instr->data[8]);
            if (instr->param_mask&4) tr1_t = checkVarI(tr1_t);
            int32_t dur = *reinterpret_cast<const int32_t*>(&instr->data[12]);
            if (instr->param_mask&8) dur = checkVarI(dur);
            int32_t tr2_t = *reinterpret_cast<const int32_t*>(&instr->data[16]);
            if (instr->param_mask&16) tr2_t = checkVarI(tr2_t);
            int32_t end_t = *reinterpret_cast<const int32_t*>(&instr->data[20]);
            if (instr->param_mask&32) end_t = checkVarI(end_t);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            enemy.BSs[etid].__laser_timing_end      = end_t;
            enemy.BSs[etid].__laser_timing_duration = dur;
            enemy.BSs[etid].__laser_timing_start    = start_t;
            enemy.BSs[etid].__laser_timing_trans1   = tr1_t;
            enemy.BSs[etid].__laser_timing_trans2   = tr2_t;
            break;
        }
        case 702: {
            int32_t etid = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) etid = checkVarI(etid);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            LaserLineInner_t inner;
            for (int _i_ = 0; _i_ < 20; _i_++)
                inner.et_ex[_i_] = enemy.BSs[etid].ex[_i_];
            if (enemy.bulletOrigins[etid].z <= 0.9f) {
                inner.start_pos = enemy.final_pos.pos + enemy.bulletOffsets[etid];
            } else {
                inner.start_pos = {
                    enemy.bulletOrigins[etid].x + enemy.bulletOffsets[etid].x,
                    enemy.bulletOrigins[etid].y + enemy.bulletOffsets[etid].y,
                    0.f
                };
            }
            inner.ang_aim            = enemy.BSs[etid].ang_aim;
            inner.bullet_color       = enemy.BSs[etid].__color;
            inner.bullet_type        = enemy.BSs[etid].type;
            inner.distance           = enemy.BSs[etid].distance;
            inner.flags              = enemy.BSs[etid].__laser_timing_end | 1;
            inner.laser_new_arg_1    = enemy.BSs[etid].__vec3_8.x;
            inner.laser_new_arg_2    = enemy.BSs[etid].__vec3_8.y;
            inner.laser_new_arg_3    = enemy.BSs[etid].__vec3_8.z;
            inner.laser_new_arg_4    = enemy.BSs[etid].__laser_new_arg_4;
            inner.shot_sfx           = enemy.BSs[etid].shot_sfx;
            inner.shot_transform_sfx = enemy.BSs[etid].__shot_transform_sfx;
            inner.spd_1              = enemy.BSs[etid].spd1;
            allocate_new_laser(0, &inner);
            break;
        }
        case 703: {
            int32_t etid = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) etid = checkVarI(etid);
            int32_t& id=checkVarRI(*reinterpret_cast<const int32_t*>(&instr->data[4]));
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            LaserInfiniteInner_t inner;
            for (int _i_ = 0; _i_ < 20; _i_++)
                inner.et_ex[_i_] = enemy.BSs[etid].ex[_i_];
            if (enemy.bulletOrigins[etid].z <= 0.9f) {
                inner.start_pos = enemy.final_pos.pos + enemy.bulletOffsets[etid];
            } else {
                inner.start_pos = {
                    enemy.bulletOrigins[etid].x + enemy.bulletOffsets[etid].x,
                    enemy.bulletOrigins[etid].y + enemy.bulletOffsets[etid].y,
                    0.f
                };
            }
            inner.type               = enemy.BSs[etid].type;
            inner.ang_aim            = enemy.BSs[etid].ang_aim;
            inner.color              = enemy.BSs[etid].__color;
            inner.distance           = enemy.BSs[etid].distance;
            inner.flags              = enemy.BSs[etid].__laser_timing_end | 2;
            inner.laser_duration     = enemy.BSs[etid].__laser_timing_duration;
            inner.laser_new_arg2     = enemy.BSs[etid].__vec3_8.y;
            inner.laser_new_arg1     = enemy.BSs[etid].__vec3_8.x;
            inner.laser_new_arg4     = enemy.BSs[etid].__laser_new_arg_4;
            inner.laser_st_on_arg1   = id;
            inner.laser_time_start   = enemy.BSs[etid].__laser_timing_start;
            inner.laser_trans_1      = enemy.BSs[etid].__laser_timing_trans1;
            inner.laser_trans_2      = enemy.BSs[etid].__laser_timing_trans2;
            inner.shot_sfx           = enemy.BSs[etid].shot_sfx;
            inner.shot_transform_sfx = enemy.BSs[etid].__shot_transform_sfx;
            inner.spd_1              = enemy.BSs[etid].spd1;
            allocate_new_laser(1, &inner);
            break;
        }
        case 704: {
            int32_t id = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) id = checkVarI(id);
            float x = *reinterpret_cast<const float*>(&instr->data[4]);
            if (instr->param_mask&2) x = checkVarF(x);
            float y = *reinterpret_cast<const float*>(&instr->data[8]);
            if (instr->param_mask&4) y = checkVarF(y);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            Laser* _l_ = find_laser_by_id(id);
            if (_l_) _l_->set_offset(x, y, 0.f);
            break;
        }
        case 705: {
            int32_t id = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) id = checkVarI(id);
            float xs = *reinterpret_cast<const float*>(&instr->data[4]);
            if (instr->param_mask&2) xs = checkVarF(xs);
            float ys = *reinterpret_cast<const float*>(&instr->data[8]);
            if (instr->param_mask&4) ys = checkVarF(ys);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            Laser* _l_ = find_laser_by_id(id);
            if (_l_) _l_->set_speed(xs, ys, 0.f);
            break;
        }
        case 706: {
            int32_t id = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) id = checkVarI(id);
            float s = *reinterpret_cast<const float*>(&instr->data[4]);
            if (instr->param_mask&2) s = checkVarF(s);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            Laser* _l_ = find_laser_by_id(id);
            if (_l_) _l_->set_speed(s);
            break;
        }
        case 707: {
            int32_t id = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) id = checkVarI(id);
            float w = *reinterpret_cast<const float*>(&instr->data[4]);
            if (instr->param_mask&2) w = checkVarF(w);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            Laser* _l_ = find_laser_by_id(id);
            if (_l_) _l_->set_width(w);
            break;
        }
        case 708: {
            int32_t id = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) id = checkVarI(id);
            float s = *reinterpret_cast<const float*>(&instr->data[4]);
            if (instr->param_mask&2) s = checkVarF(s);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            Laser* _l_ = find_laser_by_id(id);
            if (_l_) _l_->set_angle(a);
            break;
        }
        case 709: {
            int32_t id = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) id = checkVarI(id);
            float avel = *reinterpret_cast<const float*>(&instr->data[4]);
            if (instr->param_mask&2) avel = checkVarF(avel);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            Laser* _l_ = find_laser_by_id(id);
            if (dynamic_cast<LaserInfinite*>(_l_))
                dynamic_cast<LaserInfinite*>(_l_)->getInner().laser_inf_angular_velocity = avel;
            break;
        }
        case 710: {
            int32_t id = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) id = checkVarI(id);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            Laser* l = find_laser_by_id(id);
            while (l) {
                l->cancel(0, 0);
                l->set_id(0);
                l = find_laser_by_id(id);
            }
            break;
        }
        case 711: {
            int32_t etid = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) etid = checkVarI(etid);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            LaserCurveInner_t inner;
            for (int _i_ = 0; _i_ < 20; _i_++)
                inner.et_ex[_i_] = enemy.BSs[etid].ex[_i_];
            if (enemy.bulletOrigins[etid].z <= 0.9f) {
                inner.start_pos = enemy.final_pos.pos + enemy.bulletOffsets[etid];
            } else {
                inner.start_pos = {
                    enemy.bulletOrigins[etid].x + enemy.bulletOffsets[etid].x,
                    enemy.bulletOrigins[etid].y + enemy.bulletOffsets[etid].y,
                    0.f
                };
            }
            inner.ang_aim            = enemy.BSs[etid].ang_aim;
            inner.color              = enemy.BSs[etid].__color;
            inner.distance           = enemy.BSs[etid].distance;
            inner.init_1             = 1;
            inner.laser_new_arg4     = enemy.BSs[etid].__laser_new_arg_4;
            inner.laser_time_start   = enemy.BSs[etid].__laser_timing_start;
            inner.shot_sfx           = enemy.BSs[etid].shot_sfx;
            inner.shot_transform_sfx = enemy.BSs[etid].__shot_transform_sfx;
            inner.spd_1              = enemy.BSs[etid].spd1;
            inner.type               = enemy.BSs[etid].type;
            allocate_new_laser(2, &inner);
            break;
        }
        case 712: {
            float w = *reinterpret_cast<const float*>(&instr->data[0]);
            if (instr->param_mask&1) w = checkVarF(w);
            float h = *reinterpret_cast<const float*>(&instr->data[4]);
            if (instr->param_mask&2) h = checkVarF(h);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            _notImpl
            break;
        }
        case 713: {
            int32_t a = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) a = checkVarI(a);
            int32_t b = *reinterpret_cast<const int32_t*>(&instr->data[4]);
            if (instr->param_mask&2) b = checkVarI(b);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            LaserBeamInner_t inner;
            // TODO(ClementChambard)
            allocate_new_laser(3, &inner);
            break;
        }
        case 714: {
            int32_t id = *reinterpret_cast<const int32_t*>(&instr->data[0]);
            if (instr->param_mask&1) id = checkVarI(id);
            int32_t arg = *reinterpret_cast<const int32_t*>(&instr->data[4]);
            if (instr->param_mask&2) arg = checkVarI(arg);
            cont->stack.stackOffset -= stackToRemove;
            stackToRemove = 0;
            Laser* _l_ = find_laser_by_id(id);
            if (dynamic_cast<LaserBeam*>(_l_))
                dynamic_cast<LaserBeam*>(_l_)->method_8(arg);
            break;
        }
        default: {
            std::cout << "\e[92m ERROR: undefined instruction opcode "
                << instr->id << " \e[0m\n";
        }
        }
        cont->stack.stackOffset -= stackToRemove;
        stackToRemove = 0;
    }
    cont->currentLocation.offset += instr->size;
}
