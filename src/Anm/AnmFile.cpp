#include "./AnmFile.h"
#include "./AnmFuncs.h"
#include "./AnmManager.h"
#include "./anmOpener.h"
#include <Texture.hpp>
#include <cstring>
#include <numeric>
#include <logger.h>
#include <memory.h>

namespace anm {


// Forward declarations of private functions:

VM *allocate_vm();
ID insert_in_world_list_back(VM *vm);
ID insert_in_world_list_front(VM *vm);
ID insert_in_ui_list_back(VM *vm);
ID insert_in_ui_list_front(VM *vm);
void delete_of_file(File *f);




void Sprite::genTexCoords(f32 tw, f32 th) {
    u1 = x / tw;
    v1 = y / th;
    u2 = (x + w) / tw;
    v2 = (y + h) / th;
}

File::File(cstr filename, u32 slot) {
    Open(filename, slot);
}

void File::Open(cstr filename, u32 slot) {
    if (name != "") Cleanup();
    name = filename;
    opener::anm_archive_t* archive = opener::anm_read_file(filename);
    u32 scrID = 0;
    i32 at_tex_i = 0;
    i32 atr_tex_i = 0;
    for (auto entry : archive->entries) {
        ns::Texture *tex;
        std::string name = entry->name;
        int i = 0;
        while (textures.find(name) != textures.end()) {
            name = entry->name + std::to_string(i);
            i += 1;
        } 
        if (entry->header->hasdata) {
            opener::image_t img = opener::anm_get_image(entry);
            tex = ns::construct<ns::Texture, ns::MemTag::TEXTURE>(img.width, img.height, img.data);
            delete[] img.data;
        } else if (entry->name[0] == '@') {
            u32 w = entry->header->w;
            u32 h = entry->header->h;
            if (entry->name[1] == 'R') {
                w = BACK_BUFFER_SIZE.x;
                h = BACK_BUFFER_SIZE.y;
                name += std::to_string(atr_tex_i++);
            } else {
                name += std::to_string(at_tex_i++);
            }
            tex = ns::Texture::as_framebuffer(w, h);
        } else {
            tex = nullptr;
        }
        textures.insert(std::pair<std::string, ns::Texture*>(name, tex));
        i32 w = tex->get_width();
        i32 h = tex->get_height();
        for (auto spr : entry->sprites) {
            sprites.push_back({ tex, spr->x + entry->header->x, spr->y +
                entry->header->y, spr->w, spr->h, 0, 0, 0, 0 });
            sprites.back().genTexCoords(w, h);
        }
    }
    for (auto entry : archive->entries) {
        for (u32 scrid = 0; scrid < entry->scripts.size(); scrid++) {
            // generate data
            u32 size = std::accumulate(
                entry->scripts[scrid]->instrs.begin(),
                entry->scripts[scrid]->instrs.end(), 0,
                [](u32 a, auto b) { return a + b->length; });
            //            for (auto i : entry->scripts[scrid]->instrs)
            // size += i->length;
            scripts.push_back(new byte[size + 2]);
            u32 off = 0;
            for (auto ins : entry->scripts[scrid]->instrs) {
                for (u32 i = 0; i < ins->length; i++) {
                    scripts.back()[off++] = reinterpret_cast<int8_t*>(ins)[i];
                }
            }
            scripts.back()[size + 0] = -1;
            scripts.back()[size + 1] = -1;
            preloaded.emplace_back(scrID++, slot);
        }
    }

    opener::anm_free(archive);
    NS_INFO("Opened: %s on slot %d", filename, slot);
    this->slot = slot;
}

void File::Cleanup() {
    if (name == "") return;
    anm::delete_of_file(this);
    for (auto s : this->scripts)
        delete[] s;
    for (auto tex : textures) {
        ns::destroy<ns::MemTag::TEXTURE>(tex.second);
    }
    preloaded.clear();
    this->scripts.clear();
    sprites.clear();
    textures.clear();
    name = "";
}

File::~File() {
    Cleanup();
}

bytes File::getScript(usize id) const {
    if (scripts.size() == 0)
        return {};
    return scripts[id % scripts.size()];
}

VM File::getPreloaded(usize id) const {
    if (preloaded.size() == 0)
        return {};
    return preloaded[id % preloaded.size()];
}

Sprite File::getSprite(usize id) const {
    if (sprites.size() == 0)
        return {};
    return sprites[id % sprites.size()];
}

ns::Texture* File::getTextureFromName(cstr name) const {
    return textures.at(name);
}

void File::setSprite(VM* vm, usize sprite_id) {
    if (vm->index_of_sprite_mapping_func &&
        Funcs::on_sprite_set(vm->index_of_sprite_mapping_func))
        sprite_id = Funcs::on_sprite_set(vm->index_of_sprite_mapping_func)
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
    
    vm->__matrix_1 = ns::mat4::mk_scale(ns::vec3(vm->sprite_size / 256.f, 1.f));
    vm->__matrix_3 = ns::mat4::mk_scale(ns::vec3(s.u2 - s.u1, s.v2 - s.v1, 1.f));
    vm->__matrix_2 = vm->__matrix_1;
    return;
}

void File::copyFromLoaded(VM* vm, i32 id) {
    if (static_cast<usize>(id) >= scripts.size() || !vm) return;
    auto saved_entitypos = vm->entity_pos;
    auto saved_fast_id = vm->fast_id;
    auto saved_layer = vm->layer;
    ns::mem_zero(&vm->id, sizeof(VM) - offsetof(VM, id));
    vm->entity_pos = saved_entitypos;
    vm->fast_id = saved_fast_id;
    vm->layer = saved_layer;
    vm->node_in_global_list = {vm, nullptr, nullptr};
    vm->__node_as_child = {vm, nullptr, nullptr};
    vm->list_of_children = {vm, nullptr, nullptr};
    if (id >= 0)
        ns::mem_copy(vm, &this->preloaded[id], offsetof(VM, id));
    vm->time_in_script = 0;
    vm->__timer_1c = 0;
}

ID File::createEffect(i32 id, i32 layer, VM** output) {
    some_ctr++;
    auto vm = anm::allocate_vm();
    if (output) *output = vm;
    copyFromLoaded(vm, id);
    vm->bitflags.randomMode = 1;
    if (layer >= 0) {
        vm->layer = layer;
        if (layer <= 0x17) {
            vm->bitflags.originMode = 1;
        }
    }
    vm->entity_pos = ns::vec3();
    vm->rotation.z = 0;
    // vm->run();
    vm->update();
    vm->mode_of_create_child = 0;
    return anm::insert_in_world_list_back(vm);
}

ID File::createEffectFront(i32 id, i32 layer, VM** output) {
    some_ctr++;
    auto vm = anm::allocate_vm();
    if (output) *output = vm;
    copyFromLoaded(vm, id);
    vm->bitflags.randomMode = 1;
    if (layer >= 0) {
        vm->layer = layer;
        if (layer <= 0x17) {
            vm->bitflags.originMode = 1;
        }
    }
    vm->entity_pos = ns::vec3();
    vm->rotation.z = 0;
    // vm->run();
    vm->update();
    vm->mode_of_create_child = 2;
    return anm::insert_in_world_list_front(vm);
}

ID File::createEffectPos(i32 id, f32 rot, ns::vec3 const& pos,
                               i32 layer, VM** out_vm) {
    some_ctr++;
    auto vm = anm::allocate_vm();
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
    return anm::insert_in_world_list_back(vm);
}

void File::load_external_vm(VM* vm, i32 id) {
    if (this->scripts[id] == 0x0 /*|| this->load_wait != 0*/) {
        ns::mem_zero(vm, sizeof(VM));
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
    // anm::field_0xc0++;
    if (vm->bitflags.f534_14_15 != 0b10)
        vm->bitflags.f534_14_15 = 0b01;
}

ID File::spawnVMExt(i32 id, f32 rot, ns::vec3 const& pos, i32 mode,
                         i32 layer, VM** out_vm) {
    some_ctr++;
    auto vm = anm::allocate_vm();
    if (out_vm) *out_vm = vm;
    copyFromLoaded(vm, id);
    vm->bitflags.randomMode = 1;
    if (layer >= 0) {
        vm->layer = layer;
        if (layer < 0x18) {
            vm->bitflags.originMode = 0b01;
        }
    }
    if (!(mode & 8) && pos == ns::vec3{0, 0, 0}) {
        vm->entity_pos = ns::vec3();
    } else {
        if (mode & 1) {
            vm->entity_pos = pos + ns::vec3(320, 16, 0);
        } else {
            vm->entity_pos = pos;
        }
    }
    vm->rotation.z = rot;
    vm->update();
    vm->mode_of_create_child = mode;
    ID ret;
    if ((mode & 6) == 6) {
        ret = anm::insert_in_ui_list_front(vm);
        vm->bitflags.f534_14_15 = 0;
    } else if (mode & 4) {
        ret = anm::insert_in_ui_list_back(vm);
        vm->bitflags.f534_14_15 = 0;
    } else if (mode & 2) {
        ret = anm::insert_in_world_list_front(vm);
    } else {
        ret = anm::insert_in_world_list_back(vm);
    }
    return ret;
}

ID File::new_vm_ui_back(i32 id, VM** out_vm) {
    some_ctr++;
    auto vm = anm::allocate_vm();
    if (out_vm) *out_vm = vm;
    copyFromLoaded(vm, id);
    vm->bitflags.randomMode = 1;
    vm->entity_pos = ns::vec3();
    vm->rotation.z = 0;
    vm->update();
    vm->mode_of_create_child = 4;
    auto anm_id = anm::insert_in_ui_list_back(vm);
    vm->bitflags.f534_14_15 = 0;
    return anm_id;
}


ID File::create_child_vm(i32 id, VM *parent, i32 mode) {
    some_ctr++;
    auto new_vm = anm::allocate_vm();
    new_vm->bitflags.randomMode = 1;
    new_vm->layer = parent->layer;
    new_vm->entity_pos = ns::vec3();
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
    ID ret;
    if ((mode & 6) == 6) {
        ret = anm::insert_in_ui_list_front(new_vm);
    } else if (mode & 4) {
        ret = anm::insert_in_ui_list_back(new_vm);
    } else if (mode & 2) {
        ret = anm::insert_in_world_list_front(new_vm);
    } else {
        ret = anm::insert_in_world_list_back(new_vm);
    }

    if (parent->list_of_children.next) {
        new_vm->__node_as_child.next = parent->list_of_children.next;
        parent->list_of_children.next->previous = &new_vm->__node_as_child;
    }
    parent->list_of_children.next = &new_vm->__node_as_child;
    new_vm->__node_as_child.previous = &parent->list_of_children;
    return ret;
}

ID File::new_root(i32 id, VM *parent) {
    some_ctr++;
    auto new_vm = anm::allocate_vm();
    copyFromLoaded(new_vm, id);
    new_vm->bitflags.randomMode = 1;
    new_vm->layer = parent->layer;
    new_vm->bitflags.colorizeChildren = parent->bitflags.colorizeChildren;
    new_vm->entity_pos = parent->entity_pos;
    new_vm->rotation = parent->rotation;
    new_vm->__pos_2 = parent->pos;
    new_vm->update();
    new_vm->mode_of_create_child = 0;
    return anm::insert_in_world_list_back(new_vm);
}

} // namespace anm
