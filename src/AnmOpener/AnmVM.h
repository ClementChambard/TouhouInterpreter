#ifndef ANMVM_H_
#define ANMVM_H_

#include <functional>
#include <vector>
#include <glm/glm.hpp>
#include "anmOpener.h"
#include "../Interp.h"
#include <Interpolator.h>
#include <vertex.h>
#include <math/math.h>
#include <SpriteBatch.h>
#include "AnmSprite.h"
#include "AnmBitflags.h"

class AnmVM;
struct AnmFastVM;

struct AnmID {
    AnmID() {}
    AnmID(int i) { val = i; }
    static constexpr uint32_t fastIdMask = 8191;
    static constexpr uint32_t disctOffset = 14;
    uint32_t val = 0;
    void setDiscriminator(int32_t discr) { val = (val & fastIdMask) | (discr << disctOffset); }
    uint32_t getFastId() { return val & fastIdMask; }
    bool operator==(uint32_t other) { return val == other; }
    bool operator!=(uint32_t other) { return val != other; }
};

struct AnmVMList {
    AnmVM* value = nullptr;
    AnmVMList* next = nullptr;
    AnmVMList* previous = nullptr;
};

struct AnmFastVMList {
    AnmFastVM* value = nullptr;
    AnmFastVMList* next = nullptr;
    AnmFastVMList* previous = nullptr;
};

class AnmVM {

    public:
        AnmVM() {}
        AnmVM(AnmVM const& toCopy);
        void operator()(AnmVM const& other);
        AnmVM(uint32_t script_id, uint32_t anim_slot);

        ~AnmVM();

        void update(bool printInstr = false);
        void draw(NSEngine::SpriteBatch* sb = nullptr);
        void destroy();
        void interrupt(int i);
        void interruptRec(int i);

        int& check_ref(int i);
        float& check_ref(float f);
        int check_val(int i);
        float check_val(float f);
        void exec_instruction(int8_t* ins);

        void setPos(float x, float y, float z) { pos = {x, y, z}; }
        void setEntityPos(float x, float y, float z) { entity_pos = {x, y, z}; }
        void setPos2(float x, float y, float z) { pos2 = {x, y, z}; }
        void setScale(float x, float y) { scale = {x, y}; }
        void setScale2(float x, float y) { scale_2 = {x, y}; }
        void setI(int i, int v) { int_vars[i] = v; }
        void setf(int i, int v) { float_vars[i] = v; }
        void setLayer(uint32_t i) { layer = i; }
        void setEntity(void* e) { entity = e; }
        void setRotz(float z) { rotation.z = z; }
        void setFlags(AnmVM_flags_t flags) { bitflags = flags; }
        void refreshSprite(int s = -1) { sprite_id = on_set_sprite(this, s==-1?sprite_id:s); }
        AnmVM_flags_t getFlags() const { return bitflags; }
        int getMode() const;
        int getLayer() const { return layer; }
        int getZdis() const;
        void* getEntity() const { return entity; }
        AnmVMList* getChild() { return childrens; }
        uint32_t getID() const { return id.val; }
        AnmSprite getSprite() const;

        std::function<int(AnmVM*, int)> on_set_sprite = [](AnmVM* me, int spr){ return spr; };
        static int cnt;

        int32_t case_return_time = -99;
        int32_t return_instr = 0;
        uint32_t layer = 0;
        int32_t anim_slot = 0;
        int32_t sprite_id = 0;
        int32_t script_id = 0;
        int32_t current_instr = 0;
        glm::vec3 pos = {};
        glm::vec3 rotation = {};
        glm::vec3 angular_velocity = {};
        glm::vec2 scale = {1.f, 1.f};
        glm::vec2 scale_2 = {1.f, 1.f};
        glm::vec2 scale_growth = {};
        glm::vec2 uv_scale = {1.f, 1.f};
        glm::vec2 uv_scroll_pos = {};
        glm::vec2 anchor_offset = {};
        // undefined 4
        Interp<glm::vec3> pos_i = {};
        Interp<glm::vec3> rgb1_i = {};
        Interp<int> alpha1_i = {};
        Interp<glm::vec3> rotate_i = {};
        Interp<float> rotate_2d_i = {};
        Interp<glm::vec2> scale_i = {};
        Interp<glm::vec2> scale_2_i = {};
        Interp<glm::vec2> uv_scale_i = {};
        Interp<glm::vec3> rgb2_i = {};
        Interp<int> alpha2_i = {};
        Interp<float> u_vel_i = {};
        Interp<float> v_vel_i = {};
        glm::vec2 uv_quad_of_sprite[4] = {};
        glm::vec2 uv_scroll_vel = {};
        // 3 matrix4 (position, rotation, scale ?  used to not recalculate each frame ?)
        int32_t pending_switch_label = 0;
        // unused stuff ?
        int32_t int_vars[4] = {0, 0, 0, 0};
        float float_vars[4] = {0, 0, 0, 0};
        glm::vec3 rot_vars = {};        /* rotation related vars 33 34 35 */
        int32_t script_var_8 = 0;
        int32_t script_var_9 = 0;
        float rand_scale_1f = 1.f;
        float rand_scale_pi = PI;
        int32_t num_cycles_in_texture = 1;
        glm::vec3 pos2 = {};
    //  glm::vec3 last_rendered_quad_in_surface_space[4];  (used to not recalculate each frame ?)
        int32_t mode_of_special_draw = -1; // mode_of_create_child
        NSEngine::Color color1 = {255, 255, 255, 255};
        NSEngine::Color color2 = {255, 255, 255, 255};
        // mixed_inherited_color + more unused stuff
        AnmVM_flags_t bitflags;

        AnmID id = 0;
        uint32_t fast_id = 0;
        int32_t time = 0;
        AnmVMList* nodeInGlobalList = nullptr;
        AnmVMList* nodeAsChildren = nullptr;
        AnmVMList* childrens = nullptr;
        AnmVM* parent = nullptr;
        //things like that
        glm::vec3 entity_pos = {}; // pos3 ?
        void* entity = nullptr;


        friend class AnmManagerN;
        friend class AnmManager;
        friend class AnmFile;
        friend class StdFile;
};

struct AnmFastVM {
    AnmVM vm;
    AnmFastVMList* freelistNode = nullptr;
    bool isAlive = false;
    AnmID fastID = 0;
};


#endif // ANMVM_H_
