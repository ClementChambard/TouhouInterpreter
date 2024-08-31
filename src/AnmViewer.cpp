#include "./AnmViewer.hpp"
#include "./Anm/AnmManager.h"
#include "./Player.h"
#include "./Supervisor.h"
#include <imgui.h>
#include <imgui_ns.hpp>

#include <NSEngine.hpp>
#include <input.hpp>

#include "./Std/Stage.hpp"
#include "ScreenEffect.hpp"

AnmViewer *ANM_VIEWER_PTR = nullptr;
static UpdateFunc *ANM_VIEWER_ON_TICK = nullptr;
static UpdateFunc *ANM_VIEWER_ON_DRAW = nullptr;

static anm::Stats last_frame_stats;

AnmViewer::AnmViewer() {
  ANM_VIEWER_PTR = this;
  ANM_VIEWER_ON_TICK = new UpdateFunc([this]() {
    this->on_tick();
    return 1;
  });
  ANM_VIEWER_ON_DRAW = new UpdateFunc([this]() {
    this->on_draw();
    return 1;
  });
  UPDATE_FUNC_REGISTRY.register_on_tick(ANM_VIEWER_ON_TICK, 3);
  UPDATE_FUNC_REGISTRY.register_on_draw(ANM_VIEWER_ON_DRAW, 100);
}

AnmViewer::~AnmViewer() {
  UPDATE_FUNC_REGISTRY.unregister(ANM_VIEWER_ON_TICK);
  UPDATE_FUNC_REGISTRY.unregister(ANM_VIEWER_ON_DRAW);
  ANM_VIEWER_PTR = nullptr;
}

bool AnmSlotSelector(const char *label, std::string *strval, int *selected) {
  ImGui::PushID("AnmSlotSelector");
  bool changed = false;
  if (ImGui::BeginCombo(label, strval->c_str())) {
    for (int i = 0; i < 31; i++) {
      std::string t = anm::getLoaded(i)->getName();
      if (t == "")
        t = "-";
      t = std::to_string(i) + ": " + t;
      if (ImGui::Selectable(t.c_str())) {
        if (*selected != i)
          changed = true;
        *selected = i;
        *strval = t;
      }
    }
    ImGui::EndCombo();
  }
  ImGui::PopID();
  return changed;
}

auto getSpriteRange(anm::File *f, ns::Texture *tex) {
  struct rge {
    int first = -1;
    int last = -1;
  } r = {};
  int m = 0;
  int i = 0;
  for (auto s : f->sprites) {
    if (m == 0 && s.texture == tex) {
      r.first = i;
      m = 1;
    }
    if (m == 1 && s.texture != tex) {
      r.last = i - 1;
      return r;
    }
    i++;
  }

  if (r.first != -1)
    r.last = f->sprites.size() - 1;
  return r;
}

