#include "AnmManager.h"
#include "AnmBitflags.h"
#include <DrawFuncs.h>
#include <NSlist.h>
#include <TextureManager.h>
// #include "../Supervisor.h"

// zThread
// zAnmSaveRelated[4] pause_related
// undefined4[2]
int32_t AnmManager::processed_anm_ctr_0xc8 = 0;
int32_t AnmManager::draw_call_ctr_0xcc = 0;
float AnmManager::cam_vec2_fc_x = 0.f;
float AnmManager::cam_vec2_fc_y = 0.f;
int32_t AnmManager::render_loop_ctr_0xd8 = 0;
AnmVM AnmManager::__anm_vm_dc {};
AnmVMList_t* AnmManager::world_list_head = nullptr;
AnmVMList_t* AnmManager::world_list_tail = nullptr;
AnmVMList_t* AnmManager::ui_list_head = nullptr;
AnmVMList_t* AnmManager::ui_list_tail = nullptr;
AnmFastVM AnmManager::fastArray[8191] {};
int32_t AnmManager::__lolk_next_snapshot_fast_id = 0;
int32_t AnmManager::__lolk_next_snapshot_discriminator = 0;
AnmVMList_t AnmManager::__lolk_vm_snapshot_list_head {};
AnmFastVMList_t AnmManager::freelist_head {};
// undefined4
std::array<AnmFile, 32> AnmManager::loadedFiles;
glm::mat4 AnmManager::__matrix_186017c = glm::mat4(1.f);
AnmVM AnmManager::__anm_vm_18601bc {};
int32_t AnmManager::field_0x18607bc = 0;
int32_t AnmManager::field_0x18607c0 = 0;
uint32_t AnmManager::last_used_texture = 0;
uint8_t AnmManager::last_used_blendmode = 0;
uint8_t AnmManager::field_0x18607c9 = 0;
uint8_t AnmManager::field_0x18607ca = 0;
uint8_t AnmManager::field_0x18607cb = 0;
uint8_t AnmManager::field_0x18607cc = 0;
uint8_t AnmManager::field_0x18607cd = 0;
uint8_t AnmManager::last_used_resamplemode = 0;
uint8_t AnmManager::field_0x18607cf = 0;
uint8_t AnmManager::last_used_scrollmodex = 0;
uint8_t AnmManager::last_used_scrollmodey = 0;
// undefined2
float AnmManager::some_positions[22] {};
AnmVertexBuffers_t AnmManager::vertex_buffers {};
// dummy head (unused)
int32_t AnmManager::last_id_discriminator = 0;
NSEngine::Color AnmManager::custom_color_1c90a48 {};
int32_t AnmManager::use_custom_color_1c90a4c = false;

bool AnmManager::zwriteEnabled = true;
bool AnmManager::fogEnabled = true;
GLuint AnmManager::vboID = 0;
// AnmShader AnmManager::shader;

void AnmManager::setZwrite(bool active)
{
    if (zwriteEnabled && !active) { // disable zwrite
        flush_vbos();
        glDepthMask(GL_FALSE);
        zwriteEnabled = false;
    } else if (!zwriteEnabled && active) { // enable zwrite
        flush_vbos();
        glDepthMask(GL_TRUE);
        zwriteEnabled = true;
    }
}
void AnmManager::setFog(bool active)
{
    if (fogEnabled && !active) {
        flush_vbos();
        // shader.SetFog(99999999.f, 99999999.f, SUPERVISOR.current_camera->sky.color_components);
        fogEnabled = false;
    }
    if (!fogEnabled && active) {
        flush_vbos();
        // shader.SetFog(SUPERVISOR.current_camera->sky.begin_distance, SUPERVISOR.current_camera->sky.end_distance, SUPERVISOR.current_camera->sky.color_components);
        fogEnabled = true;
    }
}

void AnmManager::bindBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, vboID);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(RenderVertex_t), reinterpret_cast<void*>(offsetof(RenderVertex_t, transformed_pos)));
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RenderVertex_t), reinterpret_cast<void*>(offsetof(RenderVertex_t, diffuse_color)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex_t), reinterpret_cast<void*>(offsetof(RenderVertex_t, texture_uv)));
}

void AnmManager::drawBuffer(RenderVertex_t* start, uint32_t count)
{
    // SUPERVISOR.d3d_device->SetFVF(0x144);
    // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, vertex_buffers.unrendered_sprite_count * 2, vertex_buffers.sprite_render_cursor, 0x1c);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(RenderVertex_t), nullptr, GL_DYNAMIC_DRAW); // orphan the buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(RenderVertex_t), start); // upload the data
    glDrawArrays(GL_TRIANGLES, 0, count);
}

void AnmManager::unbindBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AnmManager::Init()
{
    world_list_head = new AnmVMList_t();
    ui_list_head = new AnmVMList_t();
    world_list_tail = new AnmVMList_t();
    ui_list_tail = new AnmVMList_t();

    world_list_head->next = world_list_tail;
    world_list_tail->previous = world_list_head;
    ui_list_tail->previous = ui_list_head;
    ui_list_head->next = ui_list_tail;

    for (size_t i = 0; i < 8191; i++) {
        fastArray[i].fastID = i;
        ListUtil::listInsertAfter(&freelist_head, &fastArray[i].freelistNode);
    }

    glGenBuffers(1, &vboID);
}

