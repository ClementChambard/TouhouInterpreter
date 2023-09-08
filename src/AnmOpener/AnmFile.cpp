#include "AnmFile.h"
#include "AnmFuncs.h"
#include "AnmManager.h"
#include "anmOpener.h"
#include "../GlobalData.h"
#include <ImageLoader.h>
#include <TextureManager.h>
#include <Texture.hpp>
#include <numeric>
#include <cstring>

void AnmSprite::genTexCoords(float tw, float th) {
    u1 = x / tw;
    v1 = y / th;
    u2 = (x + w) / tw;
    v2 = (y + h) / th;
}

AnmFile::AnmFile(std::string const& filename, uint32_t slot) {
    Open(filename, slot);
}

void AnmFile::Open(std::string const& filename, uint32_t slot) {
    if (name != "notLoaded")
        Cleanup();
    name = filename;
    AnmOpener::anm_archive_t* archive = AnmOpener::anm_read_file(filename);
    uint32_t scrID = 0;
    int at_tex_i = 0;
    int atr_tex_i = 0;
    for (auto entry : archive->entries) {
        uint32_t tex;
        std::string name = entry->name;
        if (entry->header->hasdata) {
            AnmOpener::image_t img = AnmOpener::anm_get_image(entry);
            NSEngine::Texture* t =
                new NSEngine::Texture(img.width, img.height, img.data);
            tex = NSEngine::TextureManager::AddTexture(std::move(*t));
            delete t;
            delete[] img.data;
        } else if (entry->name[0] == '@') {
            uint32_t w = entry->header->w;
            uint32_t h = entry->header->h;
            if (entry->name[1] == 'R') {
                w = BACK_BUFFER_SIZE.x;
                h = BACK_BUFFER_SIZE.y;
                name += std::to_string(atr_tex_i++);
            } else {
                name += std::to_string(at_tex_i++);
            }
            NSEngine::Texture* t = NSEngine::Texture::asFramebuffer(w, h);
            tex = NSEngine::TextureManager::AddTexture(std::move(*t));
            delete t;
        } else {
            tex = 0;
        }

        textures.insert(std::pair<std::string, uint32_t>(name, tex));
        int w, h;
        NSEngine::TextureManager::GetTextureSize(tex, w, h);
        for (auto spr : entry->sprites) {
            sprites.push_back({ tex, spr->x + entry->header->x, spr->y +
                entry->header->y, spr->w, spr->h, 0, 0, 0, 0,
                NSEngine::TextureManager::GetTextureID(tex) });
            sprites.back().genTexCoords(w, h);
        }
    }
    for (auto entry : archive->entries) {
        for (uint32_t scrid = 0; scrid < entry->scripts.size(); scrid++) {
            // generate data
            uint32_t size = std::accumulate(
                entry->scripts[scrid]->instrs.begin(),
                entry->scripts[scrid]->instrs.end(), 0,
                [](uint32_t a, auto b) { return a + b->length; });
            //            for (auto i : entry->scripts[scrid]->instrs)
            // size += i->length;
            scripts.push_back(new int8_t[size + 2]);
            uint32_t off = 0;
            for (auto ins : entry->scripts[scrid]->instrs) {
                for (uint32_t i = 0; i < ins->length; i++) {
                    scripts.back()[off++] = reinterpret_cast<int8_t*>(ins)[i];
                }
            }
            scripts.back()[size + 0] = -1;
            scripts.back()[size + 1] = -1;
            preloaded.emplace_back(scrID++, slot);
        }
    }

    AnmOpener::anm_free(archive);
    std::cout << "Opened Anm : " << filename << " on slot" << slot <<"\n";
    this->slot = slot;
}

void AnmFile::Cleanup() {
    if (name == "notLoaded") return;
    AnmManager::delete_of_file(this);
    for (auto s : this->scripts)
        delete[] s;
    for (auto vm : preloaded)
        vm.destroy();
    preloaded.clear();
    this->scripts.clear();
    sprites.clear();
    name = "notLoaded";
}

AnmFile::~AnmFile() {
    // for (auto s : scripts) delete s;
}

int8_t* AnmFile::getScript(size_t id) const {
    if (scripts.size() == 0)
        return {};
    return scripts[id % scripts.size()];
}

AnmVM AnmFile::getPreloaded(size_t id) const {
    if (preloaded.size() == 0)
        return {};
    return preloaded[id % preloaded.size()];
}

AnmSprite AnmFile::getSprite(size_t id) const {
    if (sprites.size() == 0)
        return {};
    return sprites[id % sprites.size()];
}

uint32_t AnmFile::getTextureFromName(std::string const& name) const {
    return textures.at(name);
}