void ImageWithUVRepresentedOnIt(ImTextureID tex, ImVec2 imgSize,
                                ImVec2 imgUvStart, ImVec2 imgUvEnd,
                                ImVec2 sprUvStart, ImVec2 sprUvEnd,
                                ImU32 outline) {
  ImVec2 sprFractStart = (sprUvStart - imgUvStart) / (imgUvEnd - imgUvStart);
  ImVec2 sprFractEnd = (sprUvEnd - imgUvStart) / (imgUvEnd - imgUvStart);

  bool cullAll = sprFractEnd.x < 0 || sprFractEnd.y < 0 ||
                 sprFractStart.x > 1 || sprFractStart.y > 1;
  bool cullLeft = sprFractStart.x < 0;
  if (cullLeft)
    sprFractStart.x = 0;
  bool cullTop = sprFractStart.y < 0;
  if (cullTop)
    sprFractStart.y = 0;
  bool cullRight = sprFractEnd.x > 1;
  if (cullRight)
    sprFractEnd.x = 1;
  bool cullBottom = sprFractEnd.y > 1;
  if (cullBottom)
    sprFractEnd.y = 1;

  ImU32 outlineColorU32 = outline;

  auto Window = ImGui::GetCurrentWindow();
  auto p1 = Window->DC.CursorPos;
  auto p2 = p1 + imgSize;

  auto imgP1 = p1;
  auto imgP2 = p2;

  auto sprP1 = (sprFractStart * imgSize) + p1;
  auto sprP2 = (sprFractEnd * imgSize) + p1;

  ImGuiContext &g = *GImGui;
  const ImGuiStyle &style = g.Style;
  ImGui::ItemSize(imgSize, style.FramePadding.y);
  ImGui::ItemAdd(ImRect(p1, p2), ImGui::GetID("image"));
  auto const dl = ImGui::GetWindowDrawList();
  dl->AddImage(tex, imgP1, imgP2, imgUvStart, imgUvEnd);

  if (!cullAll) {
    if (!cullLeft)
      dl->AddLine(sprP1, {sprP1.x, sprP2.y}, outlineColorU32, 2);
    if (!cullTop)
      dl->AddLine(sprP1, {sprP2.x, sprP1.y}, outlineColorU32, 2);
    if (!cullRight)
      dl->AddLine(sprP2, {sprP2.x, sprP1.y}, outlineColorU32, 2);
    if (!cullBottom)
      dl->AddLine(sprP2, {sprP1.x, sprP2.y}, outlineColorU32, 2);
  }

  dl->AddQuad(p1, {p1.x, p2.y}, p2, {p2.x, p1.y}, 0xFFFFFFFF);
}

void ImageViewerSprite(ImTextureID tex, ImVec2 imgSize, ImVec2 *pos,
                       float *zoom, bool UseCtrls, ImVec2 sprUvStart,
                       ImVec2 sprUvEnd, ImU32 outline) {
  if (UseCtrls) {
    ImGui::SliderFloat("Zoom", zoom, 1, 10);
    ImGui::SliderFloat("X", &pos->x, 0, 1);
    ImGui::SliderFloat("Y", &pos->y, 0, 1);
  }
  ImVec2 posT = (*pos) * (1 - 1 / (*zoom));
  ImVec2 imgUvStart = {posT.x, posT.y};
  ImVec2 imgUvEnd = {posT.x + 1 / (*zoom), posT.y + 1 / (*zoom)};

  ImageWithUVRepresentedOnIt(tex, imgSize, imgUvStart, imgUvEnd, sprUvStart,
                             sprUvEnd, outline);
}

void TextureViewerWindow(bool *open) {
  if (!*open)
    return;
  ImGui::Begin("Texture viewer", open);
  ImGui::PushID("TextureVwr");
  static int selected = -1;
  static std::string anmfilename = "-";
  AnmSlotSelector("Slot", &anmfilename, &selected);
  if (selected < 0 || selected > 30 ||
      anmfilename[anmfilename.size() - 1] == '-') {
    ImGui::PopID();
    ImGui::End();
    return;
  }
  anm::File *f = anm::getLoaded(selected);
  static int spriteId = 0;
  ImGui::InputInt("sprite id", &spriteId);
  if (spriteId < 0)
    spriteId = 0;
  if (static_cast<size_t>(spriteId) >= f->sprites.size())
    spriteId = f->sprites.size() - 1;
  auto sp = f->sprites[spriteId];
  std::string name = "";
  for (auto pair : f->textures) {
    if (pair.second == sp.texture) {
      name = pair.first;
      break;
    }
  }
  ImGui::Text("%s", name.c_str());
  int w = sp.texture->get_width();
  int h = sp.texture->get_height();
  static ImVec2 pos = {0.5, 0.5};
  static float zoom = 1;
  ImageViewerSprite(reinterpret_cast<ImTextureID>(sp.texture->get_opengl_id()),
                    ImVec2(w, h), &pos, &zoom, true, {sp.u1, sp.v1},
                    {sp.u2, sp.v2}, 0xFF0000FF);
  ImGui::PopID();
  ImGui::End();
}