void AnmManager::Cleanup()
{
    /* DESTROY HEAP VMS IN WORLD LIST */
    auto n = world_list_head;
    while (n->next != world_list_tail) {
        if ((n->next->value->id.val & AnmID::fastIdMask) == AnmID::fastIdMask)
            delete n->next->value;
        else
            n = n->next;
    }

    /* DESTROY HEAP VMS IN UI LIST */
    n = ui_list_head;
    while (n->next != ui_list_tail) {
        if ((n->next->value->id.val & AnmID::fastIdMask) == AnmID::fastIdMask)
            delete n->next->value;
        else
            n = n->next;
    }

    /* CLEAN FAST VMS */
    for (size_t i = 0; i < 8191; i++)
        fastArray[i].vm.destroy();

    /* CLEAN PREINIT VM */
    for (auto f : loadedFiles)
        f.Cleanup();

    if (vboID)
        glDeleteBuffers(1, &vboID);
}

AnmVM* AnmManager::SpawnVMExt(size_t slot, size_t script)
{
    AnmVM* activeVM = new AnmVM(loadedFiles[slot].getPreloaded(script));
    activeVM->id = AnmID::fastIdMask;
    activeVM->id.setDiscriminator(last_id_discriminator++);
    activeVM->script_id = script;
    return activeVM;
}

uint32_t AnmManager::SpawnVM(size_t slot, size_t script, bool ui, bool front)
{
    if (slot >= loadedFiles.size())
        return 0;
    if (loadedFiles[slot].name == "notLoaded")
        return -1;

    AnmVM* activeVM;

    auto n = freelist_head.next;
    if (n == nullptr) {
        // no free spot in the array
        activeVM = SpawnVMExt(slot, script);
    } else {
        ListUtil::listRemoveNode(n);
        n->value->fastID.setDiscriminator(++last_id_discriminator);
        n->value->isAlive = true;
        n->value->vm(loadedFiles[slot].getPreloaded(script));
        activeVM = &n->value->vm;
        activeVM->id = n->value->fastID;
    }

    if (ui) {
        if (front)
            ListUtil::listInsertAfter(ui_list_head, &activeVM->node_in_global_list);
        else
            ListUtil::listInsertBefore(ui_list_tail, &activeVM->node_in_global_list);
    } else {
        if (front)
            ListUtil::listInsertAfter(world_list_head, &activeVM->node_in_global_list);
        else
            ListUtil::listInsertBefore(world_list_tail, &activeVM->node_in_global_list);
    }
    activeVM->script_id = script; // FIXME: this is supposed to be correct from the beginning
    return activeVM->id.val;
}

void AnmManager::killAll()
{
    /* DESTROY HEAP VMS IN WORLD LIST */
    auto n = world_list_head;
    while (n->next != world_list_tail) {
        if ((n->next->value->id.val & AnmID::fastIdMask) == AnmID::fastIdMask)
            delete n->next->value;
        else
            n = n->next;
    }

    /* DESTROY HEAP VMS IN UI LIST */
    n = ui_list_head;
    while (n->next != ui_list_tail) {
        if ((n->next->value->id.val & AnmID::fastIdMask) == AnmID::fastIdMask)
            delete n->next->value;
        else
            n = n->next;
    }

    /* CLEAN FAST VMS */
    for (size_t i = 0; i < 8191; i++)
        fastArray[i].vm.destroy();

    /* CLEAN PREINIT VM */
    // for (auto f : loadedFiles)
    // f.Cleanup();
}

void AnmManager::deleteVM(uint32_t id)
{
    if (!id)
        return;
    if ((id & AnmID::fastIdMask) == AnmID::fastIdMask) {
        for (auto node = world_list_head->next; node != world_list_tail; node = node->next)
            if (node->value->id == id) {
                auto vm = node->value;
                delete vm;
                return;
            }
        for (auto node = ui_list_head->next; node != ui_list_tail; node = node->next)
            if (node->value->id == id) {
                auto vm = node->value;
                delete vm;
                return;
            }
        return;
    }
    if (fastArray[id & AnmID::fastIdMask].fastID != id)
        return;
    fastArray[id & AnmID::fastIdMask].vm.destroy();
}

void AnmManager::deleteVM(AnmVM* vm)
{
    if (!vm)
        return;
    if (vm->id.val == 0) {
        vm->destroy();
        return;
    }
    if ((vm->id.val & AnmID::fastIdMask) == AnmID::fastIdMask)
        delete vm;
    else {
        fastArray[vm->id.val & AnmID::fastIdMask].vm.destroy();
    }
}

bool AnmManager::isAlive(uint32_t id)
{
    if ((id & AnmID::fastIdMask) == AnmID::fastIdMask) {
        for (auto node = world_list_head->next; node != world_list_tail; node = node->next)
            if (node->value->id == id)
                return true;
        for (auto node = ui_list_head->next; node != ui_list_tail; node = node->next)
            if (node->value->id == id)
                return true;
        return false;
    }
    if (fastArray[id & AnmID::fastIdMask].fastID != id)
        return false;
    return fastArray[id & AnmID::fastIdMask].isAlive;
}

AnmVM* AnmManager::getVM(uint32_t id)
{
    if (id == 0)
        return nullptr;
    if ((id & AnmID::fastIdMask) == AnmID::fastIdMask) {
        for (auto node = world_list_head->next; node != world_list_tail; node = node->next)
            if (node->value->id == id)
                return node->value;
        for (auto node = ui_list_head->next; node != ui_list_tail; node = node->next)
            if (node->value->id == id)
                return node->value;
        return nullptr;
    }
    if (fastArray[id & AnmID::fastIdMask].fastID != id)
        return nullptr;
    if (fastArray[id & AnmID::fastIdMask].isAlive)
        return &(fastArray[id & AnmID::fastIdMask].vm);
    return nullptr;
}

