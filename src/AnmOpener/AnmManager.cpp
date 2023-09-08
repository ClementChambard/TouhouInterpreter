#include "AnmManager.h"
#include "../Supervisor.h"
#include "AnmBitflags.h"
#include "./AnmFuncs.h"
#include "../UpdateFuncRegistry.h"
#include <DrawFuncs.h>
#include <cstring>
#include <NSlist.h>
#include <TextureManager.h>
#include <glm/common.hpp>
// #include "../Supervisor.h"

// zThread
// zAnmSaveRelated[4] pause_related
// undefined4[2]
int32_t AnmManager::processed_anm_ctr_0xc8 = 0;
int32_t AnmManager::draw_call_ctr_0xcc = 0;
float AnmManager::cam_vec2_fc_x = 0.f;
float AnmManager::cam_vec2_fc_y = 0.f;
int32_t AnmManager::render_loop_ctr_0xd8 = 0;
AnmVM AnmManager::__anm_vm_dc{};
AnmVMList_t *AnmManager::world_list_head = nullptr;
AnmVMList_t *AnmManager::world_list_tail = nullptr;
AnmVMList_t *AnmManager::ui_list_head = nullptr;
AnmVMList_t *AnmManager::ui_list_tail = nullptr;
AnmFastVM AnmManager::fastArray[8191]{};
int32_t AnmManager::__lolk_next_snapshot_fast_id = 0;
int32_t AnmManager::__lolk_next_snapshot_discriminator = 0;
AnmVMList_t AnmManager::__lolk_vm_snapshot_list_head{};
AnmFastVMList_t AnmManager::freelist_head{};
// undefined4
std::array<AnmFile, 32> AnmManager::loadedFiles;
glm::mat4 AnmManager::__matrix_186017c = glm::mat4(1.f);
AnmVM AnmManager::__anm_vm_18601bc{};
int32_t AnmManager::field_0x18607bc = 0;
NSEngine::Color AnmManager::field_0x18607c0 = {0, 0, 0, 0};
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
float AnmManager::some_positions[22]{};
AnmVertexBuffers_t AnmManager::vertex_buffers{};
// dummy head (unused)
int32_t AnmManager::last_id_discriminator = 1;
NSEngine::Color AnmManager::custom_color_1c90a48{};
int32_t AnmManager::use_custom_color_1c90a4c = false;

GLuint AnmManager::vboID = 0;
NSEngine::SpriteBatch* AnmManager::batch;
AnmShader* AnmManager::shader;
BlitShader* AnmManager::bshader;
FogShader* AnmManager::fshader;
NSEngine::ShaderProgram* AnmManager::curr_shader;

void AnmManager::bindBuffer() {
  glBindBuffer(GL_ARRAY_BUFFER, vboID);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(
      0, 4, GL_FLOAT, GL_FALSE, sizeof(RenderVertex_t),
      reinterpret_cast<void *>(offsetof(RenderVertex_t, transformed_pos)));
  glVertexAttribPointer(
      1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RenderVertex_t),
      reinterpret_cast<void *>(offsetof(RenderVertex_t, diffuse_color)));
  glVertexAttribPointer(
      2, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex_t),
      reinterpret_cast<void *>(offsetof(RenderVertex_t, texture_uv)));
}

void AnmManager::drawBuffer(RenderVertex_t *start, uint32_t count) {
  // SUPERVISOR.d3d_device->SetFVF(0x144);
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
  // vertex_buffers.unrendered_sprite_count * 2,
  // vertex_buffers.sprite_render_cursor, 0x1c);
  glBufferData(GL_ARRAY_BUFFER, count * sizeof(RenderVertex_t), nullptr,
               GL_DYNAMIC_DRAW); // orphan the buffer
  glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(RenderVertex_t),
                  start); // upload the data
  glDrawArrays(GL_TRIANGLES, 0, count);
}

void AnmManager::unbindBuffer() { glBindBuffer(GL_ARRAY_BUFFER, 0); }


