#include "./AnmManager.h"
#include "./AnmBitflags.h"
#include "./AnmFuncs.h"
#include "./AnmShader.h"
#include "./BlitShader.h"
#include "./FogShader.h"
#include "CopyTextures.hpp"
#include "Text.hpp"
#include <DrawBatch.h>
#include <DrawFuncs.h>
#include <cstring>
#include <fstream>
#include <logger.h>
#include <memory.h>
#include <GL/glew.h>

#include <cmath> // round

anm::Stats ANM_MGR_STATS;
static void reset_stats() {
  ANM_MGR_STATS.n_flush_vbos = 0;
  ANM_MGR_STATS.n_total_initiated_draws = 0;
  ANM_MGR_STATS.n_vm_drawn_world = 0;
  ANM_MGR_STATS.n_vm_drawn_ui = 0;
  ANM_MGR_STATS.n_vm_drawn = 0;
  ANM_MGR_STATS.n_vm_ticked_world = 0;
  ANM_MGR_STATS.n_vm_ticked_ui = 0;
  ANM_MGR_STATS.n_vm_ticked = 0;
}

namespace anm {

// Forward declarations for private functions
void draw_vm__modes_0_1_2_3(VM *vm, i32 i);
i32 draw_vm__mode_6(VM *vm);
void draw_vm__mode_8_15(VM *vm);
void draw_vm_triangle_fan(VM *vm, RenderVertex_t *vertexData, i32 nVert);
i32 draw_vm__ins_603(f32 x, f32 y, f32 width, f32 height, f32 rot_z,
                     ns::Color col1, ns::Color col2, i32 anchorX, i32 anchorY);
i32 draw_vm__ins_607(f32 x, f32 y, f32 width, f32 height, f32 rot_z,
                     ns::Color col1, ns::Color col2, i32 anchorX, i32 anchorY);
i32 draw_vm__ins_612(f32 x, f32 y, f32 width, f32 height, f32 rot_z,
                     ns::Color col1, ns::Color col2, i32 anchorX, i32 anchorY);
i32 draw_vm__ins_613(f32 x, f32 y, f32 width, f32 rot_z, ns::Color col1,
                     ns::Color col2, i32 anchor);
i32 draw_vm__mode_19__drawRing(f32 x, f32 y, f32 angle_0, f32 radius,
                               f32 thickness, i32 vertex_count, ns::Color col);
i32 draw_vm__mode_18__drawCircleBorder(f32 x, f32 y, f32 angle_0, f32 radius,
                                       i32 nb_vertex, ns::Color col);
i32 draw_vm__mode_17__drawCircle(f32 x, f32 y, f32 angle_0, f32 radius,
                                 i32 vertex_count, ns::Color color_1,
                                 ns::Color color_2);
void draw_vm_mode_24_25(VM *vm, void *buff, i32 cnt);

void calc_mat_world(VM *vm);
void setup_render_state_for_vm(VM *vm);
void prepare_vm_for_delete(VM *vm, VM::List_t *list);
i32 destroy_possibly_managed_vm(VM *vm);

f32 RESOLUTION_MULT = 1.f;
ns::vec2 BACK_BUFFER_SIZE = {0, 0};

struct TableAnm508_t {
  i16 anm_loaded_index = -1;
  i16 script_index = 0;
  i32 index_of_on_create = 0;
  i32 index_of_on_tick = 0;
  i32 index_of_on_draw = 0;
  i32 index_of_on_destroy = 0;
  i32 index_of_on_interrupt = 0;
  i32 index_of_on_copy_1__disused = 0;
  i32 index_of_on_copy_2__disused = 0;
};

struct AnmManagerState {
  // zThread
  // zAnmSaveRelated[4] pause_related
  // undefined4[2]
  i32 processed_anm_ctr_0xc8 = 0;
  i32 draw_call_ctr_0xcc = 0;
  f32 cam_vec2_fc_x = 0.f;
  f32 cam_vec2_fc_y = 0.f;
  i32 render_loop_ctr_0xd8 = 0;
  VM __anm_vm_dc{};
  VM::List_t *world_list_head = nullptr;
  VM::List_t *world_list_tail = nullptr;
  VM::List_t *ui_list_head = nullptr;
  VM::List_t *ui_list_tail = nullptr;
  FastVM fastArray[8191]{};
  i32 __lolk_next_snapshot_fast_id = 0;
  i32 __lolk_next_snapshot_discriminator = 0;
  VM::List_t __lolk_vm_snapshot_list_head{};
  FastVM::List_t freelist_head{};
  // undefined4
  std::array<File, 32> loadedFiles;
  ns::mat4 __matrix_186017c = ns::mat4(1.f);
  VM __anm_vm_18601bc{};
  i32 field_0x18607bc = 0;
  ns::Color field_0x18607c0 = {0, 0, 0, 0};
  u32 last_used_texture = 0;
  u8 last_used_blendmode = 0;
  u8 field_0x18607c9 = 0;
  u8 field_0x18607ca = 0;
  u8 field_0x18607cb = 0;
  u8 field_0x18607cc = 0;
  u8 field_0x18607cd = 0;
  u8 last_used_resamplemode = 0;
  u8 field_0x18607cf = 0;
  u8 last_used_scrollmodex = 0;
  u8 last_used_scrollmodey = 0;
  // undefined2
  f32 some_positions[22]{};
  // dummy head (unused)
  i32 last_id_discriminator = 1;
  ns::Color custom_color_1c90a48{};
  i32 use_custom_color_1c90a4c = false;

  bool initialized = false;
  ns::DrawBatch *batch{};
  BaseShader *shader = nullptr;
  BlitShader *bshader = nullptr;
  FogShader *fshader = nullptr;
  ns::ShaderProgram *curr_shader = nullptr;

  u8 zwrite_enabled = false;
  u8 ztest_enabled = false;
  u8 fog_enabled = false;
  ns::ShaderProgram *last_shader_before_no_atest_save = nullptr;
  Camera_t *current_camera = nullptr;
  Camera_t *_3d_camera = nullptr;
  ns::vec2 origins[4] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};

  std::vector<TableAnm508_t> effect_table{};
};

static AnmManagerState *AM;






void ListIter::next() {
  if (this->it_ptr == nullptr) return;
  VM::List_t* ptr = reinterpret_cast<VM::List_t*>(this->it_ptr);
  ptr = ptr->next;
  this->it_ptr = reinterpret_cast<void*>(ptr);
}

VM* ListIter::get() {
  if (this->it_ptr == nullptr) return nullptr;
  VM::List_t* ptr = reinterpret_cast<VM::List_t*>(this->it_ptr);
  return ptr->value;
}

ListIter iter_world_list() {
  return ListIter { reinterpret_cast<void*>(AM->world_list_head) };
}







void get_stats(Stats &stats) { stats = ANM_MGR_STATS; }

// expose these functions ?
static void anm_use_texture(u32 tex_id) {
  if (AM->last_used_texture != tex_id) {
    AM->last_used_texture = tex_id;
    flush_vbos();
    AM->last_used_scrollmodex = 255;
    AM->last_used_scrollmodey = 255;
    AM->last_used_resamplemode = 255;
    // SUPERVISOR.d3d_device->SetTexture(0, tex_id);
    glBindTexture(GL_TEXTURE_2D, AM->last_used_texture);
  }
}

inline void anm_use_texture(ns::Texture *tex) {
  anm_use_texture(tex->get_opengl_id());
}

inline void anm_use_texture(VM *vm) {
  anm_use_texture(vm->getSprite().texture);
}

static void anm_use_default_texture() {
  if (AM->last_used_texture != 1) {
    AM->last_used_texture = 1;
    flush_vbos();
    glBindTexture(GL_TEXTURE_2D, 1);
  }
}

void init() {
  AM = ns::construct<AnmManagerState>();
  for (usize i = 0; i < 8191; i++) {
    FastVM *fvm = &AM->fastArray[i];
    fvm->vm.reset();
    fvm->fastID = i;
    fvm->vm.fast_id = i;
    fvm->isAlive = false;
    fvm->freelistNode.value = fvm;
    if (AM->freelist_head.next)
      AM->freelist_head.next->previous = &fvm->freelistNode;
    fvm->freelistNode.next = AM->freelist_head.next;
    AM->freelist_head.next = &fvm->freelistNode;
    fvm->freelistNode.previous = &AM->freelist_head;
  }

  AM->some_positions[1] = 0.0;
  AM->last_used_texture = -1;
  AM->last_used_blendmode = 0;
  AM->field_0x18607c9 = 0;
  AM->field_0x18607ca = 0;
  AM->field_0x18607cb = 0;
  AM->field_0x18607c0 = {0, 0, 0, 1};
  AM->field_0x18607cc = 255;
  // AM->pause_related[0].anm_loaded = -1;
  // AM->pause_related[1].anm_loaded = -1;
  // AM->pause_related[2].anm_loaded = -1;
  // AM->pause_related[3].anm_loaded = -1;

  // setup_vao();
  AM->batch = ns::construct<ns::DrawBatch>();
  AM->shader = ns::construct<BaseShader>();
  AM->bshader = ns::construct<BlitShader>();
  AM->fshader = ns::construct<FogShader>();
  AM->curr_shader = AM->shader;

  // zFloat3_00529ecc.x = 1.0;
  // zFloat3_00529ecc.y = 0.0;
  // zFloat3_00529ecc.z = 0.0;
  // zFloat3_00529ee4.x = 1.0;
  // zFloat3_00529ee4.y = 1.0;
  // zFloat3_00529ee4.z = 0.0;
  // zFloat3_00529efc.x = 1.0;
  // zFloat3_00529efc.y = 0.0;
  // zFloat3_00529efc.z = 1.0;
  // zFloat3_00529f14.x = 1.0;
  // zFloat3_00529f14.y = 1.0;
  // zFloat3_00529f14.z = 1.0;
  SPRITE_TEMP_BUFFER[0].transformed_pos.w = 1.0;
  SPRITE_TEMP_BUFFER[0].texture_uv.x = 0.0;
  SPRITE_TEMP_BUFFER[0].texture_uv.y = 0.0;
  SPRITE_TEMP_BUFFER[1].transformed_pos.w = 1.0;
  SPRITE_TEMP_BUFFER[1].texture_uv.x = 1.0;
  SPRITE_TEMP_BUFFER[1].texture_uv.y = 0.0;
  SPRITE_TEMP_BUFFER[2].transformed_pos.w = 1.0;
  SPRITE_TEMP_BUFFER[2].texture_uv.x = 0.0;
  SPRITE_TEMP_BUFFER[2].texture_uv.y = 1.0;
  SPRITE_TEMP_BUFFER[3].transformed_pos.w = 1.0;
  SPRITE_TEMP_BUFFER[3].texture_uv.x = 1.0;
  SPRITE_TEMP_BUFFER[3].texture_uv.y = 1.0;

  text::init();
  cptex::init();
}

void cleanup() {
  cptex::cleanup();
  text::cleanup();
  /* DESTROY HEAP VMS IN WORLD LIST */
  while (AM->world_list_head) {
    auto n = AM->world_list_head;
    AM->world_list_head = AM->world_list_head->next;
    if ((n->value->id.val & ID::fast_id_mask) == ID::fast_id_mask) {
      ANM_MGR_STATS.n_non_fast_vm_alive--;
      delete n->value;
    }
  }
  AM->world_list_tail = nullptr;

  /* DESTROY HEAP VMS IN UI LIST */
  while (AM->ui_list_head) {
    auto n = AM->ui_list_head;
    AM->ui_list_head = AM->ui_list_head->next;
    if ((n->value->id.val & ID::fast_id_mask) == ID::fast_id_mask) {
      ANM_MGR_STATS.n_non_fast_vm_alive--;
      delete n->value;
    }
  }
  AM->ui_list_tail = nullptr;

  /* CLEAN PREINIT VM */
  for (auto &f : AM->loadedFiles)
    f.Cleanup();

  ns::destroy(AM->batch);
  ns::destroy(AM->shader);
  ns::destroy(AM->bshader);
  ns::destroy(AM->fshader);

  ns::destroy(AM);
}

