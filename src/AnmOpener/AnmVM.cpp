#include <NSEngine.h>
#include "AnmVM.h"
#include "AnmManagerN.h"
#include "AnmBitflags.h"
#include <vertex.h>
#include <math/Random.h>
#include <NSlist.h>

int AnmVM::cnt = 0;

void AnmVM::interrupt(int i)
{
    pending_switch_label = i;
}

void AnmVM::interruptRec(int i)
{
    interrupt(i);
    for (AnmVMList* child = childrens->next; child != nullptr; child = child->next)
    {
        if (child->value) child->value->interruptRec(i);
    }
}

AnmVM::AnmVM(AnmVM const& toCopy)
{
    case_return_time = toCopy.case_return_time ;
    return_instr     = toCopy.return_instr     ;
    layer            = toCopy.layer            ;
    anim_slot        = toCopy.anim_slot        ;
    sprite_id        = toCopy.sprite_id        ;
    script_id        = toCopy.script_id        ;
    current_instr    = toCopy.current_instr    ;
    pos              = toCopy.pos              ;
    rotation         = toCopy.rotation         ;
    angular_velocity = toCopy.angular_velocity ;
    scale            = toCopy.scale            ;
    scale_2          = toCopy.scale_2          ;
    scale_growth     = toCopy.scale_growth     ;
    uv_scale         = toCopy.uv_scale         ;
    uv_scroll_pos    = toCopy.uv_scroll_pos    ;
    anchor_offset    = toCopy.anchor_offset    ;
    uv_scroll_vel    = toCopy.uv_scroll_vel    ;
    rot_vars         = toCopy.rot_vars         ;
    script_var_8     = toCopy.script_var_8     ;
    script_var_9     = toCopy.script_var_9     ;
    pos2             = toCopy.pos2             ;
    color1           = toCopy.color1           ;
    color2           = toCopy.color2           ;
    bitflags         = toCopy.bitflags         ;
    rand_scale_1f    = toCopy.rand_scale_1f    ;
    rand_scale_pi    = toCopy.rand_scale_pi    ;
    entity_pos       = toCopy.entity_pos       ;
    on_set_sprite    = toCopy.on_set_sprite    ;
    entity           = toCopy.entity           ;

    pos_i = toCopy.      pos_i;
    rgb1_i = toCopy.     rgb1_i;
    alpha1_i = toCopy.   alpha1_i;
    rotate_i = toCopy.   rotate_i;
    rotate_2d_i = toCopy.rotate_2d_i;
    scale_i = toCopy.    scale_i;
    scale_2_i = toCopy.  scale_2_i;
    uv_scale_i = toCopy. uv_scale_i;
    rgb2_i = toCopy.     rgb2_i;
    alpha2_i = toCopy.   alpha2_i;
    u_vel_i = toCopy.    u_vel_i;
    v_vel_i = toCopy.    v_vel_i;

    for (int i = 0; i < 4; i++) {
        int_vars[i] = toCopy.int_vars[i];
        float_vars[i] = toCopy.float_vars[i];
    }

    num_cycles_in_texture = toCopy.num_cycles_in_texture;
    mode_of_special_draw = toCopy.mode_of_special_draw;
    time = 0;
    pending_switch_label = 0;
    if (childrens != nullptr) delete childrens;
    childrens = new AnmVMList();

    bitflags.activeFlags = ANMVM_ACTIVE;
    bitflags.originMode = 1;
}

