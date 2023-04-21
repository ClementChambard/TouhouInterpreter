#ifndef ANMMANAGER_H_
#define ANMMANAGER_H_

#include "AnmFile.h"
#include "AnmShader.h"
#include "AnmVM.h"
#include <array>

struct RenderVertex_t {
    glm::vec4 transformed_pos {};
    NSEngine::Color diffuse_color {};
    glm::vec2 texture_uv {};
};

struct AnmVertexBuffers_t {
    int32_t unrendered_sprite_count = 0;
    RenderVertex_t sprite_vertex_data[131072] {};
    RenderVertex_t* sprite_write_cursor = sprite_vertex_data;
    RenderVertex_t* sprite_render_cursor = sprite_vertex_data;
    int32_t unrendered_primitive_count = 0;
    RenderVertex_t primitive_vertex_data[32768] {};
    RenderVertex_t* primitive_write_cursor = primitive_vertex_data;
    RenderVertex_t* primitive_render_cursor = primitive_vertex_data;
};

extern RenderVertex_t SPRITE_TEMP_BUFFER[4];
extern float RESOLUTION_MULT;

class AnmManager {

public:
    static void Init();
    static void Cleanup();

    static uint32_t SpawnVM(size_t slot, size_t script, bool ui = false, bool front = false);
    static AnmVM* SpawnVMExt(size_t slot, size_t script);

    static AnmVM* getVM(uint32_t id);
    static AnmVM* getVM(AnmID id) { return getVM(id.val); }

    static void deleteVM(uint32_t id);
    static void deleteVM(AnmVM* vm);
    static void killAll();
    static bool isAlive(uint32_t id);
    static AnmFile* LoadFile(size_t slot, std::string filename);
    static AnmID createVM508(int i, AnmVM* root);

    static void flush_vbos();
    static void setup_render_state_for_vm(AnmVM* vm);
    static void write_sprite(const RenderVertex_t* buffer);
    static void calc_mat_world(AnmVM* vm);
    static void drawVM(AnmVM* vm);
    static void draw_vm__modes_0_1_2_3(AnmVM* vm, int i);

    static void update(bool printInstr = false);
    static void draw();
    static void on_tick_world();
    static void on_tick_ui();
    static void on_draw(uint32_t layer);

    static void drawSprite(size_t slot, size_t& spriteID);
    static void drawTexture(size_t slot, size_t& texID);

    static int getFreeAnm();

    static AnmFile* getLoaded(int i) { return &loadedFiles[i]; }

    static void interrupt_tree(AnmID id, int interrupt)
    {
        auto vm = getVM(id.val);
        if (vm)
            vm->interruptRec(interrupt);
    }

private:
    // zThread
    // zAnmSaveRelated[4] pause_related
    // undefined4[2]
    static int32_t processed_anm_ctr_0xc8;
    static int32_t draw_call_ctr_0xcc;
    static float cam_vec2_fc_x;
    static float cam_vec2_fc_y;
    static int32_t render_loop_ctr_0xd8;
    static AnmVM __anm_vm_dc;
    static AnmVMList_t* world_list_head;
    static AnmVMList_t* world_list_tail;
    static AnmVMList_t* ui_list_head;
    static AnmVMList_t* ui_list_tail;
    static AnmFastVM fastArray[8191];
    static int32_t __lolk_next_snapshot_fast_id;
    static int32_t __lolk_next_snapshot_discriminator;
    static AnmVMList_t __lolk_vm_snapshot_list_head;
    static AnmFastVMList_t freelist_head;
    // undefined4
    static std::array<AnmFile, 32> loadedFiles;
    static glm::mat4 __matrix_186017c;
    static AnmVM __anm_vm_18601bc;
    static int32_t field_0x18607bc;
    static int32_t field_0x18607c0;
    static uint32_t last_used_texture;
    static uint8_t last_used_blendmode;
    static uint8_t field_0x18607c9;
    static uint8_t field_0x18607ca;
    static uint8_t field_0x18607cb;
    static uint8_t field_0x18607cc;
    static uint8_t field_0x18607cd;
    static uint8_t last_used_resamplemode;
    static uint8_t field_0x18607cf;
    static uint8_t last_used_scrollmodex;
    static uint8_t last_used_scrollmodey;
    // undefined2
    static float some_positions[22];
    static AnmVertexBuffers_t vertex_buffers;
    // dummy head (unused)
    static int32_t last_id_discriminator;
    static NSEngine::Color custom_color_1c90a48;
    static int32_t use_custom_color_1c90a4c;

    static bool initialized;
    static bool zwriteEnabled;
    static bool fogEnabled;
    static GLuint vboID;
    // static AnmShader shader;
    friend class AnmVM;

    static void setZwrite(bool active);
    static void setFog(bool active);
    static void bindBuffer();
    static void drawBuffer(RenderVertex_t* start, uint32_t count);
    static void unbindBuffer();
};

#endif // ANMMANAGER_H_