VM *allocate_vm() {
  if (AM->freelist_head.next) {
    auto fvm = AM->freelist_head.next->value;
    if (fvm->freelistNode.next) {
      fvm->freelistNode.next->previous = fvm->freelistNode.previous;
    }
    if (fvm->freelistNode.previous) {
      fvm->freelistNode.previous->next = fvm->freelistNode.next;
    }
    fvm->freelistNode.next = nullptr;
    fvm->freelistNode.previous = nullptr;
    fvm->vm.fast_id = fvm->fastID;
    fvm->isAlive = true;
    fvm->vm.__root_vm__or_maybe_not = nullptr;
    fvm->vm.parent_vm = nullptr;
    fvm->vm.node_in_global_list = {&fvm->vm, nullptr, nullptr};
    fvm->vm.__node_as_child = {&fvm->vm, nullptr, nullptr};
    fvm->vm.list_of_children = {&fvm->vm, nullptr, nullptr};
    return &fvm->vm;
  }
  ANM_MGR_STATS.n_non_fast_vm_alive++;
  auto vm = new VM();
  vm->sprite_id = -1;
  vm->instr_offset = -1;
  vm->reset();
  vm->fast_id = 0x3fff;
  return vm;
}

ID insert_in_world_list_back(VM *vm) {
  vm->node_in_global_list = {vm, nullptr, nullptr};
  if (!AM->world_list_head) {
    AM->world_list_head = &vm->node_in_global_list;
  } else {
    AM->world_list_tail->next = &vm->node_in_global_list;
    vm->node_in_global_list.previous = AM->world_list_tail;
  }
  AM->world_list_tail = &vm->node_in_global_list;

  AM->last_id_discriminator++;
  AM->last_id_discriminator &= 0x3ffff;
  if (AM->last_id_discriminator == 0) {
    AM->last_id_discriminator = 1;
  }
  vm->id.discriminator = AM->last_id_discriminator;
  vm->id.fast_id = vm->fast_id;
  return vm->id;
}

ID insert_in_world_list_front(VM *vm) {
  vm->node_in_global_list = {vm, nullptr, nullptr};
  if (!AM->world_list_head) {
    AM->world_list_tail = &vm->node_in_global_list;
  } else {
    AM->world_list_head->previous = &vm->node_in_global_list;
    vm->node_in_global_list.next = AM->world_list_head;
  }
  AM->world_list_head = &vm->node_in_global_list;

  AM->last_id_discriminator++;
  AM->last_id_discriminator &= 0x3ffff;
  if (AM->last_id_discriminator == 0) {
    AM->last_id_discriminator = 1;
  }
  vm->id.discriminator = AM->last_id_discriminator;
  vm->id.fast_id = vm->fast_id;
  return vm->id;
}

ID insert_in_ui_list_back(VM *vm) {
  vm->node_in_global_list = {vm, nullptr, nullptr};
  if (!AM->ui_list_head) {
    AM->ui_list_head = &vm->node_in_global_list;
  } else {
    AM->ui_list_tail->next = &vm->node_in_global_list;
    vm->node_in_global_list.previous = AM->ui_list_tail;
  }
  AM->ui_list_tail = &vm->node_in_global_list;

  AM->last_id_discriminator++;
  AM->last_id_discriminator &= 0x3ffff;
  if (AM->last_id_discriminator == 0) {
    AM->last_id_discriminator = 1;
  }
  vm->id.discriminator = AM->last_id_discriminator;
  vm->id.fast_id = vm->fast_id;
  return vm->id;
}

ID insert_in_ui_list_front(VM *vm) {
  vm->node_in_global_list = {vm, nullptr, nullptr};
  if (!AM->ui_list_head) {
    AM->ui_list_tail = &vm->node_in_global_list;
  } else {
    AM->ui_list_head->previous = &vm->node_in_global_list;
    vm->node_in_global_list.next = AM->ui_list_head;
  }
  AM->ui_list_head = &vm->node_in_global_list;

  AM->last_id_discriminator++;
  AM->last_id_discriminator &= 0x3ffff;
  if (AM->last_id_discriminator == 0) {
    AM->last_id_discriminator = 1;
  }
  vm->id.discriminator = AM->last_id_discriminator;
  vm->id.fast_id = vm->fast_id;
  return vm->id;
}

void recreate_vm(ID &id, i32 new_script) {
  auto vm = get_vm(id);
  if (vm) {
    vm->bitflags.visible = false;
    vm->instr_offset = -1;
    id = AM->loadedFiles[vm->anm_loaded_index].createEffect(new_script);
  }
}

void delete_vm(ID id) {
  if (!id)
    return;
  delete_vm(get_vm(id));
}

void delete_vm(VM *vm) {
  if (!vm)
    return;
  if (vm->bitflags.f534_26) {
    return;
  }
  if (vm->script_id == 99 && (vm->list_of_children.next == nullptr || vm->list_of_children.next->next == nullptr) && vm->anm_loaded_index == 10) {
    NS_DEBUG("BUG HERE: anm 10:99 should have 2 children");
  }
  vm->bitflags.activeFlags = 0b01;
  for (auto node = vm->list_of_children.next; node; node = node->next) {
    delete_vm(node->value);
  }
}

void delete_of_file(File *f) {
  if (!f)
    return;
  i32 id = -1;
  for (i32 i = 0; i < 32; i++) {
    if (&AM->loadedFiles[i] == f) {
      id = i;
      break;
    }
  }
  if (id == -1)
    return;
  for (auto node = AM->world_list_head; node; node = node->next)
      if (node->value->anm_loaded_index == id)
          delete_vm(node->value);
}

bool is_alive(ID id) { return get_vm(id) != nullptr; }

VM *get_vm(ID id) {
  if (id.val == 0)
    return nullptr;
  if (id.fast_id == ID::fast_id_mask) {
    for (auto node = AM->world_list_head; node; node = node->next)
      if (node->value->id == id)
        return node->value;
    for (auto node = AM->ui_list_head; node; node = node->next)
      if (node->value->id == id)
        return node->value;
    return nullptr;
  }
  if (AM->fastArray[id.fast_id].vm.id != id)
    return nullptr;
  if (AM->fastArray[id.fast_id].isAlive)
    return &(AM->fastArray[id.fast_id].vm);
  return nullptr;
}

File *loadFile(usize slot, cstr filename) {
  std::ifstream ifile;
  std::string fname = filename;
  if (filename[fname.size() - 4] != '.' || filename[fname.size() - 3] != 'a' ||
      filename[fname.size() - 2] != 'n' || filename[fname.size() - 1] != 'm') {
    fname += ".anm";
  }
  ifile.open(fname);
  if (!ifile) {
    NS_ERROR("%s: No such file", fname.c_str());
    return nullptr;
  }
  ifile.close();
  if (AM->loadedFiles[slot].name != "") { /* do something */
    AM->loadedFiles[slot].Cleanup();
  }
  AM->loadedFiles[slot].Open(filename, slot);
  return &AM->loadedFiles[slot];
}

void prepare_vm_for_delete(VM *vm, VM::List_t *list) {
  for (auto node = vm->list_of_children.next; node; node = node->next) {
    prepare_vm_for_delete(node->value, list);
  }
  if (vm->bitflags.activeFlags != 0b10) {
    vm->__wierd_list = {vm, nullptr, nullptr};
    if (list->next != 0) {
      vm->__wierd_list.next = list->next;
      list->next->previous = &vm->__wierd_list;
    }
    list->next = &vm->__wierd_list;
    vm->__wierd_list.previous = list;
  }
  vm->__root_vm__or_maybe_not = nullptr;
  vm->parent_vm = nullptr;
  vm->bitflags.activeFlags = 0b10;
  vm->new_field_added_in_1_00b = 0;
  vm->next_in_layer__disused = nullptr;
  if (vm->__node_as_child.next) {
    vm->__node_as_child.next->previous = vm->__node_as_child.previous;
  }
  if (vm->__node_as_child.previous) {
    vm->__node_as_child.previous->next = vm->__node_as_child.next;
  }
  vm->__node_as_child.previous = nullptr;
  vm->__node_as_child.next = nullptr;
}

i32 destroy_possibly_managed_vm(VM *vm) {
  if (&vm->node_in_global_list == AM->world_list_tail) {
    AM->world_list_tail = vm->node_in_global_list.previous;
  }
  if (&vm->node_in_global_list == AM->world_list_head) {
    AM->world_list_head = vm->node_in_global_list.next;
  }
  if (&vm->node_in_global_list == AM->ui_list_tail) {
    AM->ui_list_tail = vm->node_in_global_list.previous;
  }
  if (&vm->node_in_global_list == AM->ui_list_head) {
    AM->ui_list_head = vm->node_in_global_list.next;
  }
  if (vm->index_of_on_destroy && Funcs::on_destroy(vm->index_of_on_destroy)) {
    Funcs::on_destroy(vm->index_of_on_destroy)(vm);
  }
  if (vm->node_in_global_list.next) {
    vm->node_in_global_list.next->previous = vm->node_in_global_list.previous;
  }
  if (vm->node_in_global_list.previous) {
    vm->node_in_global_list.previous->next = vm->node_in_global_list.next;
  }
  vm->node_in_global_list.next = nullptr;
  vm->node_in_global_list.previous = nullptr;
  if (vm->__wierd_list.next) {
    vm->__wierd_list.next->previous = vm->__wierd_list.previous;
  }
  if (vm->__wierd_list.previous) {
    vm->__wierd_list.previous->next = vm->__wierd_list.next;
  }
  vm->__wierd_list.next = nullptr;
  vm->__wierd_list.previous = nullptr;
  vm->__root_vm__or_maybe_not = nullptr;
  vm->parent_vm = nullptr;
  // if not in fast array
  // if (fast_array > vm || this + 0x18600d4 <= vm) {
  if (vm->id.fast_id == ID::fast_id_mask) {
    delete vm;
    ANM_MGR_STATS.n_non_fast_vm_alive--;
    return 0;
  }
  AM->fastArray[vm->fast_id].isAlive = false;
  if (AM->freelist_head.next) {
    AM->fastArray[vm->fast_id].freelistNode.next = AM->freelist_head.next;
    AM->freelist_head.next->previous = &AM->fastArray[vm->fast_id].freelistNode;
  }
  AM->freelist_head.next = &AM->fastArray[vm->fast_id].freelistNode;
  AM->fastArray[vm->fast_id].freelistNode.previous = &AM->freelist_head;
  if (vm->special_vertex_buffer_data) {
    ns::free_raw(vm->special_vertex_buffer_data,
                 vm->special_vertex_buffer_size);
  }
  vm->special_vertex_buffer_data = nullptr;
  vm->special_vertex_buffer_size = 0;
  vm->id = 0;
  vm->instr_offset = -1;
  return 0;
}

i32 on_tick_world() {
  // for (i32 i = 0; i < 0x24; i++) {
  //     layer_list_dummy_heads__unused_as_of_1[i].next_in_layer__disused =
  //     nullptr;
  // }

  VM::List_t todelete = {};

  for (auto node = AM->world_list_head; node; node = node->next) {
    ANM_MGR_STATS.n_vm_ticked++;
    ANM_MGR_STATS.n_vm_ticked_world++;
    node->value->new_field_added_in_1_00b = 0;
    node->value->next_in_layer__disused = 0;
    if (node->value->bitflags.activeFlags == 0b01 ||
        (node->value->bitflags.activeFlags == 0b00 && node->value->update())) {
      prepare_vm_for_delete(node->value, &todelete);
    }
  }

  VM::List_t *next = nullptr;
  for (auto node = todelete.next; node; node = next) {
    next = node->next;
    destroy_possibly_managed_vm(node->value);
  }

  return 1;
}