void AnmView::renderInList() {
  if (ok)
    ok = anm::is_alive(anmId) || vm;
  if (!ok) {
    windowOpen = false;
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0, 0, 1.0));
    ImGui::Text("%d", anmId);
    ImGui::PopStyleColor();
    return;
  }
  ImGui::PushID(("AnmId" + std::to_string(anmId)).c_str());
  auto vm = this->vm ? this->vm : anm::get_vm(anmId);
  auto name = anm::getLoaded(vm->anm_loaded_index)->name;
  if (parentId != 0)
    ImGui::Text("%s: script %d (%d->%d)", name.c_str(), vm->script_id, anmId,
                parentId);
  else
    ImGui::Text("%s: script %d (%d)", name.c_str(), vm->script_id, anmId);
  if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    windowOpen = !windowOpen;
  }
  if (!this->vm && (ImGui::SameLine(), ImGui::Button("X"))) {
    anm::delete_vm(anmId);
  }
  ImGui::PopID();
}

void anms_list_window(bool *open) {
  if (!*open)
    return;
  ImGui::Begin("Vm list", open);
  for (auto &view : ANM_VIEWER_PTR->views()) {
    view.renderInList();
  }
  ImGui::End();
}

void anms_window(bool *open) {
  if (!*open)
    return;
  ImGui::Begin("Anm VMs", open);
  ImGui::PushID("AnmSpawn");
  static int id = 0;
  ImGui::InputInt("##capture", &id);
  ImGui::SameLine();
  if (ImGui::Button("Capture")) {
    ANM_VIEWER_PTR->anim(id);
  }
  static int slot = -1;
  static int script = 0;
  static bool ui = false;
  static bool front = false;
  static float init_rot = 0;
  static ns::vec3 init_pos = {};
  static std::string slotname = "-";
  if (AnmSlotSelector("Slot", &slotname, &slot))
    script = 0;
  if (slot == -1) {
    ImGui::PopID();
    ImGui::End();
    return;
  }
  auto loaded = anm::getLoaded(slot);
  ImGui::Text("%s: %llu scripts", slotname.c_str(), loaded->nbScripts());
  ImGui::InputInt("Script", &script);
  if (script < 0)
    script = 0;
  if (static_cast<size_t>(script) >= loaded->nbScripts())
    script = loaded->nbScripts() - 1;
  ImGui::InputFloat3("pos", &init_pos[0]);
  ImGui::InputFloat("rot", &init_rot);
  ImGui::Checkbox("ui", &ui);
  ImGui::SameLine();
  ImGui::Checkbox("front", &front);
  if (ImGui::Button("Create")) {
    int mode = ui << 2 | front << 1;
    int id = anm::getLoaded(slot)->spawnVMExt(script, init_rot, init_pos, mode);
    ANM_VIEWER_PTR->anim(id);
  }
  ImGui::PopID();
  ImGui::End();
}

bool ImGui_BeginPopupCenter(const char *name) {
  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                          ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(300, 90));
  return ImGui::BeginPopup(name, ImGuiWindowFlags_NoDecoration);
}