void AnmVM::operator()(const AnmVM &toCopy)
{
    case_return_time = toCopy.case_return_time ;
    return_instr     = toCopy.return_instr     ;
    layer            = toCopy.layer            ;
    anim_slot        = toCopy.anim_slot        ;
    sprite_id        = toCopy.sprite_id        ;
    script_id        = toCopy.script_id        ;
    current_instr    = toCopy.current_instr    ;
    pos              = toCopy.pos              ;
    rotation         = toCopy.rotation         ;
    angular_velocity = toCopy.angular_velocity ;
    scale            = toCopy.scale            ;
    scale_2          = toCopy.scale_2          ;
    scale_growth     = toCopy.scale_growth     ;
    uv_scale         = toCopy.uv_scale         ;
    uv_scroll_pos    = toCopy.uv_scroll_pos    ;
    anchor_offset    = toCopy.anchor_offset    ;
    uv_scroll_vel    = toCopy.uv_scroll_vel    ;
    rot_vars         = toCopy.rot_vars         ;
    script_var_8     = toCopy.script_var_8     ;
    script_var_9     = toCopy.script_var_9     ;
    pos2             = toCopy.pos2             ;
    color1           = toCopy.color1           ;
    color2           = toCopy.color2           ;
    bitflags         = toCopy.bitflags         ;
    rand_scale_1f    = toCopy.rand_scale_1f    ;
    rand_scale_pi    = toCopy.rand_scale_pi    ;
    entity_pos       = toCopy.entity_pos       ;
    entity           = toCopy.entity           ;
    on_set_sprite    = toCopy.on_set_sprite    ;

    pos_i = toCopy.      pos_i;
    rgb1_i = toCopy.     rgb1_i;
    alpha1_i = toCopy.   alpha1_i;
    rotate_i = toCopy.   rotate_i;
    rotate_2d_i = toCopy.rotate_2d_i;
    scale_i = toCopy.    scale_i;
    scale_2_i = toCopy.  scale_2_i;
    uv_scale_i = toCopy. uv_scale_i;
    rgb2_i = toCopy.     rgb2_i;
    alpha2_i = toCopy.   alpha2_i;
    u_vel_i = toCopy.    u_vel_i;
    v_vel_i = toCopy.    v_vel_i;

    for (int i = 0; i < 4; i++) {
        int_vars[i] = toCopy.int_vars[i];
        float_vars[i] = toCopy.float_vars[i];
    }

    num_cycles_in_texture = toCopy.num_cycles_in_texture;
    mode_of_special_draw = toCopy.mode_of_special_draw;
    time = 0;
    pending_switch_label = 0;

    if (childrens != nullptr) delete childrens;
    childrens = new AnmVMList();

    bitflags.activeFlags = ANMVM_ACTIVE;
    bitflags.originMode = 1;
}

AnmVM::AnmVM(uint32_t script_id, uint32_t anim_slot)
{
    this->anim_slot = anim_slot;
    this->script_id = script_id;

    // When creating via instruction vector, update frame -1 of the VM.
    childrens = new AnmVMList();
    time = -1;
    update();
}

enum class bytefields : uint32_t {};
std::ostream& operator<<(std::ostream& left, bytefields bf)
{
    for (int i = 31; i >= 0; i--)
        left << (((uint32_t)bf & (1 << i)) != 0);
    return left;
}

AnmVM::~AnmVM()
{
    destroy();
    if (childrens != nullptr) delete childrens;
}

AnmSprite AnmVM::getSprite() const { return AnmManagerN::loadedFiles[anim_slot].getSprite(sprite_id); }