AnmFile* AnmManager::LoadFile(size_t slot, std::string filename)
{
    if (loadedFiles[slot].name != "notLoaded") { /* do something */
    }
    loadedFiles[slot].Open(filename, slot);
    return &loadedFiles[slot];
}

void AnmManager::update(bool /*printInstr*/)
{
    on_tick_ui();
    on_tick_world();
}

int AnmManager::getFreeAnm()
{
    int n = 0;
    for (int i = 0; i < 8191; i++)
        if (!fastArray[i].isAlive)
            n++;
    return n;
}

void AnmManager::on_tick_world()
{
    for (auto node = world_list_head->next; node != world_list_tail; node = node->next) {
        if (node->value) {
            node->value->update();
            bool del = (node->value->bitflags.activeFlags & 1) && !(node->value->bitflags.activeFlags & 2);
            if (del) {
                node = node->previous;
                deleteVM(node->next->value);
            }
        }
    }
}

void AnmManager::on_tick_ui()
{
    for (auto node = ui_list_head->next; node != ui_list_tail; node = node->next) {
        if (node->value) {
            node->value->update();
            bool del = (node->value->bitflags.activeFlags & 1) && !(node->value->bitflags.activeFlags & 2);
            if (del) {
                node = node->previous;
                deleteVM(node->next->value);
            }
        }
    }
}

void AnmManager::draw()
{
    for (auto node = world_list_head->next; node != world_list_tail; node = node->next)
        if (node->value)
            node->value->draw();
    for (auto node = ui_list_head->next; node != ui_list_tail; node = node->next)
        if (node->value)
            node->value->draw();
}

void AnmManager::on_draw(uint32_t layer)
{
    for (auto node = world_list_head->next; node != world_list_tail; node = node->next)
        if (node->value && node->value->layer == layer)
            node->value->draw();
    for (auto node = ui_list_head->next; node != ui_list_tail; node = node->next)
        if (node->value && node->value->layer == layer)
            node->value->draw();
}

void AnmManager::drawSprite(size_t slot, size_t& spriteID)
{
    if (spriteID == (size_t)-1)
        spriteID = 0;
    if (spriteID >= loadedFiles[slot].sprites.size())
        spriteID = loadedFiles[slot].sprites.size() - 1;
    AnmSprite s = loadedFiles[slot].getSprite(spriteID);
    int tw = 1, th = 1;
    NSEngine::TextureManager::GetTextureSize(s.texID, tw, th);
    float u1 = s.x / (float)tw, u2 = (s.x + s.w) / (float)tw;
    float v1 = s.y / (float)th, v2 = (s.y + s.h) / (float)th;
    NSEngine::draw_quad_tex_2d(NSEngine::TextureManager::GetTextureID(s.texID), { -s.w / 2.f, +s.h / 2.f, u1, v1 }, { +s.w / 2.f, +s.h / 2.f, u2, v1 }, { +s.w / 2.f, -s.h / 2.f, u2, v2 }, { -s.w / 2.f, -s.h / 2.f, u1, v2 });
}

void AnmManager::drawTexture(size_t slot, size_t& texID)
{
    std::map<std::string, uint32_t>& texturesmap = loadedFiles[slot].textures;
    if (texID == (size_t)-1)
        texID = 0;
    if (texID >= texturesmap.size())
        texID = texturesmap.size() - 1;
    size_t i = 0;
    uint32_t tex = -1;
    for (auto t : texturesmap)
        if (i++ == texID) {
            tex = t.second;
            break;
        }
    int w = 0, h = 0;
    NSEngine::TextureManager::GetTextureSize(tex, w, h);
    NSEngine::draw_quad_tex_2d(NSEngine::TextureManager::GetTextureID(tex), { -w / 2.f, +h / 2.f, 0.f, 0.f }, { +w / 2.f, +h / 2.f, 1.f, 0.f }, { +w / 2.f, -h / 2.f, 1.f, 1.f }, { -w / 2.f, -h / 2.f, 0.f, 1.f });
}

RenderVertex_t SPRITE_TEMP_BUFFER[4];
float RESOLUTION_MULT = 1.f;

void AnmManager::flush_vbos() // TODO: do the render call
{
    if (vertex_buffers.unrendered_sprite_count != 0) {
        // if (field_0x18607cf != 1) {
        //  IDK
        // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        field_0x18607cf = 1;
        //}
        // IDK
        // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
        // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);

        // RENDER
        drawBuffer(vertex_buffers.sprite_render_cursor, vertex_buffers.unrendered_sprite_count * 6);
        draw_call_ctr_0xcc++;
        vertex_buffers.sprite_render_cursor = vertex_buffers.sprite_write_cursor;
        vertex_buffers.unrendered_sprite_count = 0;
    }
}