void anm_view_window(AnmView *v) {
  if (!v->windowOpen || !v->ok)
    return;
  ImGui::Begin(("Anm(" + std::to_string(v->anmId) + ")").c_str(),
               &v->windowOpen);
  ImGui::PushID(("viewWin" + std::to_string(v->anmId)).c_str());

  ImGui::Text("Id: %d", v->anmId);
  auto vm = anm::get_vm(v->anmId);
  if (v->anmId == 0)
    vm = v->vm;
  if (!vm) {
    ImGui::PopID();
    ImGui::End();
    return;
  }
  ImGui::SameLine();
  ImGui::Text("Slot: %d, Script: %d", vm->anm_loaded_index, vm->script_id);
  ImGui::Text("Instr: %d@%d, return: %d@%d", vm->instr_offset,
              vm->time_in_script.current, vm->interrupt_return_offset,
              vm->interrupt_return_time.current);
  if (ImGui::Button("Interrupt")) {
    ImGui::OpenPopup("InterruptVM");
  }
  ImGui::SameLine();
  if (ImGui::Button("Reset")) {
    int id = vm->script_id;
    vm->reset();
    anm::getLoaded(vm->anm_loaded_index)->copyFromLoaded(vm, id);
    v->pauseInstr = -1;
  }
  ImGui::SameLine();
  if (v->pauseInstr == -1) {
    if (ImGui::Button("Pause")) {
      v->pauseInstr = vm->instr_offset;
      vm->instr_offset = -1;
    }
  } else {
    if (ImGui::Button("Continue")) {
      vm->instr_offset = v->pauseInstr;
      v->pauseInstr = -1;
    }
  }

  ImGui::Checkbox("Follow player", &v->followPlayer);
  int spid = vm->sprite_id;
  if (ImGui::Button("show sprite")) {
    v->spriteShowOpen = !v->spriteShowOpen;
  }
  ImGui::SameLine();
  ImGui::InputInt("sprite_id", &spid);
  if (spid != vm->sprite_id)
    anm::getLoaded(vm->anm_loaded_index)->setSprite(vm, spid);
  int layid = vm->layer;
  ImGui::InputInt("layer_id", &layid);
  if (static_cast<unsigned int>(layid) != vm->layer)
    vm->setLayer(layid);
  ImGui::Separator();
  ImGui::InputFloat3("pos", &vm->pos[0]);
  ImGui::InputFloat3("pos2", &vm->__pos_2[0]);
  ImGui::InputFloat3("entity pos", &vm->entity_pos[0]);
  ImGui::InputFloat3("rotation", &vm->rotation[0]);
  ImGui::InputFloat2("scale", &vm->scale[0]);
  ImGui::InputFloat2("scale2", &vm->scale_2[0]);
  ns::vec4 color =
      ns::vec4(vm->color_1.r, vm->color_1.g, vm->color_1.b, vm->color_1.a) /
      255.f;
  ImGui::ColorEdit4("color1", &color[0]);
  vm->color_1.r = color.r * 255.f;
  vm->color_1.g = color.g * 255.f;
  vm->color_1.b = color.b * 255.f;
  vm->color_1.a = color.a * 255.f;
  color = ns::vec4(vm->color_2.r, vm->color_2.g, vm->color_2.b, vm->color_2.a) /
          255.f;
  ImGui::ColorEdit4("color2", &color[0]);
  vm->color_2.r = color.r * 255.f;
  vm->color_2.g = color.g * 255.f;
  vm->color_2.b = color.b * 255.f;
  vm->color_2.a = color.a * 255.f;
  ImGui::InputFloat3("anchor offset", &vm->anchor_offset[0]);
  ImGui::InputFloat2("sprite size", &vm->sprite_size[0]);
  ImGui::InputInt4("Integer vars", vm->int_script_vars);
  ImGui::InputFloat4("Float vars", vm->float_script_vars);
  ImGui::InputInt2("Additionnal int vars", &vm->__script_var_8);
  ImGui::InputFloat3("Additionnal float vars", &vm->__script_vars_33_34_35[0]);

  if (ImGui::CollapsingHeader("flags")) {
    bool val;
    const char *selected;
#define FLAG_CHECKBOX(flagname)                                                \
  val = vm->bitflags.flagname;                                                 \
  ImGui::Checkbox(#flagname, &val);                                            \
  vm->bitflags.flagname = val;

#define FLAG_COMBOBOX(flagname, ...)                                           \
  static const char *flagname##_options[] = {__VA_ARGS__};                     \
  selected = flagname##_options[vm->bitflags.flagname];                        \
  if (ImGui::BeginCombo(#flagname, selected)) {                                \
    for (int i = 0; i < IM_ARRAYSIZE(flagname##_options); i++) {               \
      bool isSelected = (selected == flagname##_options[i]);                   \
      if (strlen(flagname##_options[i]) == 0)                                  \
        continue;                                                              \
      if (ImGui::Selectable(flagname##_options[i], isSelected)) {              \
        selected = flagname##_options[i];                                      \
        vm->bitflags.flagname = i;                                             \
      }                                                                        \
      if (isSelected)                                                          \
        ImGui::SetItemDefaultFocus();                                          \
    }                                                                          \
    ImGui::EndCombo();                                                         \
  }

    FLAG_CHECKBOX(visible)
    FLAG_CHECKBOX(f530_1)
    FLAG_CHECKBOX(rotated)
    FLAG_CHECKBOX(scaled)
    FLAG_CHECKBOX(zoomed)
    FLAG_COMBOBOX(blendmode, "Normal", "Add", "Subtract", "Replace", "Screen",
                  "Multiply", "uh", "Behind", "Darken", "Lighten")
    FLAG_CHECKBOX(f530_9)
    FLAG_CHECKBOX(alt_pos)
    FLAG_CHECKBOX(flip_x)
    FLAG_CHECKBOX(flip_y)
    FLAG_CHECKBOX(zwritedis)
    FLAG_CHECKBOX(f530_14)
    FLAG_CHECKBOX(f530_15)
    FLAG_CHECKBOX(f530_16)
    FLAG_COMBOBOX(colmode, "Use set1", "Use set2", "H gradient", "V gradient")
    FLAG_CHECKBOX(f530_19)
    FLAG_CHECKBOX(f530_20)
    FLAG_COMBOBOX(anchorX, "Center", "Left", "Right");
    FLAG_COMBOBOX(anchorY, "Center", "Top", "Bottom");
    FLAG_COMBOBOX(rendermode, "Normal", "RotateZ", "2", "3", "billboard", "5",
                  "billboard fog", "5 fog", "3D", "Textured circle", "", "", "",
                  "Textured arc even", "Textured arc", "", "Rectangle",
                  "Polygon", "Polygon outline", "Ring", "Rectangle grad",
                  "Rectangle rot", "Rectangle rot grad", "", "Cylinder",
                  "3D Textured ring", "Line horiz", "Rectangle outline")
    FLAG_COMBOBOX(scrollY, "Wrap", "Clamp", "Mirror")
    ImGui::Separator();
    FLAG_COMBOBOX(scrollX, "Wrap", "Clamp", "Mirror")
    FLAG_COMBOBOX(rotationMode, "000", "001", "010", "011", "100", "101", "110",
                  "111")
    FLAG_COMBOBOX(activeFlags, "00", "01", "10", "11")
    FLAG_CHECKBOX(autoRotate)
    FLAG_CHECKBOX(f534_8)
    FLAG_CHECKBOX(noSlowdown)
    FLAG_CHECKBOX(randomMode)
    FLAG_COMBOBOX(resampleMode, "Linear", "Nearest")
    FLAG_CHECKBOX(f534_12)
    FLAG_CHECKBOX(ins419)
    FLAG_COMBOBOX(f534_14_15, "00", "01", "10", "11")
    FLAG_CHECKBOX(noParent)
    FLAG_CHECKBOX(f534_17)
    FLAG_COMBOBOX(originMode, "TopLeft", "EclOrigin", "ScaledEclOrigin");
    FLAG_COMBOBOX(resolutionMode, "AsIs", "UpscaleIfNeeded",
                  "DownscaleIfNeeded");
    FLAG_CHECKBOX(parRotate)
    FLAG_CHECKBOX(hasGrowth)
    FLAG_CHECKBOX(colorizeChildren)
    FLAG_CHECKBOX(f534_26)
    ImGui::Text("unknown flags 27-31: %d", vm->bitflags.f534_27_31);
  }

  if (vm->list_of_children.next && ImGui::CollapsingHeader("children")) {
    for (auto l = vm->list_of_children.next; l; l = l->next) {
      ImGui::PushID(std::to_string(l->value->id.val).c_str());
      ImGui::Text("%s: script %d (%d)",
                  anm::getLoaded(l->value->anm_loaded_index)->name.c_str(),
                  l->value->script_id, l->value->id.val);
      ImGui::SameLine();
      if (ImGui::Button("see")) {
        ANM_VIEWER_PTR->animP(l->value->id.val, vm->id.val);
      }
      ImGui::PopID();
    }
  }

  if (ImGui::CollapsingHeader("matrices")) {
    ImGui::Text("matrix 1");
    ImGui::PushID("matrix1");
    ImGui::InputFloat4("##row0", &vm->__matrix_1[0][0]);
    ImGui::InputFloat4("##row1", &vm->__matrix_1[1][0]);
    ImGui::InputFloat4("##row2", &vm->__matrix_1[2][0]);
    ImGui::InputFloat4("##row3", &vm->__matrix_1[3][0]);
    ImGui::PopID();
    ImGui::Text("matrix 2");
    ImGui::PushID("matrix2");
    ImGui::InputFloat4("##row0", &vm->__matrix_2[0][0]);
    ImGui::InputFloat4("##row1", &vm->__matrix_2[1][0]);
    ImGui::InputFloat4("##row2", &vm->__matrix_2[2][0]);
    ImGui::InputFloat4("##row3", &vm->__matrix_2[3][0]);
    ImGui::PopID();
    ImGui::Text("matrix 3");
    ImGui::PushID("matrix3");
    ImGui::InputFloat4("##row0", &vm->__matrix_3[0][0]);
    ImGui::InputFloat4("##row1", &vm->__matrix_3[1][0]);
    ImGui::InputFloat4("##row2", &vm->__matrix_3[2][0]);
    ImGui::InputFloat4("##row3", &vm->__matrix_3[3][0]);
    ImGui::PopID();
  }

  if (ImGui_BeginPopupCenter("InterruptVM")) {
    static bool interrupt_recursive = false;
    static int interrupt_value = 0;
    ImGui::PushID("interruptVM");
    ImGui::InputInt("interrrupt label", &interrupt_value);
    ImGui::Checkbox("recursive", &interrupt_recursive);
    if (ImGui::Button("Send")) {
      if (interrupt_recursive) {
        vm->interruptRec(interrupt_value);
      } else {
        vm->interrupt(interrupt_value);
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
      interrupt_recursive = false;
      interrupt_value = 0;
      ImGui::CloseCurrentPopup();
    }
    ImGui::PopID();
    ImGui::EndPopup();
  }
  ImGui::PopID();
  ImGui::End();

  if (!v->spriteShowOpen)
    return;

  ImGui::Begin(("SpriteOf(" + std::to_string(v->anmId) + ")").c_str(),
               &v->spriteShowOpen);
  ImGui::PushID(("spriteWin" + std::to_string(v->anmId)).c_str());
  auto sp = vm->getSprite();
  ImTextureID tex = reinterpret_cast<ImTextureID>(sp.texture->get_opengl_id());
  ImGui::Image(tex, {sp.w, sp.h}, {sp.u1, sp.v1}, {sp.u2, sp.v2});
  ImGui::PopID();
  ImGui::End();
}

void set_camera_window(bool *open) {
  if (!*open)
    return;
  ImGui::Begin("Camera", open);
  ImGui::PushID("CameraWindow");
  static int cam = 0;
  static const char *camera_names[] = {"Camera 0", "Camera 1", "Camera 2",
                                       "Camera 3", "Stage"};
  auto selected = camera_names[cam];
  if (ImGui::BeginCombo("Camera", selected)) {
    for (int i = 0; i < IM_ARRAYSIZE(camera_names); i++) {
      bool isSelected = (selected == camera_names[i]);
      if (ImGui::Selectable(camera_names[i], isSelected)) {
        selected = camera_names[i];
        cam = i;
      }
      if (isSelected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  anm::Camera_t *camera;
  if (cam < 4)
    camera = &SUPERVISOR.cameras[cam];
  else
    camera = &STAGE_PTR->inner.camera;
  ImGui::InputFloat3("position", &camera->position[0]);
  ImGui::InputFloat3("look", &camera->facing[0]);
  ImGui::InputFloat3("up", &camera->up[0]);
  ImGui::InputFloat("fov", &camera->fov_y);
  ImGui::PopID();
  ImGui::End();
}

void openedFiles_window(bool *open) {
  if (!*open)
    return;
  static int toOpen = -1;
  ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Appearing);
  ImGui::Begin("Anm slots", open);
  ImGui::PushID("AnmSlotsWindow");
  if (ImGui::BeginListBox("##AnmSlots_listbox",
                          ImVec2(ImGui::GetContentRegionAvail().x, 400))) {
    for (int i = 0; i < 31; i++) {
      ImGui::PushID(("slot_" + std::to_string(i)).c_str());
      auto loaded = anm::getLoaded(i);
      if (!loaded || strcmp(loaded->getName(), "") == 0) {
        ImGui::Text("%02d: Empty", i);
        ImGui::SameLine();
        if (ImGui::Button("Open")) {
          toOpen = i;
          ImGui::OpenPopup("PopupOpenAnm");
          NS_INFO("Open %d", i);
        }
        if (ImGui_BeginPopupCenter("PopupOpenAnm")) {
          ImGui::Text("Open anm on slot %d.", toOpen);
          static char buf[256] = {};
          if (ImGui::InputText("File name", buf, 255,
                               ImGuiInputTextFlags_EnterReturnsTrue) ||
              ImGui::Button("Open")) {
            std::string s = buf;
            if (s[s.size() - 1] != 'm' || s[s.size() - 2] != 'n' ||
                s[s.size() - 3] != 'a' || s[s.size() - 4] != '.') {
              s += ".anm";
            }
            anm::loadFile(
                toOpen, strdup(s.c_str())); // XXX: NEVER FREE THIS STRING: LEAK
            buf[0] = 0;
            toOpen = -1;
            ImGui::CloseCurrentPopup();
          }
          ImGui::EndPopup();
        }
      } else {
        auto name = loaded->getName();
        ImGui::Text("%02d: %s    %lld sprites, %lld scripts", i, name,
                    loaded->nbSprites(), loaded->nbScripts());
        ImGui::SameLine();
        if (ImGui::Button("X")) {
          loaded->Cleanup();
        }
      }
      ImGui::PopID();
    }
    ImGui::EndListBox();
  }
  ImGui::PopID();
  ImGui::End();
}

void screff_window(bool *open) {
  if (!*open)
    return;
  ImGui::Begin("Screen effect", open);
  static int p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0, p6 = 0;
  ImGui::InputInt("param_1", &p1);
  ImGui::InputInt("param_2", &p2);
  ImGui::InputInt("param_3", &p3);
  ImGui::InputInt("param_4", &p4);
  ImGui::InputInt("param_5", &p5);
  ImGui::InputInt("param_6", &p6);
  if (ImGui::Button("Create")) {
    new ScreenEffect(p1, p2, p3, p4, p5, p6);
  }
  ImGui::End();
}

void main_menu_window(bool *open) {
  if (!*open)
    return;

  static bool openfileswindowopen = false;
  openedFiles_window(&openfileswindowopen);

  static bool anms_window_open = false;
  anms_window(&anms_window_open);

  static bool anms_list_window_open = false;
  anms_list_window(&anms_list_window_open);

  static bool texture_vwr_window_open = false;
  TextureViewerWindow(&texture_vwr_window_open);

  static bool camera_w_open = false;
  set_camera_window(&camera_w_open);

  static bool screff_w_open = false;
  screff_window(&screff_w_open);

  ImGui::Begin("Anm viewer main menu", open);
  ImGui::PushID("MainMenu");
  if (ImGui::Button("open files window")) {
    openfileswindowopen = !openfileswindowopen;
  }
  if (ImGui::Button("spawn anm window")) {
    anms_window_open = !anms_window_open;
  }
  if (ImGui::Button("anm list window")) {
    anms_list_window_open = !anms_list_window_open;
  }
  if (ImGui::Button("texture viewer")) {
    texture_vwr_window_open = !texture_vwr_window_open;
  }
  if (ImGui::Button("camera")) {
    camera_w_open = !camera_w_open;
  }
  if (ImGui::Button("screen effect")) {
    screff_w_open = !screff_w_open;
  }

  float gs = ns::get_instance()->game_speed();
  float ogs = gs;
  ImGui::SliderFloat("Game speed", &gs, 0.0, 2.0);
  if (ogs != gs)
    ns::get_instance()->set_game_speed(gs);

  ImGui::InputFloat("RESOLUTION_MULT", &anm::RESOLUTION_MULT);
  if (ImGui::Button("ADD_ARCADE")) {
    ANM_VIEWER_PTR->animPtr(SUPERVISOR.arcade_vm_0);
    ANM_VIEWER_PTR->animPtr(SUPERVISOR.arcade_vm_1);
    ANM_VIEWER_PTR->animPtr(SUPERVISOR.arcade_vm_2__handles_upscaling);
    ANM_VIEWER_PTR->animPtr(SUPERVISOR.arcade_vm_3__handles_seija);
  }
  ImGui::PopID();
  ImGui::End();
}

void anm_mgr_stats(bool *open) {
  if (!*open)
    return;

  ImGui::Begin("Anm mgr stats", open);
  ImGui::Text("ticked world: %lld", last_frame_stats.n_vm_ticked_world);
  ImGui::Text("ticked ui   : %lld", last_frame_stats.n_vm_ticked_ui);
  ImGui::Text("ticked total: %lld", last_frame_stats.n_vm_ticked);
  ImGui::Text("drawn world : %lld", last_frame_stats.n_vm_drawn_world);
  ImGui::Text("drawn ui    : %lld", last_frame_stats.n_vm_drawn_ui);
  ImGui::Text("drawn total : %lld", last_frame_stats.n_vm_drawn);
  ImGui::Text("draw started: %lld", last_frame_stats.n_total_initiated_draws);
  ImGui::Text("drawcalls   : %lld", last_frame_stats.n_flush_vbos);
  ImGui::Text("non fast vms: %lld", last_frame_stats.n_non_fast_vm_alive);
  ImGui::End();
}

void AnmViewer::on_tick() {
  static int gc_timer = 0;
  if (gc_timer++ == 60) {
    gc_timer = 0;
    check_anmviews();
  }

  ns::ImGuiContext::getInstance()->new_frame();

  static bool anmMenu = false;
  if (ns::keyboard::pressed(ns::Key::G)) {
    anmMenu = !anmMenu;
  }

  static bool anmmgrstats = false;
  if (ns::keyboard::pressed(ns::Key::T)) {
    anmmgrstats = !anmmgrstats;
  }

  main_menu_window(&anmMenu);
  anm_mgr_stats(&anmmgrstats);

  for (auto &v : m_anmviews) {
    anm_view_window(&v);
    if (v.ok && v.followPlayer && PLAYER_PTR) {
      auto vm = anm::get_vm(v.anmId);
      if (vm)
        vm->entity_pos = PLAYER_PTR->inner.pos;
    }
  }
}

void AnmViewer::on_draw() {

  ns::ImGuiContext::getInstance()->render();

  anm::get_stats(last_frame_stats);
}

void AnmViewer::check_anmviews() {
  for (auto it = m_anmviews.begin(); it != m_anmviews.end(); it++) {
    if (!it->ok) {
      it = --m_anmviews.erase(it);
    }
  }
}