static void anmmanager_initialize() {
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
  UpdateFunc* func;
  func = new UpdateFunc(AnmManager::on_tick_world);
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_tick(func, 35);
  func = new UpdateFunc(AnmManager::on_tick_ui);
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_tick(func, 10);
  func = new UpdateFunc([](){return AnmManager::render_layer(0);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 5);
  func = new UpdateFunc([](){return AnmManager::render_layer(1);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 7);
  func = new UpdateFunc([](){return AnmManager::render_layer(2);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 9);
  func = new UpdateFunc([](){return AnmManager::render_layer(4);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 11);
  func = new UpdateFunc([](){
    SUPERVISOR.cameras[3].as_2d_matrix();
    SUPERVISOR.set_camera_by_index(3);
    SUPERVISOR.disable_d3d_fog();
    return AnmManager::render_layer(3);
  });
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 10);
  func = new UpdateFunc([](){return AnmManager::render_layer(5);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 13);
  func = new UpdateFunc([](){return AnmManager::render_layer(6);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 16);
  func = new UpdateFunc([](){return AnmManager::render_layer(7);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 18);
  func = new UpdateFunc([](){return AnmManager::render_layer(8);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 20);
  func = new UpdateFunc([](){return AnmManager::render_layer(9);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 21);
  func = new UpdateFunc([](){return AnmManager::render_layer(10);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 22);
  func = new UpdateFunc([](){return AnmManager::render_layer(11);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 24);
  func = new UpdateFunc([](){return AnmManager::render_layer(12);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 27);
  func = new UpdateFunc([](){return AnmManager::render_layer(13);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 28);
  func = new UpdateFunc([](){return AnmManager::render_layer(14);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 31);
  func = new UpdateFunc([](){return AnmManager::render_layer(15);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 32);
  func = new UpdateFunc([](){return AnmManager::render_layer(16);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 35);
  func = new UpdateFunc([](){return AnmManager::render_layer(17);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 37);
  func = new UpdateFunc([](){return AnmManager::render_layer(18);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 40);
  func = new UpdateFunc([](){return AnmManager::render_layer(19);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 43);
  func = new UpdateFunc([](){
    SUPERVISOR.set_camera_by_index(1);
    SUPERVISOR.disable_zwrite();
    SUPERVISOR.disable_ztest();
    return AnmManager::render_layer(20);
  });
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 46);
  func = new UpdateFunc([](){return AnmManager::render_layer(21);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 47);
  func = new UpdateFunc([](){return AnmManager::render_layer(22);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 53);
  func = new UpdateFunc([](){return AnmManager::render_layer(23);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 55);
  func = new UpdateFunc([](){return AnmManager::render_layer(26);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 62);
  func = new UpdateFunc([](){return AnmManager::render_layer(27);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 63);
  func = new UpdateFunc([](){return AnmManager::render_layer(29);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 78);
  func = new UpdateFunc([](){return AnmManager::render_layer(30);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 80);
  func = new UpdateFunc([](){return AnmManager::render_layer(31);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 83);
  func = new UpdateFunc([](){return AnmManager::render_layer(25);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 60);
  func = new UpdateFunc([](){
    SUPERVISOR.set_camera_by_index(2);
    SUPERVISOR.disable_zwrite();
    SUPERVISOR.disable_ztest();
    AnmManager::cam_vec2_fc_x = 0.0f;
    AnmManager::cam_vec2_fc_y = 0.0f;
    return AnmManager::render_layer(24);
  });
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 59);
  func = new UpdateFunc([](){
    SUPERVISOR.set_camera_by_index(0);
    return AnmManager::render_layer(28);
    SUPERVISOR.set_camera_by_index(2);
  });
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 65);
  func = new UpdateFunc([](){
    SUPERVISOR.set_camera_by_index(2);
    SUPERVISOR.disable_zwrite();
    SUPERVISOR.disable_ztest();
    return AnmManager::render_layer(36);
  });
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 56);
  func = new UpdateFunc([](){return AnmManager::render_layer(37);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 61);
  func = new UpdateFunc([](){return AnmManager::render_layer(38);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 64);
  func = new UpdateFunc([](){
    SUPERVISOR.set_camera_by_index(0);
    return AnmManager::render_layer(39);
    SUPERVISOR.set_camera_by_index(2);
  });
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 66);
  func = new UpdateFunc([](){return AnmManager::render_layer(40);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 79);
  func = new UpdateFunc([](){return AnmManager::render_layer(41);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 81);
  func = new UpdateFunc([](){return AnmManager::render_layer(42);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 84);
}


void AnmManager::Init() {
  for (size_t i = 0; i < 8191; i++) {
    fastArray[i].vm.reset();
    fastArray[i].fastID = i;
    fastArray[i].vm.fast_id = i;
    fastArray[i].isAlive = false;
    fastArray[i].freelistNode.value = &fastArray[i];
    ListUtil::listInsertAfter(&freelist_head, &fastArray[i].freelistNode);
  }

  some_positions[1] = 0.0;
  last_used_texture = -1;
  last_used_blendmode = 0;
  field_0x18607c9 = 0;
  field_0x18607ca = 0;
  field_0x18607cb = 0;
  field_0x18607c0 = {0, 0, 0, 1};
  field_0x18607cc = 255;
  // pause_related[0].anm_loaded = -1;
  // pause_related[1].anm_loaded = -1;
  // pause_related[2].anm_loaded = -1;
  // pause_related[3].anm_loaded = -1;

  // glGenBuffers(1, &vboID);
  batch = new NSEngine::SpriteBatch();
  batch->Init();
  shader = new AnmShader();
  bshader = new BlitShader();
  fshader = new FogShader();
  curr_shader = shader;

  anmmanager_initialize();
}

void AnmManager::Cleanup() {
  /* DESTROY HEAP VMS IN WORLD LIST */
  while (world_list_head) {
    auto n = world_list_head;
    world_list_head = world_list_head->next;
    if ((n->value->id.val & AnmID::fastIdMask) == AnmID::fastIdMask)
      delete n->value;
  }
  world_list_tail = nullptr;

  /* DESTROY HEAP VMS IN UI LIST */
  while (ui_list_head) {
    auto n = ui_list_head;
    ui_list_head = ui_list_head->next;
    if ((n->value->id.val & AnmID::fastIdMask) == AnmID::fastIdMask)
      delete n->value;
  }
  ui_list_tail = nullptr;

  /* CLEAN FAST VMS */
  for (size_t i = 0; i < 8191; i++)
    fastArray[i].vm.destroy();

  /* CLEAN PREINIT VM */
  for (auto &f : loadedFiles)
    f.Cleanup();

  if (vboID)
    glDeleteBuffers(1, &vboID);

  batch->Dispose();
  delete batch;
  delete shader;
  delete bshader;
  delete fshader;
}

AnmVM *AnmManager::allocate_vm() {
  if (freelist_head.next) {
    auto fvm = freelist_head.next->value;
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
  auto vm = new AnmVM();
  vm->sprite_id = -1;
  vm->instr_offset = -1;
  vm->reset();
  vm->fast_id = 0x3fff;
  return vm;
}

AnmID AnmManager::insert_in_world_list_back(AnmVM *vm) {
  vm->node_in_global_list = {vm, nullptr, nullptr};
  if (!world_list_head) {
    world_list_head = &vm->node_in_global_list;
  } else {
    world_list_tail->next = &vm->node_in_global_list;
    vm->node_in_global_list.previous = world_list_tail;
  }
  world_list_tail = &vm->node_in_global_list;

  last_id_discriminator++;
  last_id_discriminator &= 0x3ffff;
  if (last_id_discriminator == 0) {
    last_id_discriminator = 1;
  }
  vm->id.setDiscriminator(last_id_discriminator);
  vm->id.setFastId(vm->fast_id);
  return vm->id;
}

AnmID AnmManager::insert_in_world_list_front(AnmVM *vm) {
  vm->node_in_global_list = {vm, nullptr, nullptr};
  if (!world_list_head) {
    world_list_tail = &vm->node_in_global_list;
  } else {
    world_list_head->previous = &vm->node_in_global_list;
    vm->node_in_global_list.next = world_list_head;
  }
  world_list_head = &vm->node_in_global_list;

  last_id_discriminator++;
  last_id_discriminator &= 0x3ffff;
  if (last_id_discriminator == 0) {
    last_id_discriminator = 1;
  }
  vm->id.setDiscriminator(last_id_discriminator);
  vm->id.setFastId(vm->fast_id);
  return vm->id;
}

AnmID AnmManager::insert_in_ui_list_back(AnmVM *vm) {
  vm->node_in_global_list = {vm, nullptr, nullptr};
  if (!ui_list_head) {
    ui_list_head = &vm->node_in_global_list;
  } else {
    ui_list_tail->next = &vm->node_in_global_list;
    vm->node_in_global_list.previous = ui_list_tail;
  }
  ui_list_tail = &vm->node_in_global_list;

  last_id_discriminator++;
  last_id_discriminator &= 0x3ffff;
  if (last_id_discriminator == 0) {
    last_id_discriminator = 1;
  }
  vm->id.setDiscriminator(last_id_discriminator);
  vm->id.setFastId(vm->fast_id);
  return vm->id;
}

AnmID AnmManager::insert_in_ui_list_front(AnmVM *vm) {
  vm->node_in_global_list = {vm, nullptr, nullptr};
  if (!ui_list_head) {
    ui_list_tail = &vm->node_in_global_list;
  } else {
    ui_list_head->previous = &vm->node_in_global_list;
    vm->node_in_global_list.next = ui_list_head;
  }
  ui_list_head = &vm->node_in_global_list;

  last_id_discriminator++;
  last_id_discriminator &= 0x3ffff;
  if (last_id_discriminator == 0) {
    last_id_discriminator = 1;
  }
  vm->id.setDiscriminator(last_id_discriminator);
  vm->id.setFastId(vm->fast_id);
  return vm->id;
}

void AnmManager::recreate_vm(AnmID& id, int new_script) {
  auto vm = getVM(id);
  if (vm) {
    vm->bitflags.visible = false;
    vm->instr_offset = -1;
    id = loadedFiles[vm->anm_loaded_index].createEffect(new_script);
  }
}

void AnmManager::killAll() {
  /* DESTROY HEAP VMS IN WORLD LIST */
  while (world_list_head) {
    auto n = world_list_head->value;
    world_list_head = world_list_head->next;
    if ((n->id.val & AnmID::fastIdMask) == AnmID::fastIdMask)
      delete n;
  }
  world_list_tail = nullptr;

  /* DESTROY HEAP VMS IN UI LIST */
  while (ui_list_head) {
    auto n = ui_list_head->value;
    ui_list_head = ui_list_head->next;
    if ((n->id.val & AnmID::fastIdMask) == AnmID::fastIdMask)
      delete n;
  }

  /* CLEAN FAST VMS */
  for (size_t i = 0; i < 8191; i++)
    fastArray[i].vm.destroy();

  /* CLEAN PREINIT VM */
  // for (auto f : loadedFiles)
  // f.Cleanup();
}

void AnmManager::deleteVM(uint32_t id) {
  if (!id)
    return;
  deleteVM(getVM(id));
}

void AnmManager::deleteVM(AnmVM *vm) {
  if (!vm)
    return;
  if (vm->bitflags.f534_27_31)
    return;
  vm->bitflags.activeFlags = 0b01;
  for (auto node = vm->list_of_children.next; node; node = node->next) {
    deleteVM(node->value);
  }
}

void AnmManager::delete_of_file(AnmFile *f) {
  if (!f)
    return;
  int id = -1;
  for (int i = 0; i < 32; i++) {
    if (&loadedFiles[i] == f) {
      id = i;
      break;
    }
  }
  if (id == -1)
    return;
  // TODO
  // for (auto node = world_list_head; node; node = node->next)
  //     if (node->value->anm_loaded_index == id)
  //         deleteVM(node->value);
}

bool AnmManager::isAlive(uint32_t id) {
  if ((id & AnmID::fastIdMask) == AnmID::fastIdMask) {
    for (auto node = world_list_head; node; node = node->next)
      if (node->value->id == id)
        return true;
    for (auto node = ui_list_head; node; node = node->next)
      if (node->value->id == id)
        return true;
    return false;
  }
  if (fastArray[id & AnmID::fastIdMask].vm.id != id)
    return false;
  return fastArray[id & AnmID::fastIdMask].isAlive;
}

AnmVM *AnmManager::getVM(uint32_t id) {
  if (id == 0)
    return nullptr;
  if ((id & AnmID::fastIdMask) == AnmID::fastIdMask) {
    for (auto node = world_list_head; node; node = node->next)
      if (node->value->id == id)
        return node->value;
    for (auto node = ui_list_head; node; node = node->next)
      if (node->value->id == id)
        return node->value;
    return nullptr;
  }
  if (fastArray[id & AnmID::fastIdMask].vm.id != id)
    return nullptr;
  if (fastArray[id & AnmID::fastIdMask].isAlive)
    return &(fastArray[id & AnmID::fastIdMask].vm);
  return nullptr;
}
#include <fstream>

AnmFile *AnmManager::LoadFile(size_t slot, std::string filename) {
  std::ifstream ifile;
  if (filename[filename.size() - 4] != '.' ||
      filename[filename.size() - 3] != 'a' ||
      filename[filename.size() - 2] != 'n' ||
      filename[filename.size() - 1] != 'm') {
    filename += ".anm";
  }
  ifile.open(filename);
  if (!ifile) {
    std::cout << filename << ": No such file\n";
    return nullptr;
  }
  ifile.close();
  if (loadedFiles[slot].name != "notLoaded") { /* do something */
    loadedFiles[slot].Cleanup();
  }
  loadedFiles[slot].Open(filename, slot);
  return &loadedFiles[slot];
}

void AnmManager::update(bool /*printInstr*/) {
  on_tick_ui();
  on_tick_world();
}

int AnmManager::getFreeAnm() {
  int n = 0;
  for (int i = 0; i < 8191; i++)
    if (!fastArray[i].isAlive)
      n++;
  return n;
}

void AnmManager::prepare_vm_for_delete(AnmVM *vm, AnmVMList_t *list) {
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

int AnmManager::destroy_possibly_managed_vm(AnmVM *vm) {
  if (&vm->node_in_global_list == world_list_tail) {
    world_list_tail = vm->node_in_global_list.previous;
  }
  if (&vm->node_in_global_list == world_list_head) {
    world_list_head = vm->node_in_global_list.next;
  }
  if (&vm->node_in_global_list == ui_list_tail) {
    ui_list_tail = vm->node_in_global_list.previous;
  }
  if (&vm->node_in_global_list == ui_list_head) {
    ui_list_head = vm->node_in_global_list.next;
  }
  if (vm->index_of_on_destroy) {
      ANM_ON_DESTROY_FUNCS[vm->index_of_on_destroy](vm);
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
  if (vm->id.getFastId() == AnmID::fastIdMask) {
    delete vm;
    return 0;
  }
  fastArray[vm->fast_id].isAlive = false;
  if (freelist_head.next) {
    fastArray[vm->fast_id].freelistNode.next = freelist_head.next;
    freelist_head.next->previous = &fastArray[vm->fast_id].freelistNode;
  }
  freelist_head.next = &fastArray[vm->fast_id].freelistNode;
  fastArray[vm->fast_id].freelistNode.previous = &freelist_head;
  if (vm->special_vertex_buffer_data) {
    free(vm->special_vertex_buffer_data);
  }
  vm->special_vertex_buffer_data = nullptr;
  vm->special_vertex_buffer_size = 0;
  vm->id = 0;
  vm->instr_offset = -1;
  return 0;
}

int AnmManager::on_tick_world() {
  // for (int i = 0; i < 0x24; i++) {
  //     layer_list_dummy_heads__unused_as_of_1[i].next_in_layer__disused =
  //     nullptr;
  // }

  AnmVMList_t todelete = {};

  for (auto node = world_list_head; node; node = node->next) {
    node->value->new_field_added_in_1_00b = 0;
    node->value->next_in_layer__disused = 0;
    if (node->value->bitflags.activeFlags == 0b01 ||
        (node->value->bitflags.activeFlags == 0b00 && node->value->update())) {
      prepare_vm_for_delete(node->value, &todelete);
    }
  }

  AnmVMList_t *next = nullptr;
  for (auto node = todelete.next; node; node = next) {
    next = node->next;
    destroy_possibly_managed_vm(node->value);
  }

  return 1;
}

int AnmManager::on_tick_ui() {
  // for (int i = 0x24; i < 0x2b; i++) {
  //     layer_list_dummy_heads__unused_as_of_1[i].next_in_layer__disused =
  //     nullptr;
  // }
  render_loop_ctr_0xd8 = 0;
  AnmVMList_t todelete = {};

  for (auto node = ui_list_head; node; node = node->next) {
    node->value->new_field_added_in_1_00b = 0;
    node->value->next_in_layer__disused = 0;
    if (node->value->bitflags.activeFlags == 0b01 ||
        (node->value->bitflags.activeFlags == 0b00 && node->value->update())) {
      prepare_vm_for_delete(node->value, &todelete);
    }
  }

  AnmVMList_t *next = nullptr;
  for (auto node = todelete.next; node; node = next) {
    next = node->next;
    destroy_possibly_managed_vm(node->value);
  }

  return 1;
}

void AnmManager::draw() {
  for (auto node = world_list_head; node; node = node->next) {
    if (node->value->bitflags.activeFlags)
      continue;
    auto l = node->value->layer;
    auto oldl = l;
    if (node->value->layer <= 42 && node->value->layer >= 36) {
      l -= 12;
    }
    node->value->layer = l;
    node->value->draw();
    node->value->layer = oldl;
  }
  for (auto node = ui_list_head; node; node = node->next) {
    if (node->value->bitflags.activeFlags)
      continue;
    auto l = node->value->layer;
    auto oldl = l;
    if (24 <= l && l < 32) {
      l += 12;
    } else if (l < 36 || l > 42) {
      l = 38;
    }
    node->value->layer = l;
    node->value->draw();
    node->value->layer = oldl;
  }
}

void AnmManager::on_draw(uint32_t layer) { render_layer(layer); }

int AnmManager::render_layer(uint32_t layer) {
  for (auto node = world_list_head; node; node = node->next) {
    if (node->value->bitflags.activeFlags)
      continue;
    auto l = node->value->layer;
    if (node->value->layer <= 42 && node->value->layer >= 36) {
      l -= 12;
    }
    if (l == layer) {
      drawVM(node->value);
    }
    render_loop_ctr_0xd8++;
  }
  for (auto node = ui_list_head; node; node = node->next) {
    if (node->value->bitflags.activeFlags)
      continue;
    auto l = node->value->layer;
    if (l <= 31) {
      l += 12;
    } else if (l < 36 || l > 42) {
      l = 38;
    }
    if (l == layer) {
      drawVM(node->value);
    }
    render_loop_ctr_0xd8++;
  }
  return 1;
}

RenderVertex_t SPRITE_TEMP_BUFFER[4];
float RESOLUTION_MULT = 1.f;

void AnmManager::flush_vbos() {
  batch->end();
  curr_shader->start();
  batch->renderBatch();
  curr_shader->stop();
  batch->begin();
  return;
  // TODO: REAL
  if (vertex_buffers.unrendered_sprite_count != 0) {
    // if (field_0x18607cf != 1) {
    //  IDK
    // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP,
    // D3DTOP_MODULATE); SUPERVISOR.d3d_device->SetTextureStageState(0,
    // D3DTSS_COLOROP, D3DTOP_MODULATE);
    field_0x18607cf = 1;
    //}
    // IDK
    // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
    // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);

    // RENDER
    drawBuffer(vertex_buffers.sprite_render_cursor,
               vertex_buffers.unrendered_sprite_count * 6);
    draw_call_ctr_0xcc++;
    vertex_buffers.sprite_render_cursor = vertex_buffers.sprite_write_cursor;
    vertex_buffers.unrendered_sprite_count = 0;
  }
}

void AnmManager::setup_render_state_for_vm(AnmVM *vm) {
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

void AnmManager::write_sprite(const RenderVertex_t *buffer) {
  if ((uint64_t)&vertex_buffers.sprite_write_cursor <=
  (uint64_t)vertex_buffers.sprite_write_cursor + 6)
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

void AnmManager::calc_mat_world(AnmVM *vm) {
  if (!vm->bitflags.f530_16) {
    vm->__matrix_2 = glm::scale(vm->__matrix_1, {vm->scale_2 * vm->scale, 1});
    vm->bitflags.scaled = false;
    if (vm->rotation.x)
      vm->__matrix_2 = glm::rotate(vm->__matrix_2, vm->rotation.x, {1, 0, 0});
    if (vm->rotation.y)
      vm->__matrix_2 = glm::rotate(vm->__matrix_2, vm->rotation.y, {0, 1, 0});
    if (vm->rotation.z)
      vm->__matrix_2 = glm::rotate(vm->__matrix_2, vm->rotation.z, {0, 0, 1});
    vm->bitflags.rotated = false;
  }
  glm::vec3 p = vm->entity_pos + vm->pos + vm->__pos_2;
  if (vm->bitflags.originMode && !vm->parent_vm) {
    p.x += DAT_00524710 * 0.5;
    p.y += (DAT_00524714 - 448.0) * 0.5;
  }
  if (vm->parent_vm && !vm->bitflags.noParent)
    p +=
        vm->parent_vm->entity_pos + vm->parent_vm->pos + vm->parent_vm->__pos_2;
  __matrix_186017c = glm::translate(vm->__matrix_2, p);
}

void AnmManager::drawVM(AnmVM *vm) {
  if (vm->index_of_on_draw) ANM_ON_DRAW_FUNCS[vm->index_of_on_draw](vm);

  if (!vm->bitflags.visible || !vm->bitflags.f530_1 ||
      vm->bitflags.activeFlags)
    return;

  // auto saved_a1 = vm->color_1.a;
  // auto saved_a2 = vm->color_2.a;
  // if (vm->bitflags.fadeNearCamera) {
  //   float d = math::point_distance(
  //     {vm->pos + vm->__pos_2 + vm->entity_pos}, SUPERVISOR.current_camera->position);
  //   float factor = (d - vm->cam_fade_begin) / (vm->cam_fade_end-vm->cam_fade_begin);
  //   if (factor > 1.f) factor = 1.f;
  //   if (factor < 0.f) factor = 0.f;
  //   vm->color_1.a *= factor;
  //   vm->color_2.a *= factor;
  // }

  SUPERVISOR.disable_zwrite();

  /* switch render mode */
  switch (vm->bitflags.rendermode) {
  case 0:
    if (vm->color_1.a == 0 && vm->color_2.a == 0)
      break;
    vm->write_sprite_corners__without_rot_o(
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
    vm->write_sprite_corners__with_z_rot_o(
                                         SPRITE_TEMP_BUFFER[0].transformed_pos,
                                         SPRITE_TEMP_BUFFER[1].transformed_pos,
                                         SPRITE_TEMP_BUFFER[2].transformed_pos,
                                         SPRITE_TEMP_BUFFER[3].transformed_pos);
    draw_vm__modes_0_1_2_3(vm, 0);
    break;
  case 2:
    if (vm->color_1.a == 0 && vm->color_2.a == 0)
      break;
    vm->write_sprite_corners__without_rot_o(
        SPRITE_TEMP_BUFFER[0].transformed_pos,
        SPRITE_TEMP_BUFFER[1].transformed_pos,
        SPRITE_TEMP_BUFFER[2].transformed_pos,
        SPRITE_TEMP_BUFFER[3].transformed_pos);
    draw_vm__modes_0_1_2_3(vm, 0);
    break;
  case 4:
    if (vm->color_1.a == 0 && vm->color_2.a == 0)
      break;
    if (!vm->write_sprite_corners__mode_4_o())
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
    auto col [[maybe_unused]] = vm->bitflags.colmode ? vm->color_2 :
    vm->color_1; for (int i = 0; i < 4; i++) {
        glm::vec3 v = __matrix_186017c *
        glm::vec4(some_positions[2], some_positions[3], some_positions[4], 1);
        if (math::point_distance(v, SUPERVISOR.current_camera->position) <=
        SUPERVISOR.current_camera->sky.begin_distance) {
            SPRITE_TEMP_BUFFER[i].diffuse_color = col;
        } else {
            float coeff = (SUPERVISOR.current_camera->sky.begin_distance -
            math::point_distance(v, SUPERVISOR.current_camera->position)) /
            (SUPERVISOR.current_camera->sky.begin_distance -
            SUPERVISOR.current_camera->sky.end_distance); if (coeff < 1.0) {
                SPRITE_TEMP_BUFFER[i].diffuse_color.b = col.b - ((col.b -
                SUPERVISOR.current_camera->sky.color_components[0]) * coeff);
                SPRITE_TEMP_BUFFER[i].diffuse_color.g = col.g - ((col.g -
                SUPERVISOR.current_camera->sky.color_components[1]) * coeff);
                SPRITE_TEMP_BUFFER[i].diffuse_color.r = col.r - ((col.r -
                SUPERVISOR.current_camera->sky.color_components[2]) * coeff);
                SPRITE_TEMP_BUFFER[i].diffuse_color.a = col.a;
            } else {
                SPRITE_TEMP_BUFFER[i].diffuse_color =
                SUPERVISOR.current_camera->sky.color;
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
    draw_vm_triangle_strip(vm,
            reinterpret_cast<RenderVertex_t*>(vm->special_vertex_buffer_data),
            vm->int_script_vars[0] * 2);
    break;
  case 0xb:
    draw_vm_triangle_fan(vm,
            reinterpret_cast<RenderVertex_t*>(vm->special_vertex_buffer_data),
            vm->int_script_vars[0] * 2);
    break;
  case 0xf:
    if (vm->color_1.a == 0 && vm->color_2.a == 0)
      break;
    SUPERVISOR.enable_d3d_fog();
    draw_vm__mode_8_15(vm);
    SUPERVISOR.disable_d3d_fog();
    break;
  case 0x10:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x1a:
  case 0x1b: {
    float rot = vm->rotation.z;
    float xs = vm->sprite_size.x * vm->scale.x;
    float ys = vm->sprite_size.y * vm->scale.y;
    glm::vec3 pos = vm->pos + vm->entity_pos + vm->__pos_2;
    vm->transform_coordinate_o(pos);
    if (vm->parent_vm && !vm->bitflags.noParent) {
        xs *= vm->parent_vm->scale.x;
        ys *= vm->parent_vm->scale.y;
        rot += vm->parent_vm->rotation.z;
    }
    setup_render_state_for_vm(vm);
    if (vm->bitflags.resolutionMode == 1) ys *= RESOLUTION_MULT;
    else if (vm->bitflags.resolutionMode == 2) ys *= RESOLUTION_MULT / 2.0;
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
    float rot = vm->rotation.z;
    float xs = vm->sprite_size.x * vm->scale.x;
    float ys = vm->sprite_size.y * vm->scale.y;
    glm::vec3 pos = vm->pos + vm->entity_pos + vm->__pos_2;
    vm->transform_coordinate_o(pos);
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
            vm->int_script_vars[0], vm->color_1, vm->color_2);
    } else if (vm->bitflags.rendermode == 0x12) {
        draw_vm__mode_18__drawCircleBorder(pos.x, pos.y, rot, xs,
            vm->int_script_vars[0], vm->color_1);
    } else if (vm->bitflags.rendermode == 0x13) {
        draw_vm__mode_19__drawRing(pos.x, pos.y, rot, xs, ys,
            vm->int_script_vars[0], vm->color_1);
    }
  }
    break;
  case 0x18:
  case 0x19:
    draw_vm_mode_24_25(vm, vm->special_vertex_buffer_data,
        vm->int_script_vars[0] * 2);
    break;
  }

  // vm->color_1.a = saved_a1;
  // vm->color_2.a = saved_a2;
}

void AnmManager::draw_vm__modes_0_1_2_3(AnmVM *vm, int i) {
  SPRITE_TEMP_BUFFER[0].transformed_pos.x += cam_vec2_fc_x;
  SPRITE_TEMP_BUFFER[0].transformed_pos.y += cam_vec2_fc_y;
  SPRITE_TEMP_BUFFER[1].transformed_pos.x += cam_vec2_fc_x;
  SPRITE_TEMP_BUFFER[1].transformed_pos.y += cam_vec2_fc_y;
  SPRITE_TEMP_BUFFER[2].transformed_pos.x += cam_vec2_fc_x;
  SPRITE_TEMP_BUFFER[2].transformed_pos.y += cam_vec2_fc_y;
  SPRITE_TEMP_BUFFER[3].transformed_pos.x += cam_vec2_fc_x;
  SPRITE_TEMP_BUFFER[3].transformed_pos.y += cam_vec2_fc_y;
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
    SPRITE_TEMP_BUFFER[3].transformed_pos.x))
        < SUPERVISOR.current_camera->viewport.X) return;

    /* if max of y pos is smaller than top of viewport --> discard drawing */
    if (fmax(fmax(SPRITE_TEMP_BUFFER[0].transformed_pos.y,
    SPRITE_TEMP_BUFFER[1].transformed_pos.y),
    fmax(SPRITE_TEMP_BUFFER[2].transformed_pos.y,
    SPRITE_TEMP_BUFFER[3].transformed_pos.y))
        < SUPERVISOR.current_camera->viewport.Y) return;

    /* if min of x pos is larger than right of viewport --> discard drawing */
    if (fmin(fmin(SPRITE_TEMP_BUFFER[0].transformed_pos.x,
    SPRITE_TEMP_BUFFER[1].transformed_pos.x),
    fmin(SPRITE_TEMP_BUFFER[2].transformed_pos.x,
    SPRITE_TEMP_BUFFER[3].transformed_pos.x))
        > SUPERVISOR.current_camera->viewport.Width +
        SUPERVISOR.current_camera->viewport.X) return;

    /* if min of y pos is larger than bottom of viewport --> discard drawing */
    if (fmin(fmin(SPRITE_TEMP_BUFFER[0].transformed_pos.y,
    SPRITE_TEMP_BUFFER[1].transformed_pos.y),
    fmin(SPRITE_TEMP_BUFFER[2].transformed_pos.y,
    SPRITE_TEMP_BUFFER[3].transformed_pos.y))
        > SUPERVISOR.current_camera->viewport.Height +
        SUPERVISOR.current_camera->viewport.Y) return;
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

  if (last_used_texture != vm->getSprite().opengl_texid) {
    last_used_texture = vm->getSprite().opengl_texid;
    flush_vbos();
    glBindTexture(GL_TEXTURE_2D, last_used_texture);
    last_used_scrollmodex = 255;
    last_used_scrollmodey = 255;
    last_used_resamplemode = 255;
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
  // write_sprite(SPRITE_TEMP_BUFFER);
  NSEngine::Vertex tl = {SPRITE_TEMP_BUFFER[0].transformed_pos,
                         SPRITE_TEMP_BUFFER[0].diffuse_color,
                         SPRITE_TEMP_BUFFER[0].texture_uv};
  NSEngine::Vertex tr = {SPRITE_TEMP_BUFFER[2].transformed_pos,
                         SPRITE_TEMP_BUFFER[2].diffuse_color,
                         SPRITE_TEMP_BUFFER[2].texture_uv};
  NSEngine::Vertex br = {SPRITE_TEMP_BUFFER[3].transformed_pos,
                         SPRITE_TEMP_BUFFER[3].diffuse_color,
                         SPRITE_TEMP_BUFFER[3].texture_uv};
  NSEngine::Vertex bl = {SPRITE_TEMP_BUFFER[1].transformed_pos,
                         SPRITE_TEMP_BUFFER[1].diffuse_color,
                         SPRITE_TEMP_BUFFER[1].texture_uv};
  batch->draw(vm->getSprite().texID, tl, tr, br, bl, last_used_blendmode);
}

extern glm::vec2 BACK_BUFFER_SIZE;

int AnmManager::draw_vm__mode_6(AnmVM *vm) {
  if (vm->write_sprite_corners__mode_4_o() != 0) {
    return -1;
  }
  glm::vec3 pos_from_cam = vm->entity_pos + vm->pos + vm->__pos_2 -
                         SUPERVISOR.current_camera->position;
  if (vm->bitflags.originMode != 0 && !vm->parent_vm) {
    pos_from_cam += glm::vec3(BACK_BUFFER_SIZE.x * 0.5,
                            (BACK_BUFFER_SIZE.y - 448.0) * 0.5, 0);
  }
  float dist_to_cam = math::point_distance({0, 0, 0}, pos_from_cam);
  float fog_factor;
  NSEngine::Color col;
  NSEngine::Color col2;
  switch (vm->bitflags.colmode) {
  default:
    col = vm->bitflags.colmode == 0 ? vm->color_1 : vm->color_2;
    if (use_custom_color_1c90a4c) {
      col = NSEngine::mix(col, custom_color_1c90a48);
    }
    if (dist_to_cam <= SUPERVISOR.current_camera->sky.begin_distance) {
      SPRITE_TEMP_BUFFER[0].diffuse_color = col;
      SPRITE_TEMP_BUFFER[1].diffuse_color = col;
      SPRITE_TEMP_BUFFER[2].diffuse_color = col;
      SPRITE_TEMP_BUFFER[3].diffuse_color = col;
      draw_vm__modes_0_1_2_3(vm, 6);
      return 0;
    }

    fog_factor =
         (dist_to_cam - SUPERVISOR.current_camera->sky.begin_distance) /
         (SUPERVISOR.current_camera->sky.end_distance -
          SUPERVISOR.current_camera->sky.begin_distance);
    if (fog_factor > 1.0)
      return -1;
    SPRITE_TEMP_BUFFER[0].diffuse_color.r =
        col.r - (col.r - SUPERVISOR.current_camera->sky.color_components[2]) *
                    fog_factor;
    SPRITE_TEMP_BUFFER[0].diffuse_color.g =
        col.g - (col.g - SUPERVISOR.current_camera->sky.color_components[1]) *
                    fog_factor;
    SPRITE_TEMP_BUFFER[0].diffuse_color.b =
        col.b - (col.b - SUPERVISOR.current_camera->sky.color_components[0]) *
                    fog_factor;
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
    if (use_custom_color_1c90a4c) {
      col = NSEngine::mix(col, custom_color_1c90a48);
      col2 = NSEngine::mix(col2, custom_color_1c90a48);
    }
    SPRITE_TEMP_BUFFER[3].diffuse_color = col;
    SPRITE_TEMP_BUFFER[0].diffuse_color = col2;
    if (SUPERVISOR.current_camera->sky.begin_distance < dist_to_cam) {
      fog_factor =
          (SUPERVISOR.current_camera->sky.begin_distance - dist_to_cam) /
          (SUPERVISOR.current_camera->sky.begin_distance -
           SUPERVISOR.current_camera->sky.end_distance);
      if (1.0 <= fog_factor) {
        return -1;
      }
      SPRITE_TEMP_BUFFER[0].diffuse_color.r =
          col.r - (col.r - SUPERVISOR.current_camera->sky.color_components[2]) *
                      fog_factor;
      SPRITE_TEMP_BUFFER[0].diffuse_color.g =
          col.g - (col.g - SUPERVISOR.current_camera->sky.color_components[1]) *
                      fog_factor;
      SPRITE_TEMP_BUFFER[0].diffuse_color.b =
          col.b - (col.b - SUPERVISOR.current_camera->sky.color_components[0]) *
                      fog_factor;
      SPRITE_TEMP_BUFFER[0].diffuse_color.a = (col.a * (1.0 - fog_factor));
      SPRITE_TEMP_BUFFER[3].diffuse_color.r =
          col2.r -
          (col2.r - SUPERVISOR.current_camera->sky.color_components[2]) *
              fog_factor;
      SPRITE_TEMP_BUFFER[3].diffuse_color.g =
          col2.g -
          (col2.g - SUPERVISOR.current_camera->sky.color_components[1]) *
              fog_factor;
      SPRITE_TEMP_BUFFER[3].diffuse_color.b =
          col2.b -
          (col2.b - SUPERVISOR.current_camera->sky.color_components[0]) *
              fog_factor;
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

void AnmManager::draw_vm__mode_8_15(AnmVM *vm) {
  if (!vm->bitflags.visible || !vm->bitflags.f530_1 ||
      vm->color_1.a == 0) {
    return;
  }
  if (vertex_buffers.unrendered_sprite_count != 0) {
    flush_vbos();
  }
  if (vm->bitflags.zwritedis) {
    SUPERVISOR.disable_zwrite();
  } else {
    SUPERVISOR.enable_zwrite();
  }
  if (vm->bitflags.f530_16)
    goto LAB_0046ecb8;
  vm->__matrix_2 =
      scale(vm->__matrix_1, {vm->scale_2 * vm->scale, 1});
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
    if (vm->rotation.x != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.x, {1, 0, 0}) * vm->__matrix_2;
    if (vm->rotation.y != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.y, {0, 1, 0}) * vm->__matrix_2;
    if (vm->rotation.z != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.z, {0, 0, 1}) * vm->__matrix_2;
    break;
  case 1:
    if (vm->rotation.x != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.x, {1, 0, 0}) * vm->__matrix_2;
    if (vm->rotation.z != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.z, {0, 0, 1}) * vm->__matrix_2;
    if (vm->rotation.y != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.y, {0, 1, 0}) * vm->__matrix_2;
    break;
  case 2:
    if (vm->rotation.y != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.y, {0, 1, 0}) * vm->__matrix_2;
    if (vm->rotation.x != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.x, {1, 0, 0}) * vm->__matrix_2;
    if (vm->rotation.z != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.z, {0, 0, 1}) * vm->__matrix_2;
    break;
  case 3:
    if (vm->rotation.y != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.y, {0, 1, 0}) * vm->__matrix_2;
    if (vm->rotation.z != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.z, {0, 0, 1}) * vm->__matrix_2;
    if (vm->rotation.x != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.x, {1, 0, 0}) * vm->__matrix_2;
    break;
  case 4:
    if (vm->rotation.z != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.z, {0, 0, 1}) * vm->__matrix_2;
    if (vm->rotation.x != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.x, {1, 0, 0}) * vm->__matrix_2;
    if (vm->rotation.y != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.y, {0, 1, 0}) * vm->__matrix_2;
    break;
  case 5:
    if (vm->rotation.z != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.z, {0, 0, 1}) * vm->__matrix_2;
    if (vm->rotation.y != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.y, {0, 1, 0}) * vm->__matrix_2;
    if (vm->rotation.x != 0.0) vm->__matrix_2 = glm::rotate(glm::mat4(1.f), vm->rotation.x, {1, 0, 0}) * vm->__matrix_2;
    break;
  }
  vm->bitflags.rotated = false;
LAB_0046ecb8:
  glm::mat4 local_90 = vm->__matrix_2;
  local_90[3][0] =
      vm->entity_pos.x + vm->pos.x + vm->__pos_2.x -
      vm->anchor_offset.x * vm->scale.x *
          vm->scale_2.x;
  local_90[3][1] = vm->entity_pos.y + vm->pos.y +
               vm->__pos_2.y -
              vm->anchor_offset.y * vm->scale.y *
                  vm->scale_2.y;
  glm::vec3 pp = {local_90[3][0], local_90[3][1], local_90[3][2]};
  vm->transform_coordinate_o(pp);
  local_90[3][0] = pp.x;
  local_90[3][1] = pp.y;
  local_90[3][2] = pp.z;
  NSEngine::Color col = vm->color_1;
  if (vm->bitflags.colmode != 0) {
    col = vm->color_2;
  }
  if (use_custom_color_1c90a4c != 0) {
    col = NSEngine::mix(col, custom_color_1c90a48);
  }
  // if (field_0x18607c0 != col) {
  //   flush_vbos();
  //   field_0x18607c0 = col;
  //   SUPERVISOR.d3d_device->SetRenderState(D3DRS_TEXTUREFACTOR, col);
  // }
  local_90[3][2] = vm->entity_pos.z + vm->pos.z + vm->__pos_2.z;
  // SUPERVISOR.d3d_device->SetTransform(0x100, (D3DMATRIX *)local_90);
  if (last_used_texture != vm->getSprite().opengl_texid) {
    last_used_texture = vm->getSprite().opengl_texid;
    flush_vbos();
    glBindTexture(GL_TEXTURE_2D, last_used_texture);
    last_used_scrollmodex = 255;
    last_used_scrollmodey = 255;
    last_used_resamplemode = 255;
    // SUPERVISOR.d3d_device->SetTexture(0, vm->getSprite().opengl_texid);
  }

  setup_render_state_for_vm(vm);

  // My version
  float l = vm->bitflags.anchorX != 0 ? -(vm->bitflags.anchorX - 1) : -0.5f;
  float r = l + 1;
  float t = vm->bitflags.anchorY != 0 ? -(vm->bitflags.anchorY - 1) : -0.5f;
  float b = t + 1;
  float top = t * 256;
  float bottom = b * 256;
  float left = l * 256;
  float right = r * 256;
  float u1 = (vm->getSprite().u1 + vm->uv_scroll_pos.x) * vm->uv_scale.x;
  float u2 = (vm->getSprite().u2 + vm->uv_scroll_pos.x) * vm->uv_scale.x;
  float v1 = (vm->getSprite().v1 + vm->uv_scroll_pos.y) * vm->uv_scale.y;
  float v2 = (vm->getSprite().v2 + vm->uv_scroll_pos.y) * vm->uv_scale.y;

  NSEngine::Vertex tl = { { local_90 * glm::vec4(left, top, 0, 1) }, col, { u1, v1 } };
  NSEngine::Vertex tr = { { local_90 * glm::vec4(right, top, 0, 1) }, col, { u2, v1 } };
  NSEngine::Vertex br = { { local_90 * glm::vec4(right, bottom, 0, 1) }, col, { u2, v2 } };
  NSEngine::Vertex bl = { { local_90 * glm::vec4(left, bottom, 0, 1) }, col, { u1, v2 } };
  batch->draw(vm->getSprite().texID, tl, tr, br, bl, last_used_blendmode);
  // TODO: real ?
  // What I did is equivalent to below (don't understand TextureStageState though)

  // if ((/*(some_positions[0] != vm->getSprite()) ||*/
  //      (vm->uv_scroll_pos.x != 0.0)) ||
  //     ((vm->uv_scale.x != 1.0 ||
  //       (vm->uv_scale.y != 1.0)))) {
  //   some_positions[0] = fVar11;
  //   glm::mat4 local_d0 = vm->__matrix_3;
  //   local_d0.m00 *= vm->uv_scale.x;
  //   local_d0.m11 *= vm->uv_scale.y;
  //   local_d0.m20 = vm->uv_quad_of_sprite[0].x +
  //                  vm->uv_scroll_pos.x;
  //   local_d0.m21 = vm->uv_quad_of_sprite[0].y +
  //                  vm->uv_scroll_pos.y;
  //   SUPERVISOR.d3d_device->SetTransform(D3DTS_TEXTURE0, &local_d0);
  // }
  // if (field28_0x18607ca != 2) {
  //   SUPERVISOR.d3d_device->SetStreamSource(0, this->some_positions[1], 0, 0x14);
  //   SUPERVISOR.d3d_device->SetFVF(0x102);
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
  // SUPERVISOR.d3d_device->vtable->DrawPrimitive( 5, ((uVar7 >> 0x17 & 3) * 3 + (uVar7 >> 0x15 & 3)) * 4, 2);
}

void AnmManager::draw_vm_triangle_fan(AnmVM *vm, RenderVertex_t *vertexData,
                                      int nVert) {
  if (vertex_buffers.unrendered_sprite_count != 0) {
    flush_vbos();
  }
  // if (field_0x18607ca != 3) {
  //   SUPERVISOR.d3d_device->SetFVF(0x144);
  //   field_0x18607ca = 3;
  // }
  setup_render_state_for_vm(vm);
  if (last_used_texture != vm->getSprite().opengl_texid) {
    last_used_texture = vm->getSprite().opengl_texid;
    flush_vbos();
    // SUPERVISOR.d3d_device->SetTexture(0, last_used_texture);
    last_used_scrollmodex = 255;
    last_used_scrollmodey = 255;
    last_used_resamplemode = 255;
    glBindTexture(GL_TEXTURE_2D, last_used_texture);
  }
  SUPERVISOR.disable_zwrite();
  // if (field_0x18607ca != 1) {
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
  //   field_0x18607ca = 1;
  // }
  // draw data in buffer
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, nVert - 2,
  //                                        vertexData, 0x1c);
  // My version
  int nQuad = (nVert - 2)/2;
  for (int i = 0; i < nQuad; i++) {
    auto tl = vertexData[0];
    auto tr = vertexData[i*2+1];
    auto br = vertexData[i*2+2];
    auto bl = vertexData[i*2+3];
    batch->draw(vm->getSprite().texID,
      {tl.transformed_pos, tl.diffuse_color, {0, 0}},
      {tr.transformed_pos, tr.diffuse_color, {1, 0}},
      {br.transformed_pos, br.diffuse_color, {1, 1}},
      {bl.transformed_pos, bl.diffuse_color, {0, 1}},
      last_used_blendmode);
  }
}

void AnmManager::draw_vm_triangle_strip(AnmVM *vm,
                                             RenderVertex_t *vertexData,
                                             int nVert) {
  if (!vm->bitflags.visible || !vm->bitflags.f530_1 || vm->color_1.a == 0)
    return;
  if (vertex_buffers.unrendered_sprite_count != 0) {
    flush_vbos();
  }
  if (last_used_texture != vm->getSprite().opengl_texid) {
    last_used_texture = vm->getSprite().opengl_texid;
    flush_vbos();
    last_used_scrollmodex = 255;
    last_used_scrollmodey = 255;
    last_used_resamplemode = 255;
    // SUPERVISOR.d3d_device->SetTexture(0, vm->getSprite().opengl_texid);
    glBindTexture(GL_TEXTURE_2D, last_used_texture);
  }
  // if (field_0x18607ca != 3) {
  //   SUPERVISOR.d3d_device->SetFVF(0x144);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
  //   field_0x18607ca = 3;
  // }
  setup_render_state_for_vm(vm);
  // if (field_0x18607cf != 1) {
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, 4);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, 4);
  //   field_0x18607cf = 1;
  // }
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, nVert - 2,
  //                                        vertexData, 0x1c);
  // My version
  int nb_segments = (nVert - 2) / 2;
  for (int i = 0; i < nb_segments; i++) {
    auto tl = vertexData[i*2+0];
    auto tr = vertexData[i*2+1];
    auto bl = vertexData[i*2+2];
    auto br = vertexData[i*2+3];
    batch->draw(vm->getSprite().texID,
      {tl.transformed_pos, tl.diffuse_color, tl.texture_uv},
      {tr.transformed_pos, tr.diffuse_color, tr.texture_uv},
      {br.transformed_pos, br.diffuse_color, br.texture_uv},
      {bl.transformed_pos, bl.diffuse_color, bl.texture_uv},
      last_used_blendmode);
  }
  return;
}

int AnmManager::draw_vm__ins_607(float x, float y, float width, float height,
                                 float rot_z, NSEngine::Color col1,
                                 NSEngine::Color col2,
                                 int anchorX, int anchorY) {
  draw_vm__ins_603(x, y, width + 1.0, height + 1.0, rot_z,
                   {col1.r, col1.g, col1.b, static_cast<uint8_t>(col1.a / 2)},
                   {col2.r, col2.g, col2.b, static_cast<uint8_t>(col2.a / 2)},
                   anchorX, anchorY);
  draw_vm__ins_603(x, y, width, height, rot_z, col1, col2, anchorX, anchorY);
  return 0;
}

int AnmManager::draw_vm__ins_603(float x, float y, float width, float height,
                                 float rot_z, NSEngine::Color col1,
                                 NSEngine::Color col2,
                                 int anchorX, int anchorY) {
  if (reinterpret_cast<uint64_t>(vertex_buffers.primitive_write_cursor + 4) >=
      reinterpret_cast<uint64_t>(&vertex_buffers.primitive_write_cursor))
    return 0;
  float x1 = anchorX == 0 ? -width / 2.0 : anchorX == 1 ? 0.0 : -width;
  float x2 = anchorX == 0 ? width / 2.0 : anchorX == 1 ? width : 0.0;
  float y1 = anchorY == 0 ? -height / 2.0 : anchorY == 1 ? 0.0 : -height;
  float y2 = anchorY == 0 ? height / 2.0 : anchorY == 1 ? height : 0.0;
  auto cursor = vertex_buffers.primitive_write_cursor;
  cursor[0].transformed_pos.x = x1 * cos(rot_z) - y1 * sin(rot_z) + x;
  cursor[1].transformed_pos.x = x2 * cos(rot_z) - y1 * sin(rot_z) + x;
  cursor[2].transformed_pos.x = x1 * cos(rot_z) - y2 * sin(rot_z) + x;
  cursor[3].transformed_pos.x = x2 * cos(rot_z) - y2 * sin(rot_z) + x;
  cursor[0].transformed_pos.y = x1 * sin(rot_z) + y1 * cos(rot_z) + y;
  cursor[1].transformed_pos.y = x2 * sin(rot_z) + y1 * cos(rot_z) + y;
  cursor[2].transformed_pos.y = x1 * sin(rot_z) + y2 * cos(rot_z) + y;
  cursor[3].transformed_pos.y = x2 * sin(rot_z) + y2 * cos(rot_z) + y;
  cursor[0].diffuse_color = col1;
  cursor[1].diffuse_color = col2;
  cursor[2].diffuse_color = col1;
  cursor[3].diffuse_color = col2;
  cursor[0].transformed_pos.z = 0.0;
  cursor[1].transformed_pos.z = 0.0;
  cursor[2].transformed_pos.z = 0.0;
  cursor[3].transformed_pos.z = 0.0;
  cursor[0].transformed_pos.w = 1.0;
  cursor[1].transformed_pos.w = 1.0;
  cursor[2].transformed_pos.w = 1.0;
  cursor[3].transformed_pos.w = 1.0;
  SUPERVISOR.disable_zwrite();
  // My version
  if (last_used_texture != 1) {
    last_used_texture = 1;
    last_used_scrollmodex = 255;
    last_used_scrollmodey = 255;
    last_used_resamplemode = 255;
    flush_vbos();
    glBindTexture(GL_TEXTURE_2D, 1);
  }
  batch->draw(last_used_texture,
              {cursor[0].transformed_pos, cursor[0].diffuse_color, {0, 0}},
              {cursor[1].transformed_pos, cursor[1].diffuse_color, {0, 0}},
              {cursor[3].transformed_pos, cursor[3].diffuse_color, {0, 0}},
              {cursor[2].transformed_pos, cursor[2].diffuse_color, {0, 0}},
              last_used_blendmode);
  return 0;
  flush_vbos();
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
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(
  //     D3DPT_TRIANGLESTRIP, 2, this->vertex_buffers.primitive_write_cursor,
  //     0x14);
  // vertex_buffers.primitive_write_cursor += 4;
  // draw_call_ctr_0xcc++;
  return 0;
}

int AnmManager::draw_vm__ins_613(float x, float y, float width, float rot_z,
                                 NSEngine::Color col1,
                                 NSEngine::Color col2, int anchor) {
  if (reinterpret_cast<uint64_t>(vertex_buffers.primitive_write_cursor + 2) >=
      reinterpret_cast<uint64_t>(&vertex_buffers.primitive_write_cursor))
    return 0;
  float x1 = anchor == 0 ? -width / 2.0 : anchor == 1 ? 0.0 : -width;
  float x2 = anchor == 0 ? width / 2.0 : anchor == 1 ? width : 0.0;
  auto cursor = vertex_buffers.primitive_write_cursor;
  cursor[0].transformed_pos.x = x1 * cos(rot_z) + x;
  cursor[1].transformed_pos.x = x2 * cos(rot_z) + x;
  cursor[0].transformed_pos.y = x1 * sin(rot_z) + y;
  cursor[1].transformed_pos.y = x2 * sin(rot_z) + y;
  cursor[0].transformed_pos.z = 0.0;
  cursor[1].transformed_pos.z = 0.0;
  cursor[0].transformed_pos.w = 1.0;
  cursor[1].transformed_pos.w = 1.0;
  cursor[0].diffuse_color = col1;
  cursor[1].diffuse_color = col2;
  SUPERVISOR.disable_zwrite();
  if (last_used_texture != 1) {
    last_used_texture = 1;
    last_used_scrollmodex = 255;
    last_used_scrollmodey = 255;
    last_used_resamplemode = 255;
    flush_vbos();
    glBindTexture(GL_TEXTURE_2D, 1);
  }
  NSEngine::draw_set_blend(last_used_blendmode);
  NSEngine::batch_draw_line_color(batch,
                      cursor[0].transformed_pos.x, cursor[0].transformed_pos.y,
                      cursor[1].transformed_pos.x, cursor[1].transformed_pos.y,
                      1, cursor[0].diffuse_color, cursor[1].diffuse_color);
  NSEngine::draw_set_blend(0);
  return 0;
  flush_vbos();
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
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(
  //     D3DPT_LINESTRIP, 1, this->vertex_buffers.primitive_write_cursor, 0x14);
  vertex_buffers.primitive_write_cursor += 2;
  draw_call_ctr_0xcc++;
  return 0;
}

int AnmManager::draw_vm__ins_612(float x, float y, float width, float height,
                                 float rotz, NSEngine::Color col1,
                                 NSEngine::Color col2,
                                 int anchorX, int anchorY) {
  if (reinterpret_cast<uint64_t>(vertex_buffers.primitive_write_cursor + 5) >=
      reinterpret_cast<uint64_t>(&vertex_buffers.primitive_write_cursor))
    return 0;
  float x1 = anchorX == 0 ? -width / 2.0 : anchorX == 1 ? 0.0 : -width;
  float x2 = anchorX == 0 ? width / 2.0 : anchorX == 1 ? width : 0.0;
  float y1 = anchorY == 0 ? -height / 2.0 : anchorY == 1 ? 0.0 : -height;
  float y2 = anchorY == 0 ? height / 2.0 : anchorY == 1 ? height : 0.0;
  auto cursor = vertex_buffers.primitive_write_cursor;
  cursor[0].transformed_pos.x = x1 * cos(rotz) - y1 * sin(rotz) + x;
  cursor[1].transformed_pos.x = x2 * cos(rotz) - y1 * sin(rotz) + x;
  cursor[2].transformed_pos.x = x2 * cos(rotz) - y2 * sin(rotz) + x;
  cursor[3].transformed_pos.x = x1 * cos(rotz) - y2 * sin(rotz) + x;
  cursor[0].transformed_pos.y = x1 * sin(rotz) + y1 * cos(rotz) + y;
  cursor[1].transformed_pos.y = x2 * sin(rotz) + y1 * cos(rotz) + y;
  cursor[2].transformed_pos.y = x2 * sin(rotz) + y2 * cos(rotz) + y;
  cursor[3].transformed_pos.y = x1 * sin(rotz) + y2 * cos(rotz) + y;
  cursor[0].diffuse_color = col1;
  cursor[1].diffuse_color = col2;
  cursor[2].diffuse_color = col2;
  cursor[3].diffuse_color = col1;
  cursor[0].transformed_pos.z = 0.0;
  cursor[1].transformed_pos.z = 0.0;
  cursor[2].transformed_pos.z = 0.0;
  cursor[3].transformed_pos.z = 0.0;
  cursor[0].transformed_pos.w = 1.0;
  cursor[1].transformed_pos.w = 1.0;
  cursor[2].transformed_pos.w = 1.0;
  cursor[3].transformed_pos.w = 1.0;
  cursor[4].transformed_pos.x = cursor[0].transformed_pos.x;
  cursor[4].transformed_pos.y = cursor[0].transformed_pos.y;
  cursor[4].transformed_pos.z = 0.0;
  cursor[4].transformed_pos.w = 1.0;
  cursor[4].diffuse_color = col1;
  SUPERVISOR.disable_zwrite();
  if (last_used_texture != 1) {
    last_used_texture = 1;
    last_used_scrollmodex = 255;
    last_used_scrollmodey = 255;
    last_used_resamplemode = 255;
    flush_vbos();
    glBindTexture(GL_TEXTURE_2D, 1);
  }
  NSEngine::draw_set_blend(last_used_blendmode);
  NSEngine::batch_draw_quad_color_2d(batch,
    cursor[0].transformed_pos, cursor[1].transformed_pos,
    cursor[2].transformed_pos, cursor[3].transformed_pos,
    cursor[0].diffuse_color, cursor[1].diffuse_color,
    cursor[2].diffuse_color, cursor[3].diffuse_color, true);
  NSEngine::draw_set_blend(0);
  return 0;
  // flush_vbos();
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
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, cursor, 0x14);
  vertex_buffers.primitive_write_cursor += 5;
  draw_call_ctr_0xcc++;
  return 0;
}

int AnmManager::draw_vm__mode_17__drawCircle(float x, float y, float angle_0,
                                             float radius, int vertex_count,
                                             NSEngine::Color color_1,
                                             NSEngine::Color color_2) {
  auto *cursor = vertex_buffers.primitive_write_cursor;
  if (reinterpret_cast<uint64_t>(cursor + vertex_count + 2) >=
      reinterpret_cast<uint64_t>(&vertex_buffers.primitive_write_cursor))
    return 0;
  cursor->diffuse_color = color_1;
  cursor->transformed_pos.x = x;
  cursor->transformed_pos.y = y;
  cursor->transformed_pos.z = 0.0;
  cursor->transformed_pos.w = 1.0;
  float angle_inc = PI2 / vertex_count;
  int n = vertex_count;
  while (n-- >= 0) {
    cursor++;
    cursor->transformed_pos = {math::lengthdir_vec(radius, angle_0),
                               0.0, 1.0};
    cursor->transformed_pos.x += x;
    cursor->transformed_pos.y += y;
    cursor->diffuse_color = color_2;
    angle_0 += angle_inc;
    math::angle_normalize(angle_0);
  }
  SUPERVISOR.disable_zwrite();
  SUPERVISOR.disable_ztest();
  // My version
  if (last_used_texture != 1) {
    last_used_texture = 1;
    last_used_scrollmodex = 255;
    last_used_scrollmodey = 255;
    last_used_resamplemode = 255;
    flush_vbos();
    glBindTexture(GL_TEXTURE_2D, 1);
  }
  for (int i = 0; i < vertex_count / 2; i++) {
    auto tl = vertex_buffers.primitive_write_cursor[0];
    auto tr = vertex_buffers.primitive_write_cursor[i*2+1];
    auto br = vertex_buffers.primitive_write_cursor[i*2+2];
    auto bl = vertex_buffers.primitive_write_cursor[i*2+3];
    batch->draw(last_used_texture,
      {tl.transformed_pos, tl.diffuse_color, {0, 0}},
      {tr.transformed_pos, tr.diffuse_color, {1, 0}},
      {br.transformed_pos, br.diffuse_color, {1, 1}},
      {bl.transformed_pos, bl.diffuse_color, {0, 1}},
      last_used_blendmode);
  }
  if (vertex_count % 2) {
    auto tl = vertex_buffers.primitive_write_cursor[0];
    auto tr = vertex_buffers.primitive_write_cursor[vertex_count];
    auto br = vertex_buffers.primitive_write_cursor[1];
    auto bl = vertex_buffers.primitive_write_cursor[1];
    batch->draw(last_used_texture,
      {tl.transformed_pos, tl.diffuse_color, {0, 0}},
      {tr.transformed_pos, tr.diffuse_color, {1, 0}},
      {br.transformed_pos, br.diffuse_color, {1, 1}},
      {bl.transformed_pos, bl.diffuse_color, {0, 1}},
      last_used_blendmode);
  }
  return 0;
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
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(
  //     D3DPT_TRIANGLEFAN, vertex_count,
  //     vertex_buffers.primitive_write_cursor, 0x14);
  vertex_buffers.primitive_write_cursor += vertex_count + 2;
  draw_call_ctr_0xcc++;
  return 0;
}

int AnmManager::draw_vm__mode_18__drawCircleBorder(float x, float y,
                                                   float angle_0, float radius,
                                                   int nb_vertex,
                                                   NSEngine::Color col) {
  auto *cursor = vertex_buffers.primitive_write_cursor;
  if (reinterpret_cast<uint64_t>(cursor + nb_vertex + 1) >=
      reinterpret_cast<uint64_t>(&vertex_buffers.primitive_write_cursor))
    return 0;
  float angle_inc = PI2 / nb_vertex;
  int n = nb_vertex;
  while (n-- > -1) {
    cursor->transformed_pos = {math::lengthdir_vec(radius, angle_0),
                               0.0, 1.0};
    cursor->transformed_pos.x += x;
    cursor->transformed_pos.y += y;
    cursor->diffuse_color = col;
    cursor++;
    angle_0 += angle_inc;
    math::angle_normalize(angle_0);
  }

  // My version
  if (last_used_texture != 1) {
    last_used_texture = 1;
    last_used_scrollmodex = 255;
    last_used_scrollmodey = 255;
    last_used_resamplemode = 255;
    flush_vbos();
    glBindTexture(GL_TEXTURE_2D, 1);
  }
  for (int i = 0; i < nb_vertex; i++) {
    auto p1 = vertex_buffers.primitive_write_cursor[i];
    auto p2 = vertex_buffers.primitive_write_cursor[i+1];
    NSEngine::batch_draw_line_color(batch, p1.transformed_pos.x,
                                    p1.transformed_pos.y,
                              p2.transformed_pos.x, p2.transformed_pos.y,
                              1, p1.diffuse_color, p2.diffuse_color);
  }
  return 0;
  flush_vbos();
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
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(
  //     D3DPT_LINESTRIP, nb_vertex, vertex_buffers.primitive_write_cursor,
  //     0x14);
  vertex_buffers.primitive_write_cursor += nb_vertex + 1;
  draw_call_ctr_0xcc++;
  return 0;
}

int AnmManager::draw_vm__mode_19__drawRing(float x, float y, float angle_0,
                                          float radius, float thickness,
                                          int vertex_count,
                                          NSEngine::Color col) {
  auto *cursor = vertex_buffers.primitive_write_cursor;
  if (reinterpret_cast<uint64_t>(cursor + vertex_count * 2 + 2) >=
      reinterpret_cast<uint64_t>(&vertex_buffers.primitive_write_cursor))
    return 0;
  float radius_1 = radius - thickness * 0.5;
  float radius_2 = thickness * 0.5 + radius;
  float ang_inc = PI2 / vertex_count;
  int n = vertex_count;
  while (n-- > -1) {
    cursor[0].transformed_pos = {math::lengthdir_vec(radius_1, angle_0),
                                 0.0, 1.0};
    cursor[0].transformed_pos.x += x;
    cursor[0].transformed_pos.y += y;
    cursor[0].diffuse_color = col;
    cursor[1].transformed_pos = {math::lengthdir_vec(radius_2, angle_0),
                                 0.0, 1.0};
    cursor[1].transformed_pos.x += x;
    cursor[1].transformed_pos.y += y;
    cursor[1].diffuse_color = col;
    cursor += 2;
    angle_0 += ang_inc;
    math::angle_normalize(angle_0);
  }

  // My version
  if (last_used_texture != 1) {
    last_used_texture = 1;
    last_used_scrollmodex = 255;
    last_used_scrollmodey = 255;
    last_used_resamplemode = 255;
    flush_vbos();
    glBindTexture(GL_TEXTURE_2D, 1);
  }
  for (int i = 0; i < vertex_count; i++) {
    auto tl = vertex_buffers.primitive_write_cursor[i*2+0];
    auto tr = vertex_buffers.primitive_write_cursor[i*2+1];
    auto bl = vertex_buffers.primitive_write_cursor[i*2+2];
    auto br = vertex_buffers.primitive_write_cursor[i*2+3];
    batch->draw(last_used_texture,
      {tl.transformed_pos, tl.diffuse_color, {0, 0}},
      {tr.transformed_pos, tr.diffuse_color, {1, 0}},
      {br.transformed_pos, br.diffuse_color, {1, 1}},
      {bl.transformed_pos, bl.diffuse_color, {0, 1}},
      last_used_blendmode);
  }

  return 0;
  flush_vbos();
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
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(
  //     D3DPT_TRIANGLESTRIP, vertex_count * 2,
  //     vertex_buffers.primitive_write_cursor, 0x14);
  vertex_buffers.primitive_write_cursor += vertex_count * 2 + 2;
  draw_call_ctr_0xcc++;
  return 0;
}

extern float SURF_ORIGIN_ECL_X;

void AnmManager::draw_vm_mode_24_25(AnmVM *vm, void *buff,
                                    int cnt) {
  if (!(vm->bitflags.visible) || !(vm->bitflags.f530_1)) return;
  if (vertex_buffers.unrendered_sprite_count != 0) {
    flush_vbos();
  }
  if (vm->bitflags.zwritedis) {
    SUPERVISOR.disable_zwrite();
  } else {
    SUPERVISOR.enable_zwrite();
  }

  vm->__matrix_1 = glm::mat4(1.f);
  vm->__matrix_2 = scale(vm->__matrix_1, {vm->scale * vm->scale_2, 1});
  vm->bitflags.scaled = false;
  if (vm->rotation.z != 0.0) vm->__matrix_2 *= glm::rotate(glm::mat4(1.f), vm->rotation.z, {0, 0, 1});
  if (vm->rotation.y != 0.0) vm->__matrix_2 *= glm::rotate(glm::mat4(1.f), vm->rotation.y, {0, 1, 0});
  if (vm->rotation.x != 0.0) vm->__matrix_2 *= glm::rotate(glm::mat4(1.f), vm->rotation.x, {1, 0, 0});
  vm->bitflags.rotated = false;
  glm::mat4 DStack_e0 = vm->__matrix_2;
  DStack_e0[3][0] =
      vm->entity_pos.x + vm->pos.x + vm->__pos_2.x -
      vm->anchor_offset.x * vm->scale.x *
          vm->scale_2.x;
  DStack_e0[3][1] = vm->entity_pos.y + vm->pos.y +
               vm->__pos_2.y -
              vm->anchor_offset.y * vm->scale.y *
                  vm->scale_2.y;
  DStack_e0[3][2] = vm->entity_pos.z + vm->pos.z +
               vm->__pos_2.z;
  if (vm->bitflags.originMode && !vm->parent_vm) {
    DStack_e0[3][0] += SURF_ORIGIN_ECL_X;
  }
  setup_render_state_for_vm(vm);

  if (last_used_texture != vm->getSprite().opengl_texid) {
    last_used_texture = vm->getSprite().opengl_texid;
    last_used_scrollmodex = 255;
    last_used_scrollmodey = 255;
    last_used_resamplemode = 255;
    flush_vbos();
    // SUPERVISOR.d3d_device->SetTexture(0, last_used_texture);
    glBindTexture(GL_TEXTURE_2D, last_used_texture);
  }

  auto cursor = reinterpret_cast<RenderVertex_t*>(buff);
  std::vector<RenderVertex_t> data(cursor, &cursor[cnt]);
  for (auto& v : data) {
    v.transformed_pos = DStack_e0 * v.transformed_pos;
  }

  int nb_segments = (cnt - 2) / 2;
  for (int i = 0; i < nb_segments; i++) {
    auto& tl = data[i*2+0];
    auto& tr = data[i*2+1];
    auto& bl = data[i*2+2];
    auto& br = data[i*2+3];
    batch->draw(vm->getSprite().texID,
      {tl.transformed_pos, tl.diffuse_color, tl.texture_uv},
      {tr.transformed_pos, tr.diffuse_color, tr.texture_uv},
      {br.transformed_pos, br.diffuse_color, br.texture_uv},
      {bl.transformed_pos, bl.diffuse_color, bl.texture_uv},
      last_used_blendmode);
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
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, 4);
  //   SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, 4);
  //   ANM_MANAGER_PTR->field33_0x18607cf = 1;
  // }
  // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, cnt - 2, buff,
  //                                        0x18);
  // return;
}

struct TableAnm508_t {
  int16_t anm_loaded_index = 0;
  int16_t script_index = 0;
  std::function<int(AnmVM *)> on_create = nullptr;
  int32_t index_of_on_tick = 0;
  int32_t index_of_on_draw = 0;
  int32_t index_of_on_destroy = 0;
  int32_t index_of_on_interrupt = 0;
  int32_t index_of_on_copy_1__disused = 0;
  int32_t index_of_on_copy_2__disused = 0;
} ANM_508_TABLE[]{
    {0, 0,
     [](AnmVM *vm) {
       vm->bitflags.originMode = 0;
       vm->layer = 0x28;
       vm->bitflags.resolutionMode = 1;
       vm->special_vertex_buffer_size = sizeof(EFFECT_1_buffer_t);
       vm->special_vertex_buffer_data = malloc(sizeof(EFFECT_1_buffer_t));
       memset(vm->special_vertex_buffer_data, 0, sizeof(EFFECT_1_buffer_t));
       auto buff =
           reinterpret_cast<EFFECT_1_buffer_t*>(vm->special_vertex_buffer_data);
       for (int i = 0; i < 4; i++) {
           AnmManager::getLoaded(8)->copyFromLoaded(&buff->vms[i], i + 3);
           buff->vms[i].parent_vm = nullptr;
           buff->vms[i].__root_vm__or_maybe_not = nullptr;
           buff->vms[i].update();
           buff->vms[i].entity_pos = {320, 240, 0};
       }
       AnmManager::getLoaded(8)->copyFromLoaded(&buff->vms[4], 0xc2);
       buff->vms[4].parent_vm = nullptr;
       buff->vms[4].__root_vm__or_maybe_not = nullptr;
       buff->vms[4].update();
       return 0;
     },
     1, 1, 1, 1, 0, 0},
    {0, 0,
     [](AnmVM *vm) {
       vm->special_vertex_buffer_size = sizeof(EFFECT_2_buffer_t);
       vm->special_vertex_buffer_data = malloc(sizeof(EFFECT_2_buffer_t));
       memset(vm->special_vertex_buffer_data, 0, sizeof(EFFECT_2_buffer_t));
       reinterpret_cast<EFFECT_2_buffer_t*>(vm->special_vertex_buffer_data)
        ->timer.reset();
       return 0;
     },
     2, 2, 2, 2, 1, 1},
    {0, 0,
     [](AnmVM *vm) {
       std::cout << "[WARNING] anm: effect 2 is not implemented\n";
       vm->bitflags.activeFlags = ANMVM_DELETE;
       return 0;
     },
     3, 3, 3, 3, 0, 0},
    {0, 0,
     [](AnmVM *vm) {
       std::cout << "[WARNING] anm: effect 3 is not implemented\n";
       vm->bitflags.activeFlags = ANMVM_DELETE;
       return 0;
     },
     3, 3, 3, 3, 0, 0},
    {0, 0,
     [](AnmVM *vm) {
       std::cout << "[WARNING] anm: effect 4 is not implemented\n";
       vm->bitflags.activeFlags = ANMVM_DELETE;
       return 0;
     },
     5, 7, 4, 4, 0, 0},
    {0, 0,
     [](AnmVM *vm) {
       std::cout << "[WARNING] anm: effect 5 is not implemented\n";
       vm->bitflags.activeFlags = ANMVM_DELETE;
       return 0;
     },
     5, 7, 4, 4, 0, 0},
    {0, 0,
     [](AnmVM *vm) {
       std::cout << "[WARNING] anm: effect 6 is not implemented\n";
       vm->bitflags.activeFlags = ANMVM_DELETE;
       return 0;
     },
     5, 7, 4, 4, 0, 0},
    {0, 0,
     [](AnmVM *vm) {
       std::cout << "[WARNING] anm: effect 7 is not implemented\n";
       vm->bitflags.activeFlags = ANMVM_DELETE;
       return 0;
     },
     5, 7, 4, 4, 0, 0},
    {0, 0,
     [](AnmVM *vm) {
       std::cout << "[WARNING] anm: effect 8 is not implemented\n";
       vm->bitflags.activeFlags = ANMVM_DELETE;
       return 0;
     },
     5, 7, 4, 4, 0, 0},
};

AnmID AnmManager::createVM508(int i, AnmVM *root) {

  if (-1 < ANM_508_TABLE[i].script_index) {
    auto vm = root;
    if (!vm) {
      loadedFiles[ANM_508_TABLE[i].anm_loaded_index].createEffect(ANM_508_TABLE[i].script_index, -1, &vm);
    }
    if (ANM_508_TABLE[i].on_create)
      ANM_508_TABLE[i].on_create(vm);
    vm->index_of_on_tick = ANM_508_TABLE[i].index_of_on_tick;
    vm->index_of_on_draw = ANM_508_TABLE[i].index_of_on_draw;
    vm->index_of_on_destroy = ANM_508_TABLE[i].index_of_on_destroy;
    vm->index_of_on_interrupt = ANM_508_TABLE[i].index_of_on_interrupt;
    vm->index_of_on_copy_1__disused =
        ANM_508_TABLE[i].index_of_on_copy_1__disused;
    vm->index_of_on_copy_2__disused =
        ANM_508_TABLE[i].index_of_on_copy_2__disused;
    return vm->id;
  }
  return 0;
}