void AnmManager::setup_render_state_for_vm(AnmVM* vm)
{
    if (last_used_blendmode != vm->bitflags.blendmode) {
        flush_vbos();
        last_used_blendmode = vm->bitflags.blendmode;
        switch (last_used_blendmode) {
        case 0:
            glEnable(GL_ALPHA_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquation(GL_FUNC_ADD);
            break;
        case 1:
            glEnable(GL_ALPHA_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glBlendEquation(GL_FUNC_ADD);
            break;
        case 2:
            glEnable(GL_ALPHA_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
            break;
        case 3:
            glDisable(GL_ALPHA_TEST);
            glBlendFunc(GL_ONE, GL_ZERO);
            glBlendEquation(GL_FUNC_ADD);
            break;
        case 4:
            glEnable(GL_ALPHA_TEST);
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
            glBlendEquation(GL_FUNC_ADD);
            break;
        case 5:
            glEnable(GL_ALPHA_TEST);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            glBlendEquation(GL_FUNC_ADD);
            break;
        case 6:
            glEnable(GL_ALPHA_TEST);
            glBlendFunc(GL_ONE_MINUS_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquation(GL_FUNC_ADD);
            break;
        case 7:
            glEnable(GL_ALPHA_TEST);
            glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
            glBlendEquation(GL_FUNC_ADD);
            break;
        case 8:
            glEnable(GL_ALPHA_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glBlendEquation(GL_MIN);
            break;
        case 9:
            glEnable(GL_ALPHA_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glBlendEquation(GL_MAX);
            break;
        }
    }

    if (last_used_resamplemode != vm->bitflags.resampleMode) {
        flush_vbos();
        last_used_resamplemode = vm->bitflags.resampleMode;
        if (last_used_resamplemode == 0) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
    }

    if (last_used_scrollmodex != vm->bitflags.scrollX) {
        flush_vbos();
        last_used_scrollmodex = vm->bitflags.scrollX;
        if (last_used_scrollmodex == 0)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        else if (last_used_scrollmodex == 1)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        else if (last_used_scrollmodex == 2)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    }

    if (last_used_scrollmodey != vm->bitflags.scrollY) {
        flush_vbos();
        last_used_scrollmodey = vm->bitflags.scrollY;
        if (last_used_scrollmodex == 0)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        else if (last_used_scrollmodex == 1)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        else if (last_used_scrollmodex == 2)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    }

    processed_anm_ctr_0xc8++;
    return;
}

void AnmManager::write_sprite(const RenderVertex_t* buffer)
{
    if ((uint64_t)&vertex_buffers.sprite_write_cursor <= (uint64_t)vertex_buffers.sprite_write_cursor + 6)
        return;

    vertex_buffers.sprite_write_cursor[0] = buffer[0];
    vertex_buffers.sprite_write_cursor[1] = buffer[1];
    vertex_buffers.sprite_write_cursor[2] = buffer[2];
    vertex_buffers.sprite_write_cursor[3] = buffer[1];
    vertex_buffers.sprite_write_cursor[4] = buffer[2];
    vertex_buffers.sprite_write_cursor[5] = buffer[3];

    vertex_buffers.sprite_write_cursor += 6;
    vertex_buffers.unrendered_sprite_count++;
}

float DAT_00524710 = 0.f;
float DAT_00524714 = 448.f;

void AnmManager::calc_mat_world(AnmVM* vm)
{
    if (!vm->bitflags.f530_16) {
        vm->__matrix_2 = glm::scale(vm->__matrix_1, { vm->scale_2 * vm->scale, 1 });
        vm->bitflags.scaled = false;
        if (vm->rotation.x)
            vm->__matrix_2 = glm::rotate(vm->__matrix_2, vm->rotation.x, { 1, 0, 0 });
        if (vm->rotation.y)
            vm->__matrix_2 = glm::rotate(vm->__matrix_2, vm->rotation.y, { 0, 1, 0 });
        if (vm->rotation.z)
            vm->__matrix_2 = glm::rotate(vm->__matrix_2, vm->rotation.z, { 0, 0, 1 });
        vm->bitflags.rotated = false;
    }
    glm::vec3 p = vm->entity_pos + vm->pos + vm->__pos_2;
    if (vm->bitflags.originMode && !vm->parent_vm) {
        p.x += DAT_00524710 * 0.5;
        p.y += (DAT_00524714 - 448.0) * 0.5;
    }
    if (vm->parent_vm && !vm->bitflags.noParent)
        p += vm->parent_vm->entity_pos + vm->parent_vm->pos + vm->parent_vm->__pos_2;
    __matrix_186017c = glm::translate(vm->__matrix_2, p);
}

void AnmManager::drawVM(AnmVM* vm)
{
    // if (vm->index_of_on_draw) ANM_ON_DRAW_FUNCS[vm->index_of_on_draw](vm);

    if (!vm->bitflags.visible || !vm->bitflags.f530_1 || vm->bitflags.activeFlags)
        return;

    setZwrite(false);

    /* switch render mode */
    switch (vm->bitflags.rendermode) {
    case 0:
        if (vm->color_1.a == 0 && vm->color_2.a == 0)
            break;
        vm->write_sprite_corners__without_rot(SPRITE_TEMP_BUFFER[0].transformed_pos, SPRITE_TEMP_BUFFER[1].transformed_pos, SPRITE_TEMP_BUFFER[2].transformed_pos, SPRITE_TEMP_BUFFER[3].transformed_pos);
        draw_vm__modes_0_1_2_3(vm, 1);
        return;
    case 1:
    case 3:
        if (vm->color_1.a == 0 && vm->color_2.a == 0)
            break;
        vm->write_sprite_corners__with_z_rot(SPRITE_TEMP_BUFFER[0].transformed_pos, SPRITE_TEMP_BUFFER[1].transformed_pos, SPRITE_TEMP_BUFFER[2].transformed_pos, SPRITE_TEMP_BUFFER[3].transformed_pos);
        draw_vm__modes_0_1_2_3(vm, 0);
        return;
    case 2:
        if (vm->color_1.a == 0 && vm->color_2.a == 0)
            break;
        vm->write_sprite_corners__without_rot(SPRITE_TEMP_BUFFER[0].transformed_pos, SPRITE_TEMP_BUFFER[1].transformed_pos, SPRITE_TEMP_BUFFER[2].transformed_pos, SPRITE_TEMP_BUFFER[3].transformed_pos);
        draw_vm__modes_0_1_2_3(vm, 0);
        return;
    case 4:
        if (vm->color_1.a == 0 && vm->color_2.a == 0)
            break;
        if (!vm->write_sprite_corners__mode_4())
            draw_vm__modes_0_1_2_3(vm, 0);
        return;
    case 5:
        if (vm->color_1.a == 0 && vm->color_2.a == 0)
            break;
        calc_mat_world(vm);
        draw_vm__modes_0_1_2_3(vm, 0);
        SPRITE_TEMP_BUFFER[3].transformed_pos.w = 1.0;
        SPRITE_TEMP_BUFFER[2].transformed_pos.w = 1.0;
        SPRITE_TEMP_BUFFER[1].transformed_pos.w = 1.0;
        SPRITE_TEMP_BUFFER[0].transformed_pos.w = 1.0;
        return;
    case 6:
        if (vm->color_1.a == 0 && vm->color_2.a == 0)
            break;
        // draw_vm_mode_6(vm);
        return;
    case 7: {
        if (vm->color_1.a == 0 && vm->color_2.a == 0)
            break;
        calc_mat_world(vm);
        auto col [[maybe_unused]] = vm->bitflags.colmode ? vm->color_2 : vm->color_1;
        for (int i = 0; i < 4; i++) {
            // glm::vec3 v = __matrix_186017c * glm::vec4(some_positions[2],some_positions[3],some_positions[4],1);
            // if (math::point_distance(v, SUPERVISOR.current_camera->position) <= SUPERVISOR.current_camera->sky.begin_distance) {
            // SPRITE_TEMP_BUFFER[i].diffuse_color = col;
            //}
            // else {
            // float coeff = (SUPERVISOR.current_camera->sky.begin_distance - math::point_distance(v, SUPERVISOR.current_camera->position)) / (SUPERVISOR.current_camera->sky.begin_distance - SUPERVISOR.current_camera->sky.end_distance);
            // if (coeff < 1.0) {
            // SPRITE_TEMP_BUFFER[i].diffuse_color.b = col.b - ((col.b - SUPERVISOR.current_camera->sky.color_components[0]) * coeff);
            // SPRITE_TEMP_BUFFER[i].diffuse_color.g = col.g - ((col.g - SUPERVISOR.current_camera->sky.color_components[1]) * coeff);
            // SPRITE_TEMP_BUFFER[i].diffuse_color.r = col.r - ((col.r - SUPERVISOR.current_camera->sky.color_components[2]) * coeff);
            // SPRITE_TEMP_BUFFER[i].diffuse_color.a = col.a;
            //} else {
            // SPRITE_TEMP_BUFFER[i].diffuse_color = SUPERVISOR.current_camera->sky.color;
            // SPRITE_TEMP_BUFFER[i].diffuse_color.a = col.a;
            //}
            //}
        }
        draw_vm__modes_0_1_2_3(vm, 2);
        SPRITE_TEMP_BUFFER[3].transformed_pos.w = 1.0;
        SPRITE_TEMP_BUFFER[2].transformed_pos.w = 1.0;
        SPRITE_TEMP_BUFFER[1].transformed_pos.w = 1.0;
        SPRITE_TEMP_BUFFER[0].transformed_pos.w = 1.0;
        return;
    }
    case 8:
        if (vm->color_1.a == 0 && vm->color_2.a == 0)
            break;
        // draw_vm_mode_8_15(vm);
        return;
    case 9:
    case 0xc:
    case 0xd:
    case 0xe:
        // draw_vm__mode_textureCircle(vm,vm->special_vertex_buffer_data,vm->int_script_vars[0] * 2);
        return;
    default:
        return;
    case 0xb:
        // vm_draw_mode_11(vm,vm->special_vertex_buffer_data, vm->int_script_vars[0] * 2);
        return;
    case 0xf:
        if (vm->color_1.a == 0 && vm->color_2.a == 0)
            break;
        setFog(true);
        // draw_vm_mode_8_15(vm);
        setFog(false);
        return;
    case 0x10:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x1a:
    case 0x1b:
        // local_78 = vm->rotation.z;
        // local_74 = vm->sprite_size.x * vm->scale.x;
        // local_70 = vm->sprite_size.y * vm->scale.y;
        // local_6c = vm->pos + vm->entity_pos + vm->__pos_2;
        // vm->transform_coordinate(local_6c);
        // if (vm->parent_vm && !vm->bitflags.noParent) {
        // local_74 *= vm->parent_vm->scale.x;
        // local_70 *= vm->parent_vm->scale.y;
        // local_78 += vm->parent_vm->rotation.z;
        //}
        setup_render_state_for_vm(vm);
        // if (vm->bitflags.resolutionMode == 1) DVar5 = RESOLUTION_MULT * local_70;
        // else if (vm->bitflags.resolutionMode == 2) DVar5 = RESOLUTION_MULT * 0.5 * local_70;
        // else DVar5 = local_70;
        switch (vm->bitflags.rendermode) {
        case 0x10:
            // draw_vm__ins_603(DVar5, local_78, vm->color_1, vm->color_1, vm->bitflags.anchorX, vm->bitflags.anchorY);
            return;
        default:
            return;
        case 0x14:
            // draw_vm__ins_603(DVar5, local_78, vm->color_1, vm->color_2, vm->bitflags.anchorX, vm->bitflags.anchorY);
            return;
        case 0x15:
            // draw_vm__ins_607(DVar5, local_78, vm->color_1, vm->color_1, vm->bitflags.anchorX, vm->bitflags.anchorY);
            return;
        case 0x16:
            // draw_vm__ins_607(DVar5, local_78, vm->color_1, vm->color_2, vm->bitflags.anchorX, vm->bitflags.anchorY);
            return;
        case 0x1a:
            // if (!vm->bitflags.colmode) draw_vm__ins_613(DVar5, local_78, vm->color_1, vm->color_1, vm->bitflags.anchorX, vm->bitflags.anchorY);
            // else draw_vm__ins_613(DVar5, local_78, vm->color_1, vm->color_2, vm->bitflags.anchorX, vm->bitflags.anchorY);
            return;
        case 0x1b:
            // if (!vm->bitflags.colmode) draw_vm__ins_612(DVar5, local_78, vm->color_1, vm->color_1, vm->bitflags.anchorX, vm->bitflags.anchorY);
            // else draw_vm__ins_612(DVar5, local_78, vm->color_1, vm->color_2, vm->bitflags.anchorX, vm->bitflags.anchorY);
            return;
        }
    case 0x11:
    case 0x12:
    case 0x13:
        // local_70 = vm->rotation.z;
        // local_78 = vm->sprite_size.x * vm->scale.x;
        // local_74 = vm->sprite_size.y * vm->scale.y;
        // local_6c = vm->pos + vm->entity_pos + vm->__pos_2;
        // vm->transform_coordinate(local_6c);
        // if (vm->parent_vm && !vm->bitflags.noParent) {
        //   local_70 += vm->parent_vm->rotation.z;
        //   local_78 *= vm->parent_vm->scale.x;
        //   local_74 *= vm->parent_vm->scale.y;
        // }
        // uVar4 = *(uint *)&(vm->prefix).field_0x534 & 0x700000;
        // if (uVar4 == 0x100000) {
        //   local_74 = RESOLUTION_MULT * local_74;
        //   local_78 = RESOLUTION_MULT * local_78;
        // }
        // else if (uVar4 == 0x200000) {
        //   local_74 = RESOLUTION_MULT * 0.5 * local_74;
        //   local_78 = RESOLUTION_MULT * 0.5 * local_78;
        // }
        // setup_render_state_for_vm(vm);
        // if (vm->bitflags.rendermode == 0x11) draw_vm__mode_17__drawCircle(local_6c.x, local_6c.y, local_70, local_78, vm->int_script_vars[0], vm->color_1, vm->color_2);
        // else if (vm->bitflags.rendermode == 0x12) draw_vm__mode_18__drawCircleBorder(local_6c.x, local_6c.y, local_70, local_78, vm->int_script_vars[0], vm->color_1);
        // else if (vm->bitflags.rendermode == 0x13) draw_vm__mode_18__ins_611(local_6c.x, local_6c.y, local_70, local_78, local_74, vm->int_script_vars[0], vm->color_1);
        return;
    case 0x18:
    case 0x19:
        // vm_draw_mode_24_25(vm, vm->special_vertex_buffer_data, vm->int_script_vars[0] * 2);
        return;
    }
}

void AnmManager::draw_vm__modes_0_1_2_3(AnmVM* vm, int i)
{
    SPRITE_TEMP_BUFFER[0].transformed_pos.x += cam_vec2_fc_x;
    SPRITE_TEMP_BUFFER[0].transformed_pos.y += cam_vec2_fc_y;
    SPRITE_TEMP_BUFFER[1].transformed_pos.x += cam_vec2_fc_x;
    SPRITE_TEMP_BUFFER[1].transformed_pos.y += cam_vec2_fc_y;
    SPRITE_TEMP_BUFFER[2].transformed_pos.x += cam_vec2_fc_x;
    SPRITE_TEMP_BUFFER[2].transformed_pos.y += cam_vec2_fc_y;
    SPRITE_TEMP_BUFFER[3].transformed_pos.x += cam_vec2_fc_x;
    SPRITE_TEMP_BUFFER[3].transformed_pos.y += cam_vec2_fc_y;
    if (i & 1) {
        SPRITE_TEMP_BUFFER[0].transformed_pos.x = round(SPRITE_TEMP_BUFFER[0].transformed_pos.x) - 0.5;
        SPRITE_TEMP_BUFFER[1].transformed_pos.x = round(SPRITE_TEMP_BUFFER[1].transformed_pos.x) - 0.5;
        SPRITE_TEMP_BUFFER[0].transformed_pos.y = round(SPRITE_TEMP_BUFFER[0].transformed_pos.y) - 0.5;
        SPRITE_TEMP_BUFFER[2].transformed_pos.y = round(SPRITE_TEMP_BUFFER[2].transformed_pos.y) - 0.5;
        SPRITE_TEMP_BUFFER[1].transformed_pos.y = SPRITE_TEMP_BUFFER[0].transformed_pos.y;
        SPRITE_TEMP_BUFFER[2].transformed_pos.x = SPRITE_TEMP_BUFFER[0].transformed_pos.x;
        SPRITE_TEMP_BUFFER[3].transformed_pos.x = SPRITE_TEMP_BUFFER[1].transformed_pos.x;
        SPRITE_TEMP_BUFFER[3].transformed_pos.y = SPRITE_TEMP_BUFFER[2].transformed_pos.y;
    }
    vm->last_rendered_quad_in_surface_space[0] = SPRITE_TEMP_BUFFER[0].transformed_pos;
    vm->last_rendered_quad_in_surface_space[1] = SPRITE_TEMP_BUFFER[1].transformed_pos;
    vm->last_rendered_quad_in_surface_space[2] = SPRITE_TEMP_BUFFER[2].transformed_pos;
    vm->last_rendered_quad_in_surface_space[3] = SPRITE_TEMP_BUFFER[3].transformed_pos;

    /* if max of x pos is smaller than left of viewport --> discard drawing */
    // if (fmax(fmax(SPRITE_TEMP_BUFFER[0].transformed_pos.x, SPRITE_TEMP_BUFFER[1].transformed_pos.x), fmax(SPRITE_TEMP_BUFFER[2].transformed_pos.x, SPRITE_TEMP_BUFFER[3].transformed_pos.x))
    //     < SUPERVISOR.current_camera->viewport.X) return;

    /* if max of y pos is smaller than top of viewport --> discard drawing */
    // if (fmax(fmax(SPRITE_TEMP_BUFFER[0].transformed_pos.y, SPRITE_TEMP_BUFFER[1].transformed_pos.y), fmax(SPRITE_TEMP_BUFFER[2].transformed_pos.y, SPRITE_TEMP_BUFFER[3].transformed_pos.y))
    //     < SUPERVISOR.current_camera->viewport.Y) return;

    /* if min of x pos is larger than right of viewport --> discard drawing */
    // if (fmin(fmin(SPRITE_TEMP_BUFFER[0].transformed_pos.x, SPRITE_TEMP_BUFFER[1].transformed_pos.x), fmin(SPRITE_TEMP_BUFFER[2].transformed_pos.x, SPRITE_TEMP_BUFFER[3].transformed_pos.x))
    //     > SUPERVISOR.current_camera->viewport.Width + SUPERVISOR.current_camera->viewport.X) return;

    /* if min of y pos is larger than bottom of viewport --> discard drawing */
    // if (fmin(fmin(SPRITE_TEMP_BUFFER[0].transformed_pos.y, SPRITE_TEMP_BUFFER[1].transformed_pos.y), fmin(SPRITE_TEMP_BUFFER[2].transformed_pos.y, SPRITE_TEMP_BUFFER[3].transformed_pos.y))
    //     > SUPERVISOR.current_camera->viewport.Height + SUPERVISOR.current_camera->viewport.Y) return;

    SPRITE_TEMP_BUFFER[0].texture_uv.x = vm->uv_scroll_pos.x + vm->uv_quad_of_sprite[0].x;
    SPRITE_TEMP_BUFFER[0].texture_uv.y = vm->uv_scroll_pos.y + vm->uv_quad_of_sprite[0].y;
    SPRITE_TEMP_BUFFER[1].texture_uv.y = vm->uv_scroll_pos.y + vm->uv_quad_of_sprite[1].y;
    SPRITE_TEMP_BUFFER[1].texture_uv.x = vm->uv_scroll_pos.x + vm->uv_quad_of_sprite[0].x + (vm->uv_quad_of_sprite[1].x - vm->uv_quad_of_sprite[0].x) * vm->uv_scale.x;
    SPRITE_TEMP_BUFFER[2].texture_uv.x = vm->uv_scroll_pos.x + vm->uv_quad_of_sprite[2].x;
    SPRITE_TEMP_BUFFER[2].texture_uv.y = vm->uv_scroll_pos.y + vm->uv_quad_of_sprite[0].y + (vm->uv_quad_of_sprite[2].y - vm->uv_quad_of_sprite[0].y) * vm->uv_scale.y;
    SPRITE_TEMP_BUFFER[3].texture_uv.x = vm->uv_scroll_pos.x + vm->uv_quad_of_sprite[2].x + (vm->uv_quad_of_sprite[3].x - vm->uv_quad_of_sprite[2].x) * vm->uv_scale.x;
    SPRITE_TEMP_BUFFER[3].texture_uv.y = vm->uv_scroll_pos.y + vm->uv_quad_of_sprite[1].y + (vm->uv_quad_of_sprite[3].y - vm->uv_quad_of_sprite[1].y) * vm->uv_scale.y;

    if (last_used_texture != vm->getSprite().opengl_texid) {
        last_used_texture = vm->getSprite().opengl_texid;
        flush_vbos();
        glBindTexture(GL_TEXTURE_2D, last_used_texture);
    }

    if (field_0x18607ca != 1) {
        flush_vbos();
        field_0x18607ca = 1;
    }

    if (!(i & 2)) {
        NSEngine::Color col = vm->color_1;
        NSEngine::Color col2 = vm->color_2;
        if (vm->bitflags.colmode == 2) {
            if (use_custom_color_1c90a4c) {
                col = NSEngine::mix(col, custom_color_1c90a48);
                col2 = NSEngine::mix(col2, custom_color_1c90a48);
            }
            SPRITE_TEMP_BUFFER[0].diffuse_color = col;
            SPRITE_TEMP_BUFFER[1].diffuse_color = col2;
            SPRITE_TEMP_BUFFER[2].diffuse_color = col;
            SPRITE_TEMP_BUFFER[3].diffuse_color = col2;
        } else if (vm->bitflags.colmode == 3) {
            if (use_custom_color_1c90a4c) {
                col = NSEngine::mix(col, custom_color_1c90a48);
                col2 = NSEngine::mix(col2, custom_color_1c90a48);
            }
            SPRITE_TEMP_BUFFER[0].diffuse_color = col;
            SPRITE_TEMP_BUFFER[1].diffuse_color = col;
            SPRITE_TEMP_BUFFER[2].diffuse_color = col2;
            SPRITE_TEMP_BUFFER[3].diffuse_color = col2;
        } else {
            if (vm->bitflags.colmode == 1)
                col = vm->color_2;
            if (vm->bitflags.colorizeChildren && vm->parent_vm)
                col = NSEngine::mix(col, vm->parent_vm->mixed_inherited_color);
            vm->mixed_inherited_color = col;
            if (use_custom_color_1c90a4c)
                col = NSEngine::mix(col, custom_color_1c90a48);
            SPRITE_TEMP_BUFFER[0].diffuse_color = col;
            SPRITE_TEMP_BUFFER[1].diffuse_color = col;
            SPRITE_TEMP_BUFFER[2].diffuse_color = col;
            SPRITE_TEMP_BUFFER[3].diffuse_color = col;
        }
    }

    setup_render_state_for_vm(vm);
    write_sprite(SPRITE_TEMP_BUFFER);
}

struct TableAnm508_t {
    int16_t anm_loaded_index = 0;
    int16_t script_index = 0;
    std::function<int(AnmVM*)> on_create = nullptr;
    int32_t index_of_on_tick = 0;
    int32_t index_of_on_draw = 0;
    int32_t index_of_on_destroy = 0;
    int32_t index_of_on_interrupt = 0;
    int32_t index_of_on_copy_1__disused = 0;
    int32_t index_of_on_copy_2__disused = 0;
} ANM_508_TABLE[] {
    { 0, 0, [](AnmVM* vm) { std::cout << "[WARNING] anm: effect 0 is not implemented\n"; vm->bitflags.activeFlags = ANMVM_DELETE; return 0; }, 1, 1, 1, 1, 0, 0 },
    { 0, 0, [](AnmVM* vm) { std::cout << "[WARNING] anm: effect 1 is not implemented\n"; vm->bitflags.activeFlags = ANMVM_DELETE; return 0; }, 2, 2, 2, 2, 1, 1 },
    { 0, 0, [](AnmVM* vm) { std::cout << "[WARNING] anm: effect 2 is not implemented\n"; vm->bitflags.activeFlags = ANMVM_DELETE; return 0; }, 3, 3, 3, 3, 0, 0 },
    { 0, 0, [](AnmVM* vm) { std::cout << "[WARNING] anm: effect 3 is not implemented\n"; vm->bitflags.activeFlags = ANMVM_DELETE; return 0; }, 3, 3, 3, 3, 0, 0 },
    { 0, 0, [](AnmVM* vm) { std::cout << "[WARNING] anm: effect 4 is not implemented\n"; vm->bitflags.activeFlags = ANMVM_DELETE; return 0; }, 5, 7, 4, 4, 0, 0 },
    { 0, 0, [](AnmVM* vm) { std::cout << "[WARNING] anm: effect 5 is not implemented\n"; vm->bitflags.activeFlags = ANMVM_DELETE; return 0; }, 5, 7, 4, 4, 0, 0 },
    { 0, 0, [](AnmVM* vm) { std::cout << "[WARNING] anm: effect 6 is not implemented\n"; vm->bitflags.activeFlags = ANMVM_DELETE; return 0; }, 5, 7, 4, 4, 0, 0 },
    { 0, 0, [](AnmVM* vm) { std::cout << "[WARNING] anm: effect 7 is not implemented\n"; vm->bitflags.activeFlags = ANMVM_DELETE; return 0; }, 5, 7, 4, 4, 0, 0 },
    { 0, 0, [](AnmVM* vm) { std::cout << "[WARNING] anm: effect 8 is not implemented\n"; vm->bitflags.activeFlags = ANMVM_DELETE; return 0; }, 5, 7, 4, 4, 0, 0 },
};

AnmID AnmManager::createVM508(int i, AnmVM* root)
{

    if (-1 < ANM_508_TABLE[i].script_index) {
        auto vm = root;
        if (!vm) {
            vm = AnmManager::getVM(AnmManager::SpawnVM(ANM_508_TABLE[i].anm_loaded_index, ANM_508_TABLE[i].script_index));
        }
        if (ANM_508_TABLE[i].on_create)
            ANM_508_TABLE[i].on_create(vm);
        vm->index_of_on_tick = ANM_508_TABLE[i].index_of_on_tick;
        vm->index_of_on_draw = ANM_508_TABLE[i].index_of_on_draw;
        vm->index_of_on_destroy = ANM_508_TABLE[i].index_of_on_destroy;
        vm->index_of_on_interrupt = ANM_508_TABLE[i].index_of_on_interrupt;
        vm->index_of_on_copy_1__disused = ANM_508_TABLE[i].index_of_on_copy_1__disused;
        vm->index_of_on_copy_2__disused = ANM_508_TABLE[i].index_of_on_copy_2__disused;
        return vm->id;
    }
    return 0;
}