i32 on_tick_ui() {
  // for (i32 i = 0x24; i < 0x2b; i++) {
  //     layer_list_dummy_heads__unused_as_of_1[i].next_in_layer__disused =
  //     nullptr;
  // }
  reset_stats();

  AM->render_loop_ctr_0xd8 = 0;
  VM::List_t todelete = {};

  for (auto node = AM->ui_list_head; node; node = node->next) {
    ANM_MGR_STATS.n_vm_ticked++;
    ANM_MGR_STATS.n_vm_ticked_ui++;
    node->value->new_field_added_in_1_00b = 0;
    node->value->next_in_layer__disused = 0;
    if (node->value->bitflags.activeFlags == 0b01 ||
        (node->value->bitflags.activeFlags == 0b00 && node->value->update())) {
      prepare_vm_for_delete(node->value, &todelete);
    }
  }

  VM::List_t *next = nullptr;
  for (auto node = todelete.next; node; node = next) {
    next = node->next;
    destroy_possibly_managed_vm(node->value);
  }

  return 1;
}

i32 render_layer(u32 layer) {
  for (auto node = AM->world_list_head; node; node = node->next) {
    if (node->value->bitflags.activeFlags)
      continue;
    auto l = node->value->layer;
    if (node->value->layer <= 42 && node->value->layer >= 36) {
      l -= 12;
    }
    if (l == layer) {
      ANM_MGR_STATS.n_vm_drawn++;
      ANM_MGR_STATS.n_vm_drawn_world++;
      draw_vm(node->value);
    }
    AM->render_loop_ctr_0xd8++;
  }
  for (auto node = AM->ui_list_head; node; node = node->next) {
    if (node->value->bitflags.activeFlags)
      continue;
    auto l = node->value->layer;
    if (l <= 31) {
      l += 12;
    } else if (l < 36 || l > 42) {
      l = 38;
    }
    if (l == layer) {
      ANM_MGR_STATS.n_vm_drawn++;
      ANM_MGR_STATS.n_vm_drawn_ui++;
      draw_vm(node->value);
    }
    AM->render_loop_ctr_0xd8++;
  }
  return 1;
}

void flush_vbos() {
  if (AM->batch->is_empty())
    return;
  AM->curr_shader->use();
  AM->batch->submit();
  ANM_MGR_STATS.n_flush_vbos++;
  // if (AM->vertex_buffers.unrendered_sprite_count == 0) return;
  // if (field_0x18607cf != 1) {
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP,
  //   D3DTOP_MODULATE); SUPERVISOR.d3d_device->SetTextureStageState(0,
  //   D3DTSS_COLOROP, D3DTOP_MODULATE);
  //   AM->field_0x18607cf = 1;
  // }
  // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
  // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
  // AM->draw_call_ctr_0xcc++;
  // AM->vertex_buffers.sprite_render_cursor =
  // AM->vertex_buffers.sprite_write_cursor;
  // AM->vertex_buffers.unrendered_sprite_count = 0;
}