int AnmVM::update(bool /*printInstr*/)
{
    /* VM IS NOT RUNNING */
    if (bitflags.activeFlags != ANMVM_ACTIVE && time != -1) return 1;
    cnt++;

    /* UPDATE VARIABLES */
    rotation += angular_velocity;
    uv_scroll_pos += uv_scroll_vel;
    scale += scale_growth;

    // TODO not always
    if (pos_i.end_time != 0) pos = pos_i.step();
    if (rgb1_i.end_time != 0) {
        auto rgb1 = rgb1_i.step();
        color1 = {(uint8_t)rgb1.r, (uint8_t)rgb1.g, (uint8_t)rgb1.b, color1.a};
    }
    if (alpha1_i.end_time != 0) color1.a = alpha1_i.step();
    if (rotate_i.end_time != 0) rotation = rotate_i.step();
    if (rotate_2d_i.end_time != 0) rotate_2d_i.step(); //aaa
    if (scale_i.end_time != 0) scale = scale_i.step();
    if (scale_2_i.end_time != 0) scale_2 = scale_2_i.step();
    if (uv_scale_i.end_time != 0) uv_scale = uv_scale_i.step();
    if (rgb2_i.end_time != 0) {
        auto rgb2 = rgb2_i.step();
        color2 = {(uint8_t)rgb2.r, (uint8_t)rgb2.g, (uint8_t)rgb2.b, color2.a};
    }
    if (alpha2_i.end_time != 0) color2.a = alpha2_i.step();
    if (u_vel_i.end_time != 0) uv_scroll_vel.x = u_vel_i.step();
    if (v_vel_i.end_time != 0) uv_scroll_vel.y = v_vel_i.step();

    int8_t* instructions = AnmManagerN::loadedFiles[anim_slot].getScript(script_id);
    if (instructions == nullptr) return 1;

    /* CHECK FOR INTERRUPTIONS */
    if (pending_switch_label != 0)
    {
        //find label
        int32_t offset = 0;
        while (instructions[offset] != -1)
        {
            uint16_t instype = *reinterpret_cast<uint16_t*>(&(instructions[offset]));
            uint16_t inslength = *reinterpret_cast<uint16_t*>(&(instructions[offset+2]));
            int16_t instime = *reinterpret_cast<int16_t*>(&(instructions[offset+4]));
            if (instype == 5 && *reinterpret_cast<int32_t*>(&(instructions[offset+8])) == pending_switch_label)
            {
                return_instr = current_instr;
                case_return_time = time;
                current_instr = offset;
                time = instime;
                bitflags.visible = 1;
            }
            offset += inslength;
        }
        pending_switch_label = 0;
    }

    /* VM IS STOPPED */
    if (time < -9999) return 0;

    /* RUN INSTRUCTIONS */
    int32_t oldinstr = current_instr;
    uint16_t inslength;
    int16_t instime;
    if (instructions[current_instr] == -1) return 1;
    else {
        inslength = *reinterpret_cast<uint16_t*>(&(instructions[current_instr+2]));
        instime = *reinterpret_cast<int16_t*>(&(instructions[current_instr+4]));
    }
    while(instime <= time && (bitflags.activeFlags == ANMVM_ACTIVE || time == -1))
    {
        exec_instruction(&(instructions[current_instr]));

        if (oldinstr != current_instr)
        {
            if (instructions[current_instr] == -1) return 1;
            else {
                inslength = *reinterpret_cast<uint16_t*>(&(instructions[current_instr+2]));
                instime = *reinterpret_cast<int16_t*>(&(instructions[current_instr+4]));
            }
            if(instime <= time) exec_instruction(&(instructions[current_instr]));
        }
        current_instr+=inslength;
        oldinstr = current_instr;
        if (instructions[current_instr] == -1) return 1;
        else {
            inslength = *reinterpret_cast<uint16_t*>(&(instructions[current_instr+2]));
            instime = *reinterpret_cast<int16_t*>(&(instructions[current_instr+4]));
        }
    }
    time++;

    return 0;
}
int AnmVM::getMode() const { return bitflags.rendermode; }
int AnmVM::getZdis() const { return bitflags.zwritedis || bitflags.blendmode != 0; }

void AnmVM::destroy()
{
    bitflags = {};
    if (nodeInGlobalList != nullptr) { delete ListUtil::listRemoveNode(nodeInGlobalList); nodeInGlobalList = nullptr; }
    if (nodeAsChildren   != nullptr) { delete ListUtil::listRemoveNode(  nodeAsChildren); nodeAsChildren   = nullptr; }
    if (childrens != nullptr)
    {
        while (childrens->next) childrens->next->value->destroy();
        delete childrens; childrens = nullptr;
    }

    on_set_sprite = [](AnmVM*, int spr){ return spr; };
    case_return_time = -99;
    return_instr = 0;
    layer = 0;
    anim_slot = 0;
    sprite_id = 0;
    script_id = 0;
    current_instr = 0;
    pos = {0,0,0};
    rotation = {0,0,0};
    angular_velocity = {};
    scale = {1.f,1.f};
    scale_2 = {1,1};
    scale_growth = {};
    uv_scale = {1, 1};
    uv_scroll_pos = {};
    anchor_offset = {};
    uv_scroll_vel = {};
    pending_switch_label = 0;
    for (int i = 0; i < 4; i++) {
    int_vars[i] = 0;
    float_vars[i] = 0;}
    rot_vars = {};
    script_var_8 = 0;
    script_var_9 = 0;
    rand_scale_1f = 1.f;
    rand_scale_pi = PI;
    num_cycles_in_texture = 1;
    pos2 = {};
    mode_of_special_draw = -1; // mode_of_create_child
    color1 = {255, 255, 255, 255};
    color2 = {255, 255, 255, 255};
    id = 0;
    fast_id = 0;
    time = 0;
    parent = nullptr;
    entity_pos = {}; // pos3 ?
    entity = nullptr;
}