void AnmFile::setSprite(AnmVM* vm, size_t sprite_id) {
    if (vm->index_of_sprite_mapping_func)
        sprite_id =
            ANM_ON_SPRITE_SET_FUNCS[vm->index_of_sprite_mapping_func]
            (vm, sprite_id);
    vm->sprite_id = sprite_id;
    if (sprite_id >= sprites.size())
        return;
    auto const& s = sprites[sprite_id];
    vm->uv_quad_of_sprite[2].x = s.u1;
    vm->uv_quad_of_sprite[0].x = s.u1;
    vm->uv_quad_of_sprite[3].x = s.u2;
    vm->uv_quad_of_sprite[1].x = s.u2;
    vm->uv_quad_of_sprite[1].y = s.v1;
    vm->uv_quad_of_sprite[0].y = s.v1;
    vm->uv_quad_of_sprite[3].y = s.v2;
    vm->uv_quad_of_sprite[2].y = s.v2;
    vm->sprite_size.x = s.w;
    vm->sprite_size.y = s.h;
    vm->__matrix_1 = glm::scale(glm::mat4(1.f),
                                { vm->sprite_size / 256.f, 1.f });
    vm->__matrix_3 = glm::scale(glm::mat4(1.f),
                                { s.u2 - s.u1, s.v2 - s.v1, 1.f });
    vm->__matrix_2 = vm->__matrix_1;
    return;
}

void AnmFile::copyFromLoaded(AnmVM* vm, int id) {
    if (static_cast<size_t>(id) >= scripts.size() || !vm) return;
    auto saved_entitypos = vm->entity_pos;
    auto saved_fast_id = vm->fast_id;
    auto saved_layer = vm->layer;
    memset(reinterpret_cast<char*>(&vm->id),
           0x0, sizeof(AnmVM) - offsetof(AnmVM, id));
    vm->entity_pos = saved_entitypos;
    vm->fast_id = saved_fast_id;
    vm->layer = saved_layer;
    vm->node_in_global_list = {vm, nullptr, nullptr};
    vm->__node_as_child = {vm, nullptr, nullptr};
    vm->list_of_children = {vm, nullptr, nullptr};
    if (id >= 0)
        memcpy(reinterpret_cast<char*>(vm),
            reinterpret_cast<char*>(&this->preloaded[id]), offsetof(AnmVM, id));
    vm->time_in_script = 0;
    vm->__timer_1c = 0;
}

AnmID AnmFile::createEffect(int id, int layer, AnmVM** output) {
    some_ctr++;
    auto vm = AnmManager::allocate_vm();
    if (output) *output = vm;
    copyFromLoaded(vm, id);
    vm->bitflags.randomMode = 1;
    if (layer >= 0) {
        vm->layer = layer;
        if (layer <= 0x17) {
            vm->bitflags.originMode = 1;
        }
    }
    vm->entity_pos = {};
    vm->rotation.z = 0;
    // vm->run();
    vm->update();
    vm->mode_of_create_child = 0;
    return AnmManager::insert_in_world_list_back(vm);
}

AnmID AnmFile::createEffectFront(int id, int layer, AnmVM** output) {
    some_ctr++;
    auto vm = AnmManager::allocate_vm();
    if (output) *output = vm;
    copyFromLoaded(vm, id);
    vm->bitflags.randomMode = 1;
    if (layer >= 0) {
        vm->layer = layer;
        if (layer <= 0x17) {
            vm->bitflags.originMode = 1;
        }
    }
    vm->entity_pos = {};
    vm->rotation.z = 0;
    // vm->run();
    vm->update();
    vm->mode_of_create_child = 2;
    return AnmManager::insert_in_world_list_front(vm);
}

AnmID AnmFile::createEffectPos(int id, float rot, glm::vec3 const& pos,
                               int layer, AnmVM** out_vm) {
    some_ctr++;
    auto vm = AnmManager::allocate_vm();
    if (out_vm) {
        *out_vm = vm;
    }
    this->copyFromLoaded(vm, id);
    vm->bitflags.randomMode = 1;
    if (layer >= 0) {
        vm->layer = layer;
        if (layer < 0x18) {
            vm->bitflags.originMode = 0b01;
        }
    }
    vm->entity_pos = pos;
    vm->rotation.z = rot;
    vm->run();
    vm->mode_of_create_child = 0;
    return AnmManager::insert_in_world_list_back(vm);
}