// ???: expose these ?
static void anm_use_blendmode(u32 blendmode) {
  if (AM->last_used_blendmode != blendmode) {
    flush_vbos();
    AM->last_used_blendmode = blendmode;
    switch (AM->last_used_blendmode) {
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
}

static void anm_use_resamplemode(u32 resamplemode) {
  // TODO: In GL, this is by texture, so  it should work differently.

  if (AM->last_used_resamplemode != resamplemode) {
    flush_vbos();
    AM->last_used_resamplemode = resamplemode;
    if (AM->last_used_resamplemode == 0) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
  }
}

static void anm_use_scrollmode_x(u32 scrollmode) {
  // TODO: In GL, this is by texture, so  it should work differently.

  if (AM->last_used_scrollmodex != scrollmode) {
    flush_vbos();
    AM->last_used_scrollmodex = scrollmode;
    if (AM->last_used_scrollmodex == 0)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    else if (AM->last_used_scrollmodex == 1)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    else if (AM->last_used_scrollmodex == 2)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  }
}

static void anm_use_scrollmode_y(u32 scrollmode) {
  // TODO: In GL, this is by texture, so  it should work differently.

  if (AM->last_used_scrollmodey != scrollmode) {
    flush_vbos();
    AM->last_used_scrollmodey = scrollmode;
    if (AM->last_used_scrollmodey == 0)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    else if (AM->last_used_scrollmodey == 1)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    else if (AM->last_used_scrollmodey == 2)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  }
}

void setup_render_state_for_vm(VM *vm) {
  anm_use_blendmode(vm->bitflags.blendmode);
  anm_use_resamplemode(vm->bitflags.resampleMode);
  anm_use_scrollmode_x(vm->bitflags.scrollX);
  anm_use_scrollmode_y(vm->bitflags.scrollY);
  AM->processed_anm_ctr_0xc8++;
  return;
}

f32 DAT_00524710 = 0.f;
f32 DAT_00524714 = 448.f;

void calc_mat_world(VM *vm) {
  if (!vm->bitflags.f530_16) {
    vm->__matrix_2 = ns::mat::mk4_scale({vm->scale_2 * vm->scale, 1});
    vm->bitflags.scaled = false;
    if (vm->rotation.x)
      vm->__matrix_2 = ns::mat::mk4_rotate_x(vm->rotation.x);
    if (vm->rotation.y)
      vm->__matrix_2 = ns::mat::mk4_rotate_y(vm->rotation.y);
    if (vm->rotation.z)
      vm->__matrix_2 = ns::mat::mk4_rotate_z(vm->rotation.z);
    vm->bitflags.rotated = false;
  }
  ns::vec3 p = vm->entity_pos + vm->pos + vm->__pos_2;
  if (vm->bitflags.originMode && !vm->parent_vm) {
    p.x += DAT_00524710 * 0.5;
    p.y += (DAT_00524714 - 448.0) * 0.5;
  }
  if (vm->parent_vm && !vm->bitflags.noParent)
    p +=
        vm->parent_vm->entity_pos + vm->parent_vm->pos + vm->parent_vm->__pos_2;
  AM->__matrix_186017c = ns::mat::translate(vm->__matrix_2, p);
}

void draw_vm(VM *vm) {
  ANM_MGR_STATS.n_total_initiated_draws++;
  if (vm->index_of_on_draw && Funcs::on_draw(vm->index_of_on_draw))
    Funcs::on_draw(vm->index_of_on_draw)(vm);

  if (!vm->bitflags.visible || !vm->bitflags.f530_1 || vm->bitflags.activeFlags)
    return;

  // auto saved_a1 = vm->color_1.a;
  // auto saved_a2 = vm->color_2.a;
  // if (vm->bitflags.fadeNearCamera) {
  //   f32 d = ns::point_distance(
  //     {vm->pos + vm->__pos_2 + vm->entity_pos},
  //     SUPERVISOR.current_camera->position);
  //   f32 factor = (d - vm->cam_fade_begin) /
  //   (vm->cam_fade_end-vm->cam_fade_begin); if (factor > 1.f) factor = 1.f; if
  //   (factor < 0.f) factor = 0.f; vm->color_1.a *= factor; vm->color_2.a *=
  //   factor;
  // }

  if (vm->bitflags.rendermode != 8 && vm->bitflags.rendermode != 15 &&
      vm->bitflags.rendermode != 24 && vm->bitflags.rendermode != 25)
    disable_zwrite();

  /* switch render mode */
  switch (vm->bitflags.rendermode) {
  case 0:
    if (vm->color_1.a == 0 && vm->color_2.a == 0)
      break;
    vm->write_sprite_corners__without_rot(
        SPRITE_TEMP_BUFFER[0].transformed_pos,
        SPRITE_TEMP_BUFFER[1].transformed_pos,
        SPRITE_TEMP_BUFFER[2].transformed_pos,
        SPRITE_TEMP_BUFFER[3].transformed_pos);
    draw_vm__modes_0_1_2_3(vm, 1);
    break;
  case 1:
  case 3:
    if (vm->color_1.a == 0 && vm->color_2.a == 0)
      break;
    vm->write_sprite_corners__with_z_rot(SPRITE_TEMP_BUFFER[0].transformed_pos,
                                         SPRITE_TEMP_BUFFER[1].transformed_pos,
                                         SPRITE_TEMP_BUFFER[2].transformed_pos,
                                         SPRITE_TEMP_BUFFER[3].transformed_pos);
    draw_vm__modes_0_1_2_3(vm, 0);
    break;
  case 2:
    if (vm->color_1.a == 0 && vm->color_2.a == 0)
      break;
    vm->write_sprite_corners__without_rot(
        SPRITE_TEMP_BUFFER[0].transformed_pos,
        SPRITE_TEMP_BUFFER[1].transformed_pos,
        SPRITE_TEMP_BUFFER[2].transformed_pos,
        SPRITE_TEMP_BUFFER[3].transformed_pos);
    draw_vm__modes_0_1_2_3(vm, 0);
    break;
  case 4:
    if (vm->color_1.a == 0 && vm->color_2.a == 0)
      break;
    if (!vm->write_sprite_corners__mode_4())
      draw_vm__modes_0_1_2_3(vm, 4);
    break;
  case 5:
    if (vm->color_1.a == 0 && vm->color_2.a == 0)
      break;
    calc_mat_world(vm);
    // are probably multiplied with __matrix_186017c... When ??
    draw_vm__modes_0_1_2_3(vm, 0);
    SPRITE_TEMP_BUFFER[3].transformed_pos.w = 1.0;
    SPRITE_TEMP_BUFFER[2].transformed_pos.w = 1.0;
    SPRITE_TEMP_BUFFER[1].transformed_pos.w = 1.0;
    SPRITE_TEMP_BUFFER[0].transformed_pos.w = 1.0;
    break;
  case 6:
    if (vm->color_1.a == 0 && vm->color_2.a == 0)
      break;
    draw_vm__mode_6(vm);
    break;
  case 7: {
    if (vm->color_1.a == 0 && vm->color_2.a == 0)
      break;
    calc_mat_world(vm);
    auto col [[maybe_unused]] =
        vm->bitflags.colmode ? vm->color_2 : vm->color_1;
    for (i32 i = 0; i < 4; i++) {
      ns::vec3 v =
          ns::vec3(AM->__matrix_186017c * ns::vec4(AM->some_positions[2],
                                                   AM->some_positions[3],
                                                   AM->some_positions[4], 1));
      if (ns::point_distance(v, AM->current_camera->position) <=
          AM->current_camera->sky.begin_distance) {
        SPRITE_TEMP_BUFFER[i].diffuse_color = col;
      } else {
        f32 coeff = (AM->current_camera->sky.begin_distance -
                     ns::point_distance(v, AM->current_camera->position)) /
                    (AM->current_camera->sky.begin_distance -
                     AM->current_camera->sky.end_distance);
        if (coeff < 1.0) {
          SPRITE_TEMP_BUFFER[i].diffuse_color.b =
              col.b -
              ((col.b - AM->current_camera->sky.color_components[0]) * coeff);
          SPRITE_TEMP_BUFFER[i].diffuse_color.g =
              col.g -
              ((col.g - AM->current_camera->sky.color_components[1]) * coeff);
          SPRITE_TEMP_BUFFER[i].diffuse_color.r =
              col.r -
              ((col.r - AM->current_camera->sky.color_components[2]) * coeff);
          SPRITE_TEMP_BUFFER[i].diffuse_color.a = col.a;
        } else {
          SPRITE_TEMP_BUFFER[i].diffuse_color = AM->current_camera->sky.color;
          SPRITE_TEMP_BUFFER[i].diffuse_color.a = col.a;
        }
      }
    }
    // somehow the coords are multiplied with __matrix_186017c... When ??
    draw_vm__modes_0_1_2_3(vm, 2);
    SPRITE_TEMP_BUFFER[3].transformed_pos.w = 1.0;
    SPRITE_TEMP_BUFFER[2].transformed_pos.w = 1.0;
    SPRITE_TEMP_BUFFER[1].transformed_pos.w = 1.0;
    SPRITE_TEMP_BUFFER[0].transformed_pos.w = 1.0;
    break;
  }
  case 8:
    if (vm->color_1.a == 0 && vm->color_2.a == 0)
      break;
    draw_vm__mode_8_15(vm);
    break;
  case 9:
  case 0xc:
  case 0xd:
  case 0xe:
    draw_vm_as_triangle_strip(
        vm, reinterpret_cast<RenderVertex_t *>(vm->special_vertex_buffer_data),
        vm->int_script_vars[0] * 2);
    break;
  case 0xb:
    draw_vm_triangle_fan(
        vm, reinterpret_cast<RenderVertex_t *>(vm->special_vertex_buffer_data),
        vm->int_script_vars[0] * 2);
    break;
  case 0xf:
    if (vm->color_1.a == 0 && vm->color_2.a == 0)
      break;
    enable_fog();
    draw_vm__mode_8_15(vm);
    disable_fog();
    break;
  case 0x10:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x1a:
  case 0x1b: {
    f32 rot = vm->rotation.z;
    f32 xs = vm->sprite_size.x * vm->scale.x;
    f32 ys = vm->sprite_size.y * vm->scale.y;
    ns::vec3 pos = vm->pos + vm->entity_pos + vm->__pos_2;
    vm->transform_coordinate(pos);
    if (vm->parent_vm && !vm->bitflags.noParent) {
      xs *= vm->parent_vm->scale.x;
      ys *= vm->parent_vm->scale.y;
      rot += vm->parent_vm->rotation.z;
    }
    setup_render_state_for_vm(vm);
    if (vm->bitflags.resolutionMode == 1)
      ys *= RESOLUTION_MULT;
    else if (vm->bitflags.resolutionMode == 2)
      ys *= RESOLUTION_MULT / 2.0;
    switch (vm->bitflags.rendermode) {
    case 0x10:
      draw_vm__ins_603(pos.x, pos.y, xs, ys, rot, vm->color_1, vm->color_1,
                       vm->bitflags.anchorX, vm->bitflags.anchorY);
      break;
    case 0x14:
      draw_vm__ins_603(pos.x, pos.y, xs, ys, rot, vm->color_1, vm->color_2,
                       vm->bitflags.anchorX, vm->bitflags.anchorY);
      break;
    case 0x15:
      draw_vm__ins_607(pos.x, pos.y, xs, ys, rot, vm->color_1, vm->color_1,
                       vm->bitflags.anchorX, vm->bitflags.anchorY);
      break;
    case 0x16:
      draw_vm__ins_607(pos.x, pos.y, xs, ys, rot, vm->color_1, vm->color_2,
                       vm->bitflags.anchorX, vm->bitflags.anchorY);
      break;
    case 0x1a:
      draw_vm__ins_613(pos.x, pos.y, xs, rot, vm->color_1,
                       (vm->bitflags.colmode == 0) ? vm->color_1 : vm->color_2,
                       vm->bitflags.anchorX);
      break;
    case 0x1b:
      draw_vm__ins_612(pos.x, pos.y, xs, ys, rot, vm->color_1,
                       (vm->bitflags.colmode == 0) ? vm->color_1 : vm->color_2,
                       vm->bitflags.anchorX, vm->bitflags.anchorY);
      break;
    }
    break;
  }
  case 0x11:
  case 0x12:
  case 0x13: {
    f32 rot = vm->rotation.z;
    f32 xs = vm->sprite_size.x * vm->scale.x;
    f32 ys = vm->sprite_size.y * vm->scale.y;
    ns::vec3 pos = vm->pos + vm->entity_pos + vm->__pos_2;
    vm->transform_coordinate(pos);
    if (vm->parent_vm && !vm->bitflags.noParent) {
      rot += vm->parent_vm->rotation.z;
      xs *= vm->parent_vm->scale.x;
      ys *= vm->parent_vm->scale.y;
    }
    if (vm->bitflags.resolutionMode == 1) {
      xs *= RESOLUTION_MULT;
      ys *= RESOLUTION_MULT;
    } else if (vm->bitflags.resolutionMode == 2) {
      xs *= RESOLUTION_MULT / 2.0;
      ys *= RESOLUTION_MULT / 2.0;
    }
    setup_render_state_for_vm(vm);
    if (vm->bitflags.rendermode == 0x11) {
      draw_vm__mode_17__drawCircle(pos.x, pos.y, rot, xs,
                                   vm->int_script_vars[0], vm->color_1,
                                   vm->color_2);
    } else if (vm->bitflags.rendermode == 0x12) {
      draw_vm__mode_18__drawCircleBorder(pos.x, pos.y, rot, xs,
                                         vm->int_script_vars[0], vm->color_1);
    } else if (vm->bitflags.rendermode == 0x13) {
      draw_vm__mode_19__drawRing(pos.x, pos.y, rot, xs, ys,
                                 vm->int_script_vars[0], vm->color_1);
    }
  } break;
  case 0x18:
  case 0x19:
    draw_vm_mode_24_25(vm, vm->special_vertex_buffer_data,
                       vm->int_script_vars[0] * 2);
    break;
  }

  // vm->color_1.a = saved_a1;
  // vm->color_2.a = saved_a2;
}

void draw_vm__modes_0_1_2_3(VM *vm, i32 i) {
  SPRITE_TEMP_BUFFER[0].transformed_pos.x += AM->cam_vec2_fc_x;
  SPRITE_TEMP_BUFFER[0].transformed_pos.y += AM->cam_vec2_fc_y;
  SPRITE_TEMP_BUFFER[1].transformed_pos.x += AM->cam_vec2_fc_x;
  SPRITE_TEMP_BUFFER[1].transformed_pos.y += AM->cam_vec2_fc_y;
  SPRITE_TEMP_BUFFER[2].transformed_pos.x += AM->cam_vec2_fc_x;
  SPRITE_TEMP_BUFFER[2].transformed_pos.y += AM->cam_vec2_fc_y;
  SPRITE_TEMP_BUFFER[3].transformed_pos.x += AM->cam_vec2_fc_x;
  SPRITE_TEMP_BUFFER[3].transformed_pos.y += AM->cam_vec2_fc_y;
  if (!(i & 1)) {
    SPRITE_TEMP_BUFFER[0].transformed_pos.x =
        round(SPRITE_TEMP_BUFFER[0].transformed_pos.x) - 0.5;
    SPRITE_TEMP_BUFFER[0].transformed_pos.y =
        round(SPRITE_TEMP_BUFFER[0].transformed_pos.y) - 0.5;
    SPRITE_TEMP_BUFFER[1].transformed_pos.x =
        round(SPRITE_TEMP_BUFFER[1].transformed_pos.x) - 0.5;
    SPRITE_TEMP_BUFFER[1].transformed_pos.y =
        round(SPRITE_TEMP_BUFFER[1].transformed_pos.y) - 0.5;
    SPRITE_TEMP_BUFFER[2].transformed_pos.x =
        round(SPRITE_TEMP_BUFFER[2].transformed_pos.x) - 0.5;
    SPRITE_TEMP_BUFFER[2].transformed_pos.y =
        round(SPRITE_TEMP_BUFFER[2].transformed_pos.y) - 0.5;
    SPRITE_TEMP_BUFFER[3].transformed_pos.x =
        round(SPRITE_TEMP_BUFFER[3].transformed_pos.x) - 0.5;
    SPRITE_TEMP_BUFFER[3].transformed_pos.y =
        round(SPRITE_TEMP_BUFFER[3].transformed_pos.y) - 0.5;
  }
  vm->last_rendered_quad_in_surface_space[0] =
      SPRITE_TEMP_BUFFER[0].transformed_pos;
  vm->last_rendered_quad_in_surface_space[1] =
      SPRITE_TEMP_BUFFER[1].transformed_pos;
  vm->last_rendered_quad_in_surface_space[2] =
      SPRITE_TEMP_BUFFER[2].transformed_pos;
  vm->last_rendered_quad_in_surface_space[3] =
      SPRITE_TEMP_BUFFER[3].transformed_pos;

  if (!(i & 4)) {
    /* if max of x pos is smaller than left of viewport --> discard drawing */
    if (fmax(fmax(SPRITE_TEMP_BUFFER[0].transformed_pos.x,
                  SPRITE_TEMP_BUFFER[1].transformed_pos.x),
             fmax(SPRITE_TEMP_BUFFER[2].transformed_pos.x,
                  SPRITE_TEMP_BUFFER[3].transformed_pos.x)) <
        AM->current_camera->viewport.X)
      return;

    /* if max of y pos is smaller than top of viewport --> discard drawing */
    if (fmax(fmax(SPRITE_TEMP_BUFFER[0].transformed_pos.y,
                  SPRITE_TEMP_BUFFER[1].transformed_pos.y),
             fmax(SPRITE_TEMP_BUFFER[2].transformed_pos.y,
                  SPRITE_TEMP_BUFFER[3].transformed_pos.y)) <
        AM->current_camera->viewport.Y)
      return;

    /* if min of x pos is larger than right of viewport --> discard drawing */
    if (fmin(fmin(SPRITE_TEMP_BUFFER[0].transformed_pos.x,
                  SPRITE_TEMP_BUFFER[1].transformed_pos.x),
             fmin(SPRITE_TEMP_BUFFER[2].transformed_pos.x,
                  SPRITE_TEMP_BUFFER[3].transformed_pos.x)) >
        AM->current_camera->viewport.Width + AM->current_camera->viewport.X)
      return;

    /* if min of y pos is larger than bottom of viewport --> discard drawing */
    if (fmin(fmin(SPRITE_TEMP_BUFFER[0].transformed_pos.y,
                  SPRITE_TEMP_BUFFER[1].transformed_pos.y),
             fmin(SPRITE_TEMP_BUFFER[2].transformed_pos.y,
                  SPRITE_TEMP_BUFFER[3].transformed_pos.y)) >
        AM->current_camera->viewport.Height + AM->current_camera->viewport.Y)
      return;
  }

  SPRITE_TEMP_BUFFER[0].texture_uv.x =
      vm->uv_scroll_pos.x + vm->uv_quad_of_sprite[0].x;
  SPRITE_TEMP_BUFFER[0].texture_uv.y =
      vm->uv_scroll_pos.y + vm->uv_quad_of_sprite[0].y;
  SPRITE_TEMP_BUFFER[1].texture_uv.y =
      vm->uv_scroll_pos.y + vm->uv_quad_of_sprite[1].y;
  SPRITE_TEMP_BUFFER[1].texture_uv.x =
      vm->uv_scroll_pos.x + vm->uv_quad_of_sprite[0].x +
      (vm->uv_quad_of_sprite[1].x - vm->uv_quad_of_sprite[0].x) *
          vm->uv_scale.x;
  SPRITE_TEMP_BUFFER[2].texture_uv.x =
      vm->uv_scroll_pos.x + vm->uv_quad_of_sprite[2].x;
  SPRITE_TEMP_BUFFER[2].texture_uv.y =
      vm->uv_scroll_pos.y + vm->uv_quad_of_sprite[0].y +
      (vm->uv_quad_of_sprite[2].y - vm->uv_quad_of_sprite[0].y) *
          vm->uv_scale.y;
  SPRITE_TEMP_BUFFER[3].texture_uv.x =
      vm->uv_scroll_pos.x + vm->uv_quad_of_sprite[2].x +
      (vm->uv_quad_of_sprite[3].x - vm->uv_quad_of_sprite[2].x) *
          vm->uv_scale.x;
  SPRITE_TEMP_BUFFER[3].texture_uv.y =
      vm->uv_scroll_pos.y + vm->uv_quad_of_sprite[1].y +
      (vm->uv_quad_of_sprite[3].y - vm->uv_quad_of_sprite[1].y) *
          vm->uv_scale.y;

  anm_use_texture(vm);

  if (AM->field_0x18607ca != 1) {
    flush_vbos();
    AM->field_0x18607ca = 1;
  }

  if (!(i & 2)) {
    ns::Color col = vm->color_1;
    ns::Color col2 = vm->color_2;
    if (vm->bitflags.colmode == 2) {
      if (AM->use_custom_color_1c90a4c) {
        col = ns::mix(col, AM->custom_color_1c90a48);
        col2 = ns::mix(col2, AM->custom_color_1c90a48);
      }
      SPRITE_TEMP_BUFFER[0].diffuse_color = col;
      SPRITE_TEMP_BUFFER[1].diffuse_color = col2;
      SPRITE_TEMP_BUFFER[2].diffuse_color = col;
      SPRITE_TEMP_BUFFER[3].diffuse_color = col2;
    } else if (vm->bitflags.colmode == 3) {
      if (AM->use_custom_color_1c90a4c) {
        col = ns::mix(col, AM->custom_color_1c90a48);
        col2 = ns::mix(col2, AM->custom_color_1c90a48);
      }
      SPRITE_TEMP_BUFFER[0].diffuse_color = col;
      SPRITE_TEMP_BUFFER[1].diffuse_color = col;
      SPRITE_TEMP_BUFFER[2].diffuse_color = col2;
      SPRITE_TEMP_BUFFER[3].diffuse_color = col2;
    } else {
      if (vm->bitflags.colmode == 1)
        col = vm->color_2;
      if (vm->bitflags.colorizeChildren && vm->parent_vm)
        col = ns::mix(col, vm->parent_vm->mixed_inherited_color);
      vm->mixed_inherited_color = col;
      if (AM->use_custom_color_1c90a4c)
        col = ns::mix(col, AM->custom_color_1c90a48);
      SPRITE_TEMP_BUFFER[0].diffuse_color = col;
      SPRITE_TEMP_BUFFER[1].diffuse_color = col;
      SPRITE_TEMP_BUFFER[2].diffuse_color = col;
      SPRITE_TEMP_BUFFER[3].diffuse_color = col;
    }
  }

  setup_render_state_for_vm(vm);
  ns::Vertex tl = {ns::vec3(SPRITE_TEMP_BUFFER[0].transformed_pos),
                   SPRITE_TEMP_BUFFER[0].diffuse_color,
                   SPRITE_TEMP_BUFFER[0].texture_uv};
  ns::Vertex tr = {ns::vec3(SPRITE_TEMP_BUFFER[2].transformed_pos),
                   SPRITE_TEMP_BUFFER[2].diffuse_color,
                   SPRITE_TEMP_BUFFER[2].texture_uv};
  ns::Vertex br = {ns::vec3(SPRITE_TEMP_BUFFER[3].transformed_pos),
                   SPRITE_TEMP_BUFFER[3].diffuse_color,
                   SPRITE_TEMP_BUFFER[3].texture_uv};
  ns::Vertex bl = {ns::vec3(SPRITE_TEMP_BUFFER[1].transformed_pos),
                   SPRITE_TEMP_BUFFER[1].diffuse_color,
                   SPRITE_TEMP_BUFFER[1].texture_uv};
  AM->batch->draw_quad(tl, tr, br, bl);
}

i32 draw_vm__mode_6(VM *vm) {
  if (vm->write_sprite_corners__mode_4() != 0) {
    return -1;
  }
  ns::vec3 pos_from_cam =
      vm->entity_pos + vm->pos + vm->__pos_2 - AM->current_camera->position;
  if (vm->bitflags.originMode != 0 && !vm->parent_vm) {
    pos_from_cam += ns::vec3(BACK_BUFFER_SIZE.x * 0.5,
                             (BACK_BUFFER_SIZE.y - 448.0) * 0.5, 0);
  }
  f32 dist_to_cam = ns::point_distance({0, 0, 0}, pos_from_cam);
  f32 fog_factor;
  ns::Color col;
  ns::Color col2;
  switch (vm->bitflags.colmode) {
  default:
    col = vm->bitflags.colmode == 0 ? vm->color_1 : vm->color_2;
    if (AM->use_custom_color_1c90a4c) {
      col = ns::mix(col, AM->custom_color_1c90a48);
    }
    if (dist_to_cam <= AM->current_camera->sky.begin_distance) {
      SPRITE_TEMP_BUFFER[0].diffuse_color = col;
      SPRITE_TEMP_BUFFER[1].diffuse_color = col;
      SPRITE_TEMP_BUFFER[2].diffuse_color = col;
      SPRITE_TEMP_BUFFER[3].diffuse_color = col;
      draw_vm__modes_0_1_2_3(vm, 6);
      return 0;
    }

    fog_factor = (dist_to_cam - AM->current_camera->sky.begin_distance) /
                 (AM->current_camera->sky.end_distance -
                  AM->current_camera->sky.begin_distance);
    if (fog_factor > 1.0)
      return -1;
    SPRITE_TEMP_BUFFER[0].diffuse_color.r =
        col.r -
        (col.r - AM->current_camera->sky.color_components[2]) * fog_factor;
    SPRITE_TEMP_BUFFER[0].diffuse_color.g =
        col.g -
        (col.g - AM->current_camera->sky.color_components[1]) * fog_factor;
    SPRITE_TEMP_BUFFER[0].diffuse_color.b =
        col.b -
        (col.b - AM->current_camera->sky.color_components[0]) * fog_factor;
    SPRITE_TEMP_BUFFER[0].diffuse_color.a = (1.0 - pow(fog_factor, 3)) * col.a;
    SPRITE_TEMP_BUFFER[1].diffuse_color = SPRITE_TEMP_BUFFER[0].diffuse_color;
    SPRITE_TEMP_BUFFER[2].diffuse_color = SPRITE_TEMP_BUFFER[0].diffuse_color;
    SPRITE_TEMP_BUFFER[3].diffuse_color = SPRITE_TEMP_BUFFER[0].diffuse_color;
    draw_vm__modes_0_1_2_3(vm, 6);
    return 0;
  case 2:
  case 3:
    col2 = vm->color_2;
    col = vm->color_1;
    if (AM->use_custom_color_1c90a4c) {
      col = ns::mix(col, AM->custom_color_1c90a48);
      col2 = ns::mix(col2, AM->custom_color_1c90a48);
    }
    SPRITE_TEMP_BUFFER[3].diffuse_color = col;
    SPRITE_TEMP_BUFFER[0].diffuse_color = col2;
    if (AM->current_camera->sky.begin_distance < dist_to_cam) {
      fog_factor = (AM->current_camera->sky.begin_distance - dist_to_cam) /
                   (AM->current_camera->sky.begin_distance -
                    AM->current_camera->sky.end_distance);
      if (1.0 <= fog_factor) {
        return -1;
      }
      SPRITE_TEMP_BUFFER[0].diffuse_color.r =
          col.r -
          (col.r - AM->current_camera->sky.color_components[2]) * fog_factor;
      SPRITE_TEMP_BUFFER[0].diffuse_color.g =
          col.g -
          (col.g - AM->current_camera->sky.color_components[1]) * fog_factor;
      SPRITE_TEMP_BUFFER[0].diffuse_color.b =
          col.b -
          (col.b - AM->current_camera->sky.color_components[0]) * fog_factor;
      SPRITE_TEMP_BUFFER[0].diffuse_color.a = (col.a * (1.0 - fog_factor));
      SPRITE_TEMP_BUFFER[3].diffuse_color.r =
          col2.r -
          (col2.r - AM->current_camera->sky.color_components[2]) * fog_factor;
      SPRITE_TEMP_BUFFER[3].diffuse_color.g =
          col2.g -
          (col2.g - AM->current_camera->sky.color_components[1]) * fog_factor;
      SPRITE_TEMP_BUFFER[3].diffuse_color.b =
          col2.b -
          (col2.b - AM->current_camera->sky.color_components[0]) * fog_factor;
      SPRITE_TEMP_BUFFER[3].diffuse_color.a = (col2.a * (1.0 - fog_factor));
    }
    if (vm->bitflags.colmode == 2) {
      SPRITE_TEMP_BUFFER[1].diffuse_color = SPRITE_TEMP_BUFFER[3].diffuse_color;
      SPRITE_TEMP_BUFFER[2].diffuse_color = SPRITE_TEMP_BUFFER[0].diffuse_color;
    } else {
      SPRITE_TEMP_BUFFER[1].diffuse_color = SPRITE_TEMP_BUFFER[0].diffuse_color;
      SPRITE_TEMP_BUFFER[2].diffuse_color = SPRITE_TEMP_BUFFER[3].diffuse_color;
    }
    draw_vm__modes_0_1_2_3(vm, 6);
    return 0;
  }
}

void draw_vm_as_quad(VM *vm) {
  vm->write_sprite_corners__with_z_rot(SPRITE_TEMP_BUFFER[0].transformed_pos,
                                       SPRITE_TEMP_BUFFER[1].transformed_pos,
                                       SPRITE_TEMP_BUFFER[2].transformed_pos,
                                       SPRITE_TEMP_BUFFER[3].transformed_pos);
  draw_vm__modes_0_1_2_3(vm, 0);
}

void draw_vm_as_quad_no_rot(VM *vm) {
  vm->write_sprite_corners__without_rot(SPRITE_TEMP_BUFFER[0].transformed_pos,
                                        SPRITE_TEMP_BUFFER[1].transformed_pos,
                                        SPRITE_TEMP_BUFFER[2].transformed_pos,
                                        SPRITE_TEMP_BUFFER[3].transformed_pos);
  draw_vm__modes_0_1_2_3(vm, 1);
}

void draw_vm__mode_8_15(VM *vm) {
  if (!vm->bitflags.visible || !vm->bitflags.f530_1 || vm->color_1.a == 0) {
    return;
  }
  if (vm->bitflags.zwritedis) {
    disable_zwrite();
  } else {
    enable_zwrite();
  }
  if (vm->bitflags.f530_16)
    goto LAB_0046ecb8;
  vm->__matrix_2 = ns::mat::scale(vm->__matrix_1, ns::vec3(vm->scale * vm->scale_2, 1));
  vm->bitflags.scaled = false;
  if (vm->bitflags.resolutionMode == 1) {
    vm->__matrix_2[0][0] *= RESOLUTION_MULT;
    vm->__matrix_2[1][1] *= RESOLUTION_MULT;
  } else if (vm->bitflags.resolutionMode == 2) {
    vm->__matrix_2[0][0] *= RESOLUTION_MULT * 0.5;
    vm->__matrix_2[1][1] *= RESOLUTION_MULT * 0.5;
  }
  switch (vm->bitflags.rotationMode) {
  case 0:
    if (vm->rotation.x != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_x(vm->rotation.x) * vm->__matrix_2;
    if (vm->rotation.y != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_y(vm->rotation.y) * vm->__matrix_2;
    if (vm->rotation.z != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_z(vm->rotation.z) * vm->__matrix_2;
    break;
  case 1:
    if (vm->rotation.x != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_x(vm->rotation.x) * vm->__matrix_2;
    if (vm->rotation.z != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_z(vm->rotation.z) * vm->__matrix_2;
    if (vm->rotation.y != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_y(vm->rotation.y) * vm->__matrix_2;
    break;
  case 2:
    if (vm->rotation.y != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_y(vm->rotation.y) * vm->__matrix_2;
    if (vm->rotation.x != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_x(vm->rotation.x) * vm->__matrix_2;
    if (vm->rotation.z != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_z(vm->rotation.z) * vm->__matrix_2;
    break;
  case 3:
    if (vm->rotation.y != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_y(vm->rotation.y) * vm->__matrix_2;
    if (vm->rotation.z != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_z(vm->rotation.z) * vm->__matrix_2;
    if (vm->rotation.x != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_x(vm->rotation.x) * vm->__matrix_2;
    break;
  case 4:
    if (vm->rotation.z != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_z(vm->rotation.z) * vm->__matrix_2;
    if (vm->rotation.x != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_x(vm->rotation.x) * vm->__matrix_2;
    if (vm->rotation.y != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_y(vm->rotation.y) * vm->__matrix_2;
    break;
  case 5:
    if (vm->rotation.z != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_z(vm->rotation.z) * vm->__matrix_2;
    if (vm->rotation.y != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_y(vm->rotation.y) * vm->__matrix_2;
    if (vm->rotation.x != 0.0)
      vm->__matrix_2 = ns::mat::mk4_rotate_x(vm->rotation.x) * vm->__matrix_2;
    break;
  }
  vm->bitflags.rotated = false;
LAB_0046ecb8:
  ns::mat4 local_90 = vm->__matrix_2;
  local_90[3][0] = vm->entity_pos.x + vm->pos.x + vm->__pos_2.x -
                   vm->anchor_offset.x * vm->scale.x * vm->scale_2.x;
  local_90[3][1] = vm->entity_pos.y + vm->pos.y + vm->__pos_2.y -
                   vm->anchor_offset.y * vm->scale.y * vm->scale_2.y;
  ns::vec3 pp = {local_90[3][0], local_90[3][1], local_90[3][2]};
  vm->transform_coordinate(pp);
  local_90[3][0] = pp.x;
  local_90[3][1] = pp.y;
  local_90[3][2] = pp.z;
  ns::Color col = vm->color_1;
  if (vm->bitflags.colmode != 0) {
    col = vm->color_2;
  }
  if (AM->use_custom_color_1c90a4c != 0) {
    col = ns::mix(col, AM->custom_color_1c90a48);
  }
  // if (field_0x18607c0 != col) {
  //   flush_vbos();
  //   field_0x18607c0 = col;
  //   SUPERVISOR.d3d_device->SetRenderState(D3DRS_TEXTUREFACTOR, col);
  // }
  local_90[3][2] = vm->entity_pos.z + vm->pos.z + vm->__pos_2.z;
  // SUPERVISOR.d3d_device->SetTransform(0x100, (D3DMATRIX *)local_90);
  anm_use_texture(vm);
  setup_render_state_for_vm(vm);

  // My version
  f32 l = vm->bitflags.anchorX != 0 ? -(vm->bitflags.anchorX - 1) : -0.5f;
  f32 r = l + 1;
  f32 t = vm->bitflags.anchorY != 0 ? -(vm->bitflags.anchorY - 1) : -0.5f;
  f32 b = t + 1;
  f32 top = -t * 256;
  f32 bottom = -b * 256;
  f32 left = l * 256;
  f32 right = r * 256;
  f32 u1 = (vm->getSprite().u1 + vm->uv_scroll_pos.x) * vm->uv_scale.x;
  f32 u2 = (vm->getSprite().u2 + vm->uv_scroll_pos.x) * vm->uv_scale.x;
  f32 v1 = (vm->getSprite().v1 + vm->uv_scroll_pos.y) * vm->uv_scale.y;
  f32 v2 = (vm->getSprite().v2 + vm->uv_scroll_pos.y) * vm->uv_scale.y;

  ns::Vertex tl = {ns::vec3(local_90 * ns::vec4(left, top, 0, 1)), col, {u1, v2}};
  ns::Vertex tr = {ns::vec3(local_90 * ns::vec4(right, top, 0, 1)), col, {u2, v2}};
  ns::Vertex br = {ns::vec3(local_90 * ns::vec4(right, bottom, 0, 1)), col, {u2, v1}};
  ns::Vertex bl = {ns::vec3(local_90 * ns::vec4(left, bottom, 0, 1)), col, {u1, v1}};

  AM->batch->draw_quad(tl, tr, br, bl);
  // TODO: real ?
  // What I did is equivalent to below (don't understand TextureStageState
  // though)

  // if ((/*(some_positions[0] != vm->getSprite()) ||*/
  //      (vm->uv_scroll_pos.x != 0.0)) ||
  //     ((vm->uv_scale.x != 1.0 ||
  //       (vm->uv_scale.y != 1.0)))) {
  //   some_positions[0] = fVar11;
  //   ns::mat4 local_d0 = vm->__matrix_3;
  //   local_d0.m00 *= vm->uv_scale.x;
  //   local_d0.m11 *= vm->uv_scale.y;
  //   local_d0.m20 = vm->uv_quad_of_sprite[0].x +
  //                  vm->uv_scroll_pos.x;
  //   local_d0.m21 = vm->uv_quad_of_sprite[0].y +
  //                  vm->uv_scroll_pos.y;
  //   SUPERVISOR.d3d_device->SetTransform(D3DTS_TEXTURE0, &local_d0);
  // }
  // if (field28_0x18607ca != 2) {
  //   SUPERVISOR.d3d_device->SetStreamSource(0, this->some_positions[1], 0,
  //   0x14); SUPERVISOR.d3d_device->SetFVF(0x102);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 3);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, 3);
  //   field28_0x18607ca = 2;
  // }
  // if (ANM_MANAGER_PTR->field33_0x18607cf != 1) {
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, 4);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, 4);
  //   ANM_MANAGER_PTR->field33_0x18607cf = 1;
  // }
  // uVar7 = vm->field_0x530;
  // SUPERVISOR.d3d_device->vtable->DrawPrimitive( 5, ((uVar7 >> 0x17 & 3) * 3 +
  // (uVar7 >> 0x15 & 3)) * 4, 2);
}

void draw_vm_triangle_fan(VM *vm, RenderVertex_t *vertexData, i32 nVert) {
  // if (field_0x18607ca != 3) {
  //   SUPERVISOR.d3d_device->SetFVF(0x144);
  //   field_0x18607ca = 3;
  // }
  anm_use_texture(vm);
  setup_render_state_for_vm(vm);
  disable_zwrite();
  // if (field_0x18607ca != 1) {
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
  //   field_0x18607ca = 1;
  // }
  // draw data in buffer
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, nVert - 2,
  //                                        vertexData, 0x1c);
  // My version
  // i32 nQuad = (nVert - 2) / 2;
  // for (i32 i = 0; i < nQuad; i++) {
  //   auto tl = vertexData[0];
  //   auto tr = vertexData[i * 2 + 1];
  //   auto br = vertexData[i * 2 + 2];
  //   auto bl = vertexData[i * 2 + 3];
  //   AM->batch->draw({ns::vec3(tl.transformed_pos), tl.diffuse_color, {0, 0}},
  //                   {ns::vec3(tr.transformed_pos), tr.diffuse_color, {1, 0}},
  //                   {ns::vec3(br.transformed_pos), br.diffuse_color, {1, 1}},
  //                   {ns::vec3(bl.transformed_pos), bl.diffuse_color, {0,
  //                   1}});
  // }
  u32 center = AM->batch->add_vertex({ns::vec3(vertexData[0].transformed_pos),
                                      vertexData[0].diffuse_color,
                                      vertexData[0].texture_uv});
  u32 last = AM->batch->add_vertex({ns::vec3(vertexData[1].transformed_pos),
                                    vertexData[1].diffuse_color,
                                    vertexData[1].texture_uv});
  for (i32 i = 2; i < nVert; i++) {
    u32 new_vert = AM->batch->add_vertex(
        {ns::vec3(vertexData[i].transformed_pos), vertexData[i].diffuse_color,
         vertexData[i].texture_uv});
    AM->batch->draw_tri(center, last, new_vert);
    last = new_vert;
  }
}

static void some_d3d_stuff() {
  // if (field_0x18607cf != 0) {
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, 3);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, 3);
  //   field_0x18607cf = 0;
  // }
  // if (field_0x18607ca != 1) {
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
  //   field_0x18607ca = 1;
  // }
  // SUPERVISOR.d3d_device->SetFVF(0x44);
}

void draw_vm_as_triangle_strip(VM *vm, RenderVertex_t *vertexData, usize nVert) {
  if (!vm->bitflags.visible || !vm->bitflags.f530_1 || vm->color_1.a == 0)
    return;
  anm_use_texture(vm);
  setup_render_state_for_vm(vm);
  u32 last2 = AM->batch->add_vertex({ns::vec3(vertexData[0].transformed_pos),
                                     vertexData[0].diffuse_color,
                                     vertexData[0].texture_uv});
  u32 last = AM->batch->add_vertex({ns::vec3(vertexData[1].transformed_pos),
                                    vertexData[1].diffuse_color,
                                    vertexData[1].texture_uv});
  for (usize i = 2; i < nVert; i++) {
    u32 new_vert = AM->batch->add_vertex(
        {ns::vec3(vertexData[i].transformed_pos), vertexData[i].diffuse_color,
         vertexData[i].texture_uv});
    AM->batch->draw_tri(last2, i % 2 ? new_vert : last,
                        i % 2 ? last : new_vert);
    last2 = last;
    last = new_vert;
  }
  // if (field_0x18607ca != 3) {
  //   SUPERVISOR.d3d_device->SetFVF(0x144);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
  //   field_0x18607ca = 3;
  // }
  // if (field_0x18607cf != 1) {
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, 4);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, 4);
  //   field_0x18607cf = 1;
  // }
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, nVert - 2,
  //                                        vertexData, 0x1c);
  return;
}

i32 draw_vm__ins_607(f32 x, f32 y, f32 width, f32 height, f32 rot_z,
                     ns::Color col1, ns::Color col2, i32 anchorX, i32 anchorY) {
  draw_vm__ins_603(x, y, width + 1.0, height + 1.0, rot_z,
                   {col1.r, col1.g, col1.b, static_cast<u8>(col1.a / 2)},
                   {col2.r, col2.g, col2.b, static_cast<u8>(col2.a / 2)},
                   anchorX, anchorY);
  draw_vm__ins_603(x, y, width, height, rot_z, col1, col2, anchorX, anchorY);
  return 0;
}

i32 draw_vm__ins_603(f32 x, f32 y, f32 width, f32 height, f32 rot_z,
                     ns::Color col1, ns::Color col2, i32 anchorX, i32 anchorY) {
  f32 x1 = anchorX == 0 ? -width / 2.0 : anchorX == 1 ? 0.0 : -width;
  f32 x2 = anchorX == 0 ? width / 2.0 : anchorX == 1 ? width : 0.0;
  f32 y1 = anchorY == 0 ? -height / 2.0 : anchorY == 1 ? 0.0 : -height;
  f32 y2 = anchorY == 0 ? height / 2.0 : anchorY == 1 ? height : 0.0;
  float cz = cos(rot_z);
  float sz = sin(rot_z);
  ns::vec2 pos0 = {x1 * cz - y1 * sz + x, x1 * sz + y1 * cz + y};
  ns::vec2 pos1 = {x2 * cz - y1 * sz + x, x2 * sz + y1 * cz + y};
  ns::vec2 pos2 = {x2 * cz - y2 * sz + x, x2 * sz + y2 * cz + y};
  ns::vec2 pos3 = {x1 * cz - y2 * sz + x, x1 * sz + y2 * cz + y};
  disable_zwrite();
  anm_use_default_texture();
  AM->batch->draw_quad({{pos0, 0.f}, col1, {}}, {{pos1, 0.f}, col2, {}},
                       {{pos2, 0.f}, col2, {}}, {{pos3, 0.f}, col1, {}});
  return 0;
  flush_vbos();
  some_d3d_stuff();
  // SUPERVISOR.d3d_device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, this->vertex_buffers.primitive_write_cursor, 0x14);
  // AM->vertex_buffers.primitive_write_cursor += 4;
  AM->draw_call_ctr_0xcc++;
  return 0;
}

i32 draw_vm__ins_613(f32 x, f32 y, f32 width, f32 rot_z, ns::Color col1,
                     ns::Color col2, i32 anchor) {
  f32 x1 = anchor == 0 ? -width / 2.0 : anchor == 1 ? 0.0 : -width;
  f32 x2 = anchor == 0 ? width / 2.0 : anchor == 1 ? width : 0.0;
  ns::vec2 pos0 = {x1 * ns::cos(rot_z) + x, x1 * ns::sin(rot_z) + y};
  ns::vec2 pos1 = {x2 * ns::cos(rot_z) + x, x2 * ns::sin(rot_z) + y};

  disable_zwrite();
  anm_use_default_texture();

  f32 angle = ns::point_direction(pos0, pos1);
  ns::vec2 dir = ns::lengthdir_vec(1.f / 2.f, angle + 90);

  AM->batch->draw_quad(
      {{pos0 - dir, 0.f}, col1, {}}, {{pos0 + dir, 0.f}, col1, {}},
      {{pos1 + dir, 0.f}, col2, {}}, {{pos1 - dir, 0.f}, col2, {}});
  some_d3d_stuff();
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, this->vertex_buffers.primitive_write_cursor, 0x14);
  // AM->vertex_buffers.primitive_write_cursor += 2;
  AM->draw_call_ctr_0xcc++;
  return 0;
}

i32 draw_vm__ins_612(f32 x, f32 y, f32 width, f32 height, f32 rotz,
                     ns::Color col1, ns::Color col2, i32 anchorX, i32 anchorY) {
  f32 x1 = anchorX == 0 ? -width / 2.0 : anchorX == 1 ? 0.0 : -width;
  f32 x2 = anchorX == 0 ? width / 2.0 : anchorX == 1 ? width : 0.0;
  f32 y1 = anchorY == 0 ? -height / 2.0 : anchorY == 1 ? 0.0 : -height;
  f32 y2 = anchorY == 0 ? height / 2.0 : anchorY == 1 ? height : 0.0;
  float cz = cos(rotz);
  float sz = sin(rotz);
  ns::vec2 pos0 = {x1 * cz - y1 * sz + x, x1 * sz + y1 * cz + y};
  ns::vec2 pos1 = {x2 * cz - y1 * sz + x, x2 * sz + y1 * cz + y};
  ns::vec2 pos2 = {x2 * cz - y2 * sz + x, x2 * sz + y2 * cz + y};
  ns::vec2 pos3 = {x1 * cz - y2 * sz + x, x1 * sz + y2 * cz + y};
  f32 angle = ns::point_direction(pos0, pos1);
  ns::vec2 diag2 = ns::lengthdir_vec(1.f / 2.f, angle + 90);
  ns::vec2 diag1 = ns::vec2(-diag2.x, diag2.y);

  disable_zwrite();
  anm_use_default_texture();

  u32 tlX = AM->batch->add_vertex({{pos0 - diag1, 0.f}, col1, {}});
  u32 tlI = AM->batch->add_vertex({{pos0 + diag1, 0.f}, col1, {}});
  u32 trX = AM->batch->add_vertex({{pos1 - diag2, 0.f}, col2, {}});
  u32 trI = AM->batch->add_vertex({{pos1 + diag2, 0.f}, col2, {}});
  u32 brX = AM->batch->add_vertex({{pos2 - diag1, 0.f}, col2, {}});
  u32 brI = AM->batch->add_vertex({{pos2 + diag1, 0.f}, col2, {}});
  u32 blX = AM->batch->add_vertex({{pos3 - diag2, 0.f}, col1, {}});
  u32 blI = AM->batch->add_vertex({{pos3 + diag2, 0.f}, col1, {}});

  AM->batch->draw_quad(tlX, tlI, blX, blI);
  AM->batch->draw_quad(trX, trI, tlI, tlX);
  AM->batch->draw_quad(brI, brX, trI, trX);
  AM->batch->draw_quad(blX, blI, brI, brX);
  return 0;
  // flush_vbos();
  some_d3d_stuff();
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, cursor, 0x14);
  // AM->vertex_buffers.primitive_write_cursor += 5;
  AM->draw_call_ctr_0xcc++;
  return 0;
}

i32 draw_vm__mode_17__drawCircle(f32 x, f32 y, f32 angle_0, f32 radius,
                                 i32 vertex_count, ns::Color color_1,
                                 ns::Color color_2) {
  disable_zwrite();
  disable_ztest();
  anm_use_default_texture();
  u32 center = AM->batch->add_vertex({{x, y, 0.0f}, color_1, {}});
  f32 angle_inc = ns::PI_2<f32> / vertex_count;
  for (i32 i = 0; i < vertex_count - 1; i++) {
    ns::vec2 pos = ns::lengthdir_vec(radius, angle_0) + ns::vec2(x, y);
    u32 pt = AM->batch->add_vertex({{pos, 0.0}, color_2, {}});
    AM->batch->draw_tri(center, pt, pt + 1);
    angle_0 += angle_inc;
    ns::angle_normalize(angle_0);
  }
  ns::vec2 pos = ns::lengthdir_vec(radius, angle_0) + ns::vec2(x, y);
  u32 pt = AM->batch->add_vertex({{pos, 0.0}, color_2, {}});
  AM->batch->draw_tri(center, pt, center + 1);
  some_d3d_stuff();
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertex_count, vertex_buffers.primitive_write_cursor, 0x14);
  // AM->vertex_buffers.primitive_write_cursor += vertex_count + 2;
  AM->draw_call_ctr_0xcc++;
  return 0;
}

i32 draw_vm__mode_18__drawCircleBorder(f32 x, f32 y, f32 angle_0, f32 radius,
                                       i32 nb_vertex, ns::Color col) {
  anm_use_default_texture();

  u32 first = AM->batch->get_next_vid();
  f32 angle_inc = ns::PI_2<f32> / nb_vertex;

  for (i32 i = 0; i < nb_vertex - 1; i++) {
    ns::vec2 pos = ns::lengthdir_vec(radius + 0.5, angle_0) + ns::vec2(x, y);
    u32 v1 = AM->batch->add_vertex({{pos, 0.0}, col, {}});
    pos = ns::lengthdir_vec(radius - 0.5, angle_0) + ns::vec2(x, y);
    u32 v2 = AM->batch->add_vertex({{pos, 0.0}, col, {}});
    AM->batch->draw_quad(v1, v2, v2 + 2, v1 + 2);

    angle_0 += angle_inc;
    ns::angle_normalize(angle_0);
  }

  ns::vec2 pos = ns::lengthdir_vec(radius + 0.5, angle_0) + ns::vec2(x, y);
  u32 v1 = AM->batch->add_vertex({{pos, 0.0}, col, {}});
  pos = ns::lengthdir_vec(radius - 0.5, angle_0) + ns::vec2(x, y);
  u32 v2 = AM->batch->add_vertex({{pos, 0.0}, col, {}});
  AM->batch->draw_quad(v1, v2, first + 1, first);
  some_d3d_stuff();
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_LINESTRIP, nb_vertex, vertex_buffers.primitive_write_cursor, 0x14);
  // AM->vertex_buffers.primitive_write_cursor += nb_vertex + 1;
  AM->draw_call_ctr_0xcc++;
  return 0;
}

i32 draw_vm__mode_19__drawRing(f32 x, f32 y, f32 angle_0, f32 radius,
                               f32 thickness, i32 vertex_count, ns::Color col) {
  f32 radius_1 = radius - thickness * 0.5;
  f32 radius_2 = thickness * 0.5 + radius;
  f32 angle_inc = ns::PI_2<f32> / vertex_count;
  u32 first = AM->batch->get_next_vid();

  for (i32 i = 0; i < vertex_count - 1; i++) {
    ns::vec2 pos = ns::lengthdir_vec(radius_2, angle_0) + ns::vec2(x, y);
    u32 v1 = AM->batch->add_vertex({{pos, 0.0}, col, {}});
    pos = ns::lengthdir_vec(radius_1, angle_0) + ns::vec2(x, y);
    u32 v2 = AM->batch->add_vertex({{pos, 0.0}, col, {}});
    AM->batch->draw_quad(v1, v2, v2 + 2, v1 + 2);

    angle_0 += angle_inc;
    ns::angle_normalize(angle_0);
  }

  ns::vec2 pos = ns::lengthdir_vec(radius_2, angle_0) + ns::vec2(x, y);
  u32 v1 = AM->batch->add_vertex({{pos, 0.0}, col, {}});
  pos = ns::lengthdir_vec(radius_1, angle_0) + ns::vec2(x, y);
  u32 v2 = AM->batch->add_vertex({{pos, 0.0}, col, {}});
  AM->batch->draw_quad(v1, v2, first + 1, first);
  some_d3d_stuff();
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, vertex_count * 2, vertex_buffers.primitive_write_cursor, 0x14);
  // AM->vertex_buffers.primitive_write_cursor += vertex_count * 2 + 2;
  AM->draw_call_ctr_0xcc++;
  return 0;
}

void draw_vm_mode_24_25(VM *vm, void *buff, i32 cnt) {
  if (!(vm->bitflags.visible) || !(vm->bitflags.f530_1))
    return;
  if (vm->bitflags.zwritedis) {
    disable_zwrite();
  } else {
    enable_zwrite();
  }

  vm->__matrix_1 = ns::mat4(1.f);
  vm->__matrix_2 = ns::mat::scale(vm->__matrix_1, {vm->scale * vm->scale_2, 1});
  vm->bitflags.scaled = false;
  if (vm->rotation.z != 0.0)
    vm->__matrix_2 = vm->__matrix_2 * ns::mat::mk4_rotate_z(vm->rotation.z);
  if (vm->rotation.y != 0.0)
    vm->__matrix_2 = vm->__matrix_2 * ns::mat::mk4_rotate_y(vm->rotation.y);
  if (vm->rotation.x != 0.0)
    vm->__matrix_2 = vm->__matrix_2 * ns::mat::mk4_rotate_x(vm->rotation.x);
  vm->bitflags.rotated = false;
  ns::mat4 DStack_e0 = vm->__matrix_2;
  DStack_e0[3][0] = vm->entity_pos.x + vm->pos.x + vm->__pos_2.x; // - vm->anchor_offset.x * vm->scale.x * vm->scale_2.x;
  DStack_e0[3][1] = vm->entity_pos.y + vm->pos.y + vm->__pos_2.y; // - vm->anchor_offset.y * vm->scale.y * vm->scale_2.y;
  DStack_e0[3][2] = vm->entity_pos.z + vm->pos.z + vm->__pos_2.z;
  if (!vm->parent_vm) {
    DStack_e0[3][0] += AM->origins[vm->bitflags.originMode].x;
  }
  anm_use_texture(vm);
  setup_render_state_for_vm(vm);

  i32 nb_segments = (cnt - 2) / 2;
  auto cursor = reinterpret_cast<RenderVertex_t *>(buff);
  // std::vector<RenderVertex_t> data(cursor, &cursor[cnt]);
  // for (auto &v : data) {
  //   v.transformed_pos = DStack_e0 * v.transformed_pos;
  // }

  // for (i32 i = 0; i < nb_segments; i++) {
  //   auto &tl = data[i * 2 + 0];
  //   auto &tr = data[i * 2 + 1];
  //   auto &bl = data[i * 2 + 2];
  //   auto &br = data[i * 2 + 3];
  //   AM->batch->draw_quad({ns::vec3(tl.transformed_pos), tl.diffuse_color, tl.texture_uv},
  //                        {ns::vec3(tr.transformed_pos), tr.diffuse_color, tr.texture_uv},
  //                        {ns::vec3(br.transformed_pos), br.diffuse_color, br.texture_uv},
  //                        {ns::vec3(bl.transformed_pos), bl.diffuse_color, bl.texture_uv});
  // }

  u32 tl = AM->batch->add_vertex({ns::vec3(DStack_e0 * cursor[0].transformed_pos), cursor[0].diffuse_color, cursor[0].texture_uv});
  u32 tr = AM->batch->add_vertex({ns::vec3(DStack_e0 * cursor[1].transformed_pos), cursor[1].diffuse_color, cursor[1].texture_uv});

  for (i32 i = 0; i < nb_segments; i++) {
    auto *here = &cursor[i * 2 + 2];
    auto bl = AM->batch->add_vertex({ns::vec3(DStack_e0 * here[0].transformed_pos), here[0].diffuse_color, here[0].texture_uv});
    auto br = AM->batch->add_vertex({ns::vec3(DStack_e0 * here[1].transformed_pos), here[1].diffuse_color, here[1].texture_uv});
    AM->batch->draw_quad(tl, tr, br, bl);
    tl = bl;
    tr = br;
  }
  return;

  /// Actual
  // if (some_positions[0] != vm->getSprite() || vm->uv_scroll_pos.x != 0.0 ||
  //     vm->uv_scale.x != 1.0 || vm->uv_scale.y != 1.0) {
  //   some_positions[0] = vm->getSprite();
  //   matrix = vm->__matrix_3;
  //   matrix.m20 = vm->uv_quad_of_sprite[0].x + vm->uv_scroll_pos.x;
  //   matrix.m21 = vm->uv_quad_of_sprite[0].y + vm->uv_scroll_pos.y;
  //   matrix.m00 = vm->uv_scale.x * vm->__matrix_3.m00;
  //   matrix.m11 = vm->uv_scale.y * vm->__matrix_3.m11;
  //   SUPERVISOR.d3d_device->SetTransform(D3DTS_TEXTURE0, &matrix);
  // }
  // if (this->field28_0x18607ca != 5) {
  //   SUPERVISOR.d3d_device->SetFVF(0x142);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
  //   this->field28_0x18607ca = 5;
  // }
  // if (ANM_MANAGER_PTR->field33_0x18607cf != 1) {
  //   SUPERVISOR.d3d
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, 4);
  //   ANM_MANAGER_PTR->field33_0x18607cf = 1;
  // }
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, cnt - 2, buff,
  //                                        0x18);
  // return;
}

void set_effect_508(usize i, int16_t anm_loaded_index, int16_t script_index,
                    i32 index_of_on_create, i32 index_of_on_tick,
                    i32 index_of_on_draw, i32 index_of_on_destroy,
                    i32 index_of_on_interrupt, i32 index_of_on_copy_1__disused,
                    i32 index_of_on_copy_2__disused) {
  if (i >= AM->effect_table.size())
    AM->effect_table.resize(i + 1);
  AM->effect_table[i].anm_loaded_index = anm_loaded_index;
  AM->effect_table[i].script_index = script_index;
  AM->effect_table[i].index_of_on_create = index_of_on_create;
  AM->effect_table[i].index_of_on_tick = index_of_on_tick;
  AM->effect_table[i].index_of_on_draw = index_of_on_draw;
  AM->effect_table[i].index_of_on_destroy = index_of_on_destroy;
  AM->effect_table[i].index_of_on_interrupt = index_of_on_interrupt;
  AM->effect_table[i].index_of_on_copy_1__disused = index_of_on_copy_1__disused;
  AM->effect_table[i].index_of_on_copy_2__disused = index_of_on_copy_2__disused;
}

ID createVM508(i32 i, VM *root) {
  if (AM->effect_table.size() <= static_cast<usize>(i) ||
      AM->effect_table[i].anm_loaded_index < 0)
    return 0;
  if (-1 < AM->effect_table[i].script_index) {
    auto vm = root;
    if (!vm) {
      AM->loadedFiles[AM->effect_table[i].anm_loaded_index].createEffect(
          AM->effect_table[i].script_index, -1, &vm);
    }
    if (AM->effect_table[i].index_of_on_create &&
        Funcs::on_create(AM->effect_table[i].index_of_on_create))
      Funcs::on_create(AM->effect_table[i].index_of_on_create)(vm);
    vm->index_of_on_tick = AM->effect_table[i].index_of_on_tick;
    vm->index_of_on_draw = AM->effect_table[i].index_of_on_draw;
    vm->index_of_on_destroy = AM->effect_table[i].index_of_on_destroy;
    vm->index_of_on_interrupt = AM->effect_table[i].index_of_on_interrupt;
    vm->index_of_on_copy_1__disused =
        AM->effect_table[i].index_of_on_copy_1__disused;
    vm->index_of_on_copy_2__disused =
        AM->effect_table[i].index_of_on_copy_2__disused;
    return vm->id;
  }
  return 0;
}

void disable_fog() {
  if (AM->fog_enabled) {
    flush_vbos();
    AM->fog_enabled = false;
    AM->curr_shader = AM->shader; // d3d: FOGENABLE
  }
}

void enable_fog() {
  if (!AM->fog_enabled) {
    flush_vbos();
    AM->fog_enabled = true;
    AM->curr_shader = AM->fshader; // d3d: FOGENABLE
  }
}

void set_fog_params(ns::Color col, f32 beg, f32 end) {
  if (AM->curr_shader == AM->fshader)
    flush_vbos();
  AM->fshader->use();
  AM->fshader->SetFog(
      beg, end, {col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f});
}

void disable_atest() {
  if (AM->curr_shader != AM->bshader) {
    flush_vbos();
    AM->last_shader_before_no_atest_save = AM->curr_shader;
    AM->curr_shader = AM->bshader;
  }
}
void enable_atest() {
  if (AM->curr_shader == AM->bshader) {
    flush_vbos();
    AM->curr_shader = AM->last_shader_before_no_atest_save;
  }
}

void disable_zwrite() {
  if (AM->zwrite_enabled) {
    flush_vbos();
    AM->zwrite_enabled = false;
    glDepthMask(false); // d3d: ZWRITEENABLE
  }
}

void enable_zwrite() {
  if (!AM->zwrite_enabled) {
    flush_vbos();
    AM->zwrite_enabled = true;
    glDepthMask(true); // d3d: ZWRITEENABLE
  }
}

void disable_ztest() {
  if (AM->ztest_enabled) {
    flush_vbos();
    AM->ztest_enabled = false;
    glDisable(GL_DEPTH_TEST); // d3d: ZFUNC 8
  }
}

void enable_ztest() {
  if (!AM->ztest_enabled) {
    flush_vbos();
    AM->ztest_enabled = true;
    glEnable(GL_DEPTH_TEST); // d3d: ZFUNC 4
  }
}

void set_camera(Camera_t *cam) {
  AM->current_camera = cam;
  flush_vbos();
  AM->cam_vec2_fc_x = AM->current_camera->screen_shake.x;
  AM->cam_vec2_fc_y = AM->current_camera->screen_shake.y;
  // d3d: SetTransform
  AM->shader->use();
  AM->shader->set_view_matrix(AM->current_camera->view_matrix);
  AM->shader->set_projection_matrix(AM->current_camera->projection_matrix);
  AM->fshader->use();
  AM->fshader->SetViewMatrix(AM->current_camera->view_matrix);
  AM->fshader->SetProjectionMatrix(AM->current_camera->projection_matrix);
  AM->fshader->SetCameraPosition(AM->current_camera->position);
  AM->bshader->use();
  AM->bshader->set_view_matrix(AM->current_camera->view_matrix);
  AM->bshader->set_projection_matrix(AM->current_camera->projection_matrix);
  set_viewport(AM->current_camera->viewport);
  return;
}

Camera_t *get_camera() { return AM->current_camera; }

void set_viewport(Viewport_t viewport) {
  glViewport(viewport.X, viewport.Y, viewport.Width, viewport.Height);
}

void set_viewport(Camera_t &camera) { set_viewport(camera.viewport); }

void clear_framebuffer_color(ns::Color c) {
  glClearColor(c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void clear_framebuffer_depth(f32 d) {
  glClearDepth(d);
  glClear(GL_DEPTH_BUFFER_BIT);
}

void clear_framebuffer(ns::Color c, f32 d) {
  glClearColor(c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f);
  glClearDepth(d);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void enable_blending(bool val) {
  if (val)
    glEnable(GL_BLEND);
  else
    glDisable(GL_BLEND);
}

void enable_culling(bool val) {
  if (val)
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);
}

void reset_blend_eq() { glBlendEquation(GL_FUNC_ADD); }

File *getLoaded(i32 i) { return &AM->loadedFiles[i]; }

void interrupt_tree(ID id, i32 interrupt) {
  auto vm = get_vm(id.val);
  if (vm)
    vm->interruptRec(interrupt);
}

void interrupt_tree_run(ID id, i32 interrupt) {
  auto vm = get_vm(id.val);
  if (vm)
    vm->interruptRecRun(interrupt);
}

void reset_texture() { AM->last_used_texture = -1; }

void draw_rect_col(ns::vec4 const &rect, ns::Color c) {
  flush_vbos();
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_ALPHAOP,2)
  //;
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_COLOROP,2)
  //;
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_ALPHAARG1,0);
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_COLORARG1,0);
  //(*(SUPERVISOR.d3d_device)->vtable->SetRenderState)(SUPERVISOR.d3d_device,D3DRS_DESTBLEND,6);
  //(*(SUPERVISOR.d3d_device)->vtable->SetFVF)(SUPERVISOR.d3d_device,0x44);
  //(*(SUPERVISOR.d3d_device)->vtable->DrawPrimitiveUP)(SUPERVISOR.d3d_device,D3DPT_TRIANGLESTRIP,2,vertices,0x14);
  AM->batch->draw_quad({{rect.x, rect.y, 0.f}, c, {0.f, 0.f}},
                       {{rect.z, rect.y, 0.f}, c, {1.f, 0.f}},
                       {{rect.z, rect.w, 0.f}, c, {1.f, 1.f}},
                       {{rect.x, rect.w, 0.f}, c, {0.f, 1.f}});
  flush_vbos();
  AM->field_0x18607c9 = 255;
  AM->field_0x18607ca = 255;
  AM->field_0x18607cb = 255;
  AM->last_used_blendmode = 10;
  AM->last_used_texture = -1;
  AM->some_positions[0] = 0.0;
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_ALPHAOP,4)
  //;
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_COLOROP,4)
  //;
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_ALPHAARG1,2);
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_COLORARG1,2);
  return;
}

void draw_rect_col(ns::vec4 const &rect, int a, int rgb) {
  ns::Color c = {static_cast<uint8_t>((rgb >> 0x10) & 0xff),
                 static_cast<uint8_t>((rgb >> 0x08) & 0xff),
                 static_cast<uint8_t>((rgb >> 0x00) & 0xff),
                 static_cast<uint8_t>(a & 0xff)};
  draw_rect_col(rect, c);
}

void reset_batch_state() {
  AM->some_positions[0] = 0.0;
  AM->last_used_texture = -1;
  AM->last_used_blendmode = 10;
  AM->field_0x18607c9 = 255;
  AM->field_0x18607ca = 255;
  AM->field_0x18607cb = 255;
  AM->field_0x18607cc = 255;
  AM->field_0x18607cf = 255;
  AM->custom_color_1c90a48.b = 128;
  AM->custom_color_1c90a48.g = 128;
  AM->custom_color_1c90a48.r = 128;
  AM->custom_color_1c90a48.a = 128;
  AM->use_custom_color_1c90a4c = 0;
  AM->last_used_resamplemode = 255;
  AM->cam_vec2_fc_y = 0.0;
  AM->cam_vec2_fc_x = 0.0;
}

ns::vec2 &origin(usize i) { return AM->origins[i]; }

Camera_t *get_3d_camera() { return AM->_3d_camera; }

void set_3d_camera(Camera_t *c) { AM->_3d_camera = c; }

void use_custom_color(ns::Color c) {
  AM->custom_color_1c90a48 = c;
  AM->use_custom_color_1c90a4c = true;
}

void unuse_custom_color() {
  AM->custom_color_1c90a48 = {128, 128, 128, 128};
  AM->use_custom_color_1c90a4c = false;
}

void _set_cam_vec2_fc(ns::vec2 v) {
  AM->cam_vec2_fc_x = v.x;
  AM->cam_vec2_fc_y = v.y;
}

void raw_batch_draw(u32 texture, RenderVertex_t tl, RenderVertex_t tr,
                    RenderVertex_t br, RenderVertex_t bl, u8 blendmode) {
  anm_use_texture(texture);
  anm_use_blendmode(blendmode);
  ns::Vertex ns_tl = {ns::vec3(tl.transformed_pos), tl.diffuse_color,
                      tl.texture_uv};
  ns::Vertex ns_tr = {ns::vec3(tr.transformed_pos), tr.diffuse_color,
                      tr.texture_uv};
  ns::Vertex ns_br = {ns::vec3(br.transformed_pos), br.diffuse_color,
                      br.texture_uv};
  ns::Vertex ns_bl = {ns::vec3(bl.transformed_pos), bl.diffuse_color,
                      bl.texture_uv};
  AM->batch->draw_quad(ns_tl, ns_tr, ns_br, ns_bl);
}

void raw_batch_draw(ns::Texture *texture, RenderVertex_t tl, RenderVertex_t tr,
                    RenderVertex_t br, RenderVertex_t bl, u8 blendmode) {
  anm_use_texture(texture);
  anm_use_blendmode(blendmode);
  ns::Vertex ns_tl = {ns::vec3(tl.transformed_pos), tl.diffuse_color,
                      tl.texture_uv};
  ns::Vertex ns_tr = {ns::vec3(tr.transformed_pos), tr.diffuse_color,
                      tr.texture_uv};
  ns::Vertex ns_br = {ns::vec3(br.transformed_pos), br.diffuse_color,
                      br.texture_uv};
  ns::Vertex ns_bl = {ns::vec3(bl.transformed_pos), bl.diffuse_color,
                      bl.texture_uv};
  AM->batch->draw_quad(ns_tl, ns_tr, ns_br, ns_bl);
}

} // namespace anm