void AnmFile::load_external_vm(AnmVM* vm, int id) {
    if (this->scripts[id] == 0x0 /*|| this->load_wait != 0*/) {
        memset(reinterpret_cast<char*>(vm), 0x0, sizeof(AnmVM));
        return;
    }
    // vm->field_0x49c = DX
    vm->anm_loaded_index = slot;
    vm->script_id = id;
    vm->instr_offset = 0;
    vm->bitflags.f530_19 = 0;
    vm->bitflags.f530_20 = 0;
    vm->bitflags.visible = 0;
    vm->bitflags.f534_14_15 = 0b10;
    // vm->__timer_1c.reset();
    vm->__timer_1c = 0;
    // vm->time_in_script.reset();
    vm->time_in_script = 0;
    // vm->run();
    vm->update();
    // AnmManager::field_0xc0++;
    if (vm->bitflags.f534_14_15 != 0b10)
        vm->bitflags.f534_14_15 = 0b01;
}

AnmID AnmFile::spawnVMExt(int id, float rot, glm::vec3 const& pos, int mode,
                         int layer, AnmVM** out_vm) {
    some_ctr++;
    auto vm = AnmManager::allocate_vm();
    if (out_vm) *out_vm = vm;
    copyFromLoaded(vm, id);
    vm->bitflags.randomMode = 1;
    if (layer >= 0) {
        vm->layer = layer;
        if (layer < 0x18) {
            vm->bitflags.originMode = 0b01;
        }
    }
    if (!(mode & 8) && pos == glm::vec3{0, 0, 0}) {
        vm->entity_pos = {};
    } else {
        if (mode & 1) {
            vm->entity_pos = pos + glm::vec3(320, 16, 0);
        } else {
            vm->entity_pos = pos;
        }
    }
    vm->rotation.z = rot;
    vm->update();
    vm->mode_of_create_child = mode;
    AnmID ret;
    if ((mode & 6) == 6) {
        ret = AnmManager::insert_in_ui_list_front(vm);
        vm->bitflags.f534_14_15 = 0;
    } else if (mode & 4) {
        ret = AnmManager::insert_in_ui_list_back(vm);
        vm->bitflags.f534_14_15 = 0;
    } else if (mode & 2) {
        ret = AnmManager::insert_in_world_list_front(vm);
    } else {
        ret = AnmManager::insert_in_world_list_back(vm);
    }
    return ret;
}

AnmID AnmFile::new_vm_ui_back(int id, AnmVM** out_vm) {
    some_ctr++;
    auto vm = AnmManager::allocate_vm();
    if (out_vm) *out_vm = vm;
    copyFromLoaded(vm, id);
    vm->bitflags.randomMode = 1;
    vm->entity_pos = {};
    vm->rotation.z = 0;
    vm->update();
    vm->mode_of_create_child = 4;
    auto anm_id = AnmManager::insert_in_ui_list_back(vm);
    vm->bitflags.f534_14_15 = 0;
    return anm_id;
}


AnmID AnmFile::create_child_vm(int id, AnmVM *parent, int mode) {
    some_ctr++;
    auto new_vm = AnmManager::allocate_vm();
    new_vm->bitflags.randomMode = 1;
    new_vm->layer = parent->layer;
    new_vm->entity_pos = {};
    copyFromLoaded(new_vm, id);
    new_vm->parent_vm = parent;
    new_vm->bitflags.colorizeChildren =
        parent->bitflags.colorizeChildren;
    new_vm->__root_vm__or_maybe_not = parent;
    if (parent->__root_vm__or_maybe_not)
        new_vm->__root_vm__or_maybe_not =
            parent->__root_vm__or_maybe_not;
    new_vm->update();
    new_vm->mode_of_create_child = mode;
    AnmID ret;
    if ((mode & 6) == 6) {
        ret = AnmManager::insert_in_ui_list_front(new_vm);
    } else if (mode & 4) {
        ret = AnmManager::insert_in_ui_list_back(new_vm);
    } else if (mode & 2) {
        ret = AnmManager::insert_in_world_list_front(new_vm);
    } else {
        ret = AnmManager::insert_in_world_list_back(new_vm);
    }

    if (parent->list_of_children.next) {
        new_vm->__node_as_child.next = parent->list_of_children.next;
        parent->list_of_children.next->previous = &new_vm->__node_as_child;
    }
    parent->list_of_children.next = &new_vm->__node_as_child;
    new_vm->__node_as_child.previous = &parent->list_of_children;
    return ret;
}

AnmID AnmFile::new_root(int id, AnmVM *parent) {
    some_ctr++;
    auto new_vm = AnmManager::allocate_vm();
    copyFromLoaded(new_vm, id);
    new_vm->bitflags.randomMode = 1;
    new_vm->layer = parent->layer;
    new_vm->bitflags.colorizeChildren = parent->bitflags.colorizeChildren;
    new_vm->entity_pos = parent->entity_pos;
    new_vm->rotation = parent->rotation;
    new_vm->__pos_2 = parent->pos;
    new_vm->update();
    new_vm->mode_of_create_child = 0;
    return AnmManager::insert_in_world_list_back(new_vm);
}
