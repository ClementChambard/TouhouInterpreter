#include "./AnmViewer.hpp"
#include "./AnmOpener/AnmManager.h"
#include "./Player.h"
#include "./Supervisor.h"
#include "GlobalData.h"
#include <imgui.h>
#include <imguiW.hpp>

#include <Engine.hpp>
#include <NSEngine.h>
#include <string>

AnmViewer *ANM_VIEWER_PTR = nullptr;

float c = 640.f;
float top = 256.f;
float w = 200.f;
float bottom = 600.f;
int cam = 3;

class ImGuiEventProcessor : public NSEngine::IEventProcessor {
public:
  ~ImGuiEventProcessor() override {}
  void ProcessEvent(SDL_Event *e, bool &noKeyboard, bool &noMouse) override {
    ImGui_ImplSDL2_ProcessEvent(e);

    ImGuiIO &io = ImGui::GetIO();
    noKeyboard = io.WantCaptureKeyboard;
    noMouse = io.WantCaptureMouse;
  }
};

static ImGuiEventProcessor *igep = nullptr;

// AnmVM vm1;
// AnmVM vm2;
AnmViewer::AnmViewer() {
  RESOLUTION_MULT = 1.f;
  BACK_BUFFER_SIZE.x = 1280/2.f;
  BACK_BUFFER_SIZE.y = 960/2.f;
  SUPERVISOR.init_cameras();
  RESOLUTION_MULT = 1.f;
  // AnmManager::LoadFile(8, "effect.anm");
  // AnmManager::LoadFile(5, "front.anm");
  // AnmManager::getLoaded(8)->copyFromLoaded(&vm1, 105);
  // AnmManager::getLoaded(5)->copyFromLoaded(&vm2, 391);
  // animPtr(&vm1);
  // animPtr(&vm2);
  ANM_VIEWER_PTR = this;
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplOpenGL3_Init("#version 130");
  ImGui_ImplSDL2_InitForOpenGL(
      NSEngine::getInstance()->window().getSdlWindow(),
      NSEngine::getInstance()->window().getSdlGlContext());
  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = "";
  ImGui::StyleColorsDark();
  igep = new ImGuiEventProcessor();
  NSEngine::engineData::eventProcessors.push_back(igep);
  NSEngine::InputManager::SetAsEventProcessor();
}

AnmViewer::~AnmViewer() {
  delete igep;
  igep = nullptr;
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}

bool AnmSlotSelector(const char *label, std::string *strval, int *selected) {
  ImGui::PushID("AnmSlotSelector");
  bool changed = false;
  if (ImGui::BeginCombo(label, strval->c_str())) {
    for (int i = 0; i < 31; i++) {
      std::string t = AnmManager::getLoaded(i)->getName();
      if (t == "notLoaded")
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

auto getSpriteRange(AnmFile *f, uint32_t texid) {
  struct rge {
    int first = -1;
    int last = -1;
  } r = {};
  int m = 0;
  int i = 0;
  for (auto s : f->sprites) {
    if (m == 0 && s.texID == texid) {
      r.first = i;
      m = 1;
    }
    if (m == 1 && s.texID != texid) {
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
  AnmFile *f = AnmManager::getLoaded(selected);
  static int spriteId = 0;
  ImGui::InputInt("sprite id", &spriteId);
  if (spriteId < 0)
    spriteId = 0;
  if (static_cast<size_t>(spriteId) >= f->sprites.size())
    spriteId = f->sprites.size() - 1;
  auto sp = f->sprites[spriteId];
  auto t = NSEngine::TextureManager::GetTextureID(sp.texID);
  int w, h;
  NSEngine::TextureManager::GetTextureSize(sp.texID, w, h);
  static ImVec2 pos = {0.5, 0.5};
  static float zoom = 1;
  ImageViewerSprite(reinterpret_cast<ImTextureID>(t), ImVec2(w, h), &pos, &zoom,
                    true, {sp.u1, sp.v1}, {sp.u2, sp.v2}, 0xFF0000FF);
  ImGui::PopID();
  ImGui::End();
}

void AnmView::renderInList() {
  if (ok)
    ok = AnmManager::isAlive(anmId) || vm;
  if (!ok) {
    windowOpen = false;
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0, 0, 1.0));
    ImGui::Text("%d", anmId);
    ImGui::PopStyleColor();
    return;
  }
  ImGui::PushID(("AnmId" + std::to_string(anmId)).c_str());
  auto vm = this->vm ? this->vm : AnmManager::getVM(anmId);
  auto name = AnmManager::getLoaded(vm->anm_loaded_index)->name.c_str();
  if (parentId != 0)
    ImGui::Text("%s: script %d (%d->%d)", name, vm->script_id, anmId, parentId);
  else
    ImGui::Text("%s: script %d (%d)", name, vm->script_id, anmId);
  if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    windowOpen = !windowOpen;
  }
  ImGui::SameLine();
  if (!this->vm && ImGui::Button("X")) {
    AnmManager::deleteVM(anmId);
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
  static glm::vec3 init_pos = {};
  static std::string slotname = "-";
  if (AnmSlotSelector("Slot", &slotname, &slot))
    script = 0;
  if (slot == -1) {
    ImGui::PopID();
    ImGui::End();
    return;
  }
  auto loaded = AnmManager::getLoaded(slot);
  ImGui::Text("%s: %zd scripts", slotname.c_str(), loaded->nbScripts());
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
    int id = AnmManager::getLoaded(slot)->spawnVMExt(script, init_rot, init_pos,
                                                     mode);
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
  auto vm = AnmManager::getVM(v->anmId);
  if (v->anmId == 0) vm = v->vm;
  if (!vm) {
    ImGui::PopID();
    ImGui::End();
    return;
  }
  ImGui::SameLine();
  ImGui::Text("Slot: %d, Script: %d", vm->anm_loaded_index, vm->script_id);
  ImGui::Text("Instr: %d@%d, return: %d@%d", vm->instr_offset,
              vm->time_in_script, vm->interrupt_return_offset,
              vm->interrupt_return_time);
  if (ImGui::Button("Interrupt")) {
    ImGui::OpenPopup("InterruptVM");
  }
  ImGui::SameLine();
  if (ImGui::Button("Reset")) {
    int id = vm->script_id;
    vm->reset();
    AnmManager::getLoaded(vm->anm_loaded_index)->copyFromLoaded(vm, id);
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
    AnmManager::getLoaded(vm->anm_loaded_index)->setSprite(vm, spid);
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
  glm::vec4 color =
      glm::vec4(vm->color_1.r, vm->color_1.g, vm->color_1.b, vm->color_1.a) /
      255.f;
  ImGui::ColorEdit4("color1", &color[0]);
  vm->color_1.r = color.r * 255.f;
  vm->color_1.g = color.g * 255.f;
  vm->color_1.b = color.b * 255.f;
  vm->color_1.a = color.a * 255.f;
  color =
      glm::vec4(vm->color_2.r, vm->color_2.g, vm->color_2.b, vm->color_2.a) /
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
    FLAG_COMBOBOX(originMode, "TopLeft", "0,0", "scaled 0,0");
    FLAG_COMBOBOX(resolutionMode, "0", "1", "2", "3");
    FLAG_CHECKBOX(parRotate)
    FLAG_CHECKBOX(hasGrowth)
    FLAG_CHECKBOX(colorizeChildren)
    FLAG_CHECKBOX(f534_26)
    ImGui::Text("unknown flags 27-31: %d", vm->bitflags.f534_27_31);
  }

  if (vm->list_of_children.next && ImGui::CollapsingHeader("children")) {
    for (auto l = vm->list_of_children.next; l; l = l->next) {
      ImGui::PushID(std::to_string(l->value->id.val).c_str());
      ImGui::Text(
          "%s: script %d (%d)",
          AnmManager::getLoaded(l->value->anm_loaded_index)->name.c_str(),
          l->value->script_id, l->value->id.val);
      ImGui::SameLine();
      if (ImGui::Button("see")) {
        ANM_VIEWER_PTR->animP(l->value->id.val, vm->id.val);
      }
      ImGui::PopID();
    }
  }

  if (ImGui_BeginPopupCenter("InterruptVM")) {
    static bool interrupt_recursive = false;
    static int interrupt_value = 0;
    ImGui::PushID("interruptVM");
    ImGui::InputInt("interrrupt label", &interrupt_value);
    ImGui::Checkbox("recursive", &interrupt_recursive);
    if (ImGui::Button("Send")) {
      if (interrupt_recursive)
        vm->interruptRec(interrupt_value);
      else
        vm->interrupt(interrupt_value);
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
               &v->windowOpen);
  ImGui::PushID(("spriteWin" + std::to_string(v->anmId)).c_str());
  auto sp = vm->getSprite();
  ImTextureID tex = reinterpret_cast<ImTextureID>(
      NSEngine::TextureManager::GetTextureID(sp.texID));
  ImGui::Image(tex, {sp.w, sp.h}, {sp.u1, sp.v1}, {sp.u2, sp.v2});
  ImGui::PopID();
  ImGui::End();
}

void set_camera_window(bool *open) {
  if (!*open)
    return;
  ImGui::Begin("Camera", open);
  ImGui::PushID("CameraWindow");
  static bool captureActive = false;
  ImGui::Checkbox("Capture camera", &captureActive);
  static glm::vec3 camPos = {0, 0, -2000};
  static glm::vec3 up = {0, 0, -1};
  static float yaw = 0.f;
  static float pitch = 0.1f;
  static float fov = NSEngine::engineData::cam3d->getFov();
  ImGui::InputFloat3("position", &camPos[0]);
  ImGui::InputFloat3("up", &up[0]);
  ImGui::InputFloat("yaw", &yaw);
  ImGui::InputFloat("pitch", &pitch);
  ImGui::InputFloat("fov", &fov);
  pitch = glm::clamp(pitch, -PI / 2.f + 0.001f, PI / 2.f - 0.001f);
  fov = glm::clamp(fov, 0.f, PI / 2.f);
  static const float aspectRatio =
      (float)NSEngine::getInstance()->window().getWindowData().width /
      (float)NSEngine::getInstance()->window().getWindowData().height;
  if (captureActive) {
    glm::vec3 debugLookat =
        glm::vec3(cos(pitch) * sin(yaw), sin(pitch), cos(pitch) * cos(yaw));
    glm::vec3 debugRight = glm::normalize(glm::cross(debugLookat, up));
    glm::mat4 persp = glm::perspective(fov, aspectRatio, 0.1f, 10000.f);
    glm::vec3 up2 = glm::normalize(glm::cross(debugRight, debugLookat));
    glm::mat4 viewMatrix = glm::lookAt(camPos, camPos + debugLookat, up2);
    NSEngine::engineData::cam3d->setMat(persp, viewMatrix);
  }
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
      auto loaded = AnmManager::getLoaded(i);
      if (!loaded || loaded->getName() == "notLoaded") {
        ImGui::Text("%02d: Empty", i);
        ImGui::SameLine();
        if (ImGui::Button("Open")) {
          toOpen = i;
          ImGui::OpenPopup("PopupOpenAnm");
          std::cout << "Open" << i << "\n";
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
            AnmManager::LoadFile(toOpen, s);
            buf[0] = 0;
            toOpen = -1;
            ImGui::CloseCurrentPopup();
          }
          ImGui::EndPopup();
        }
      } else {
        auto name = loaded->getName();
        ImGui::Text("%02d: %s    %zd sprites, %zd scripts", i, name.c_str(),
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
extern float SURF_ORIGIN_ECL_X;
extern float SURF_ORIGIN_ECL_Y;
extern float SURF_ORIGIN_ECL_FULL_X;
extern float SURF_ORIGIN_ECL_FULL_Y;
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

  ImGui::InputFloat("RESOLUTION_MULT", &RESOLUTION_MULT);
  glm::vec2 ecl_origin = {SURF_ORIGIN_ECL_X, SURF_ORIGIN_ECL_Y};
  glm::vec2 ecl_origin_full = {SURF_ORIGIN_ECL_FULL_X, SURF_ORIGIN_ECL_FULL_Y};
  ImGui::InputFloat2("SURF_ORIGIN_ECL", &ecl_origin.x);
  ImGui::InputFloat2("SURF_ORIGIN_ECL_FULL", &ecl_origin_full.x);
  SURF_ORIGIN_ECL_X = ecl_origin.x;
  SURF_ORIGIN_ECL_Y = ecl_origin.y;
  SURF_ORIGIN_ECL_FULL_X = ecl_origin_full.x;
  SURF_ORIGIN_ECL_FULL_Y = ecl_origin_full.y;
  ImGui::PopID();
  ImGui::End();
}

void AnmViewer::on_tick() {
  static int gc_timer = 0;
  if (gc_timer++ == 60) {
    gc_timer = 0;
    check_anmviews();
  }

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  static bool anmMenu = false;
  if (Inputs::Keyboard().Pressed(NSK_g)) {
    anmMenu = !anmMenu;
  }

  main_menu_window(&anmMenu);

  for (auto &v : m_anmviews) {
    anm_view_window(&v);
    if (v.ok && v.followPlayer && PLAYER_PTR) {
      auto vm = AnmManager::getVM(v.anmId);
      if (vm)
        vm->entity_pos = PLAYER_PTR->inner.pos;
    }
  }

  ImGui::Begin("test");
  ImGui::InputFloat("top", &top);
  ImGui::InputFloat("bottom", &bottom);
  ImGui::InputFloat("width", &w);
  ImGui::InputFloat("center", &c);
  ImGui::InputInt("cam", &cam);
  ImGui::End();
}

extern float SURF_ORIGIN_ECL_X;
extern float SURF_ORIGIN_ECL_Y;
void AnmViewer::on_draw() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  // vm1.update();
  // vm2.update();
  // vm1.entity_pos = PLAYER_PTR->inner.pos;
  // vm2.entity_pos = PLAYER_PTR->inner.pos*2.f;
  // // SUPERVISOR.cameras[3].as_2d_matrix();
  // SUPERVISOR.cameras[2].as_2d_matrix();
  // SUPERVISOR.set_camera_by_index(cam, true);
  // glEnable(GL_BLEND);
  // AnmManager::drawVM(&vm2);
  // AnmManager::drawVM(&vm1);
  // SUPERVISOR.disable_ztest();
  // SUPERVISOR.disable_zwrite(true);
  // SUPERVISOR.disable_d3d_fog(true);
  // // AnmManager::batch->draw(1, {{c, top, 0}, {255, 0, 0, 255}, {0, 0}},
  // //                         {{c - w / 2, bottom, 0}, {0, 255, 0, 255}, {0, 0}},
  // //                         {{c + w / 2, bottom, 0}, {0, 0, 255, 255}, {0, 0}},
  // //                         {{c, top, 0}, {255, 0, 0, 255}, {0, 0}});
  // AnmManager::batch->end();
  // // AnmManager::shader->start();
  // AnmManager::batch->renderBatch();
  // // AnmManager::shader->stop();
  // AnmManager::batch->begin();
  // glDisable(GL_BLEND);
}

void AnmViewer::check_anmviews() {
  for (auto it = m_anmviews.begin(); it != m_anmviews.end(); it++) {
    if (!it->ok) {
      it = --m_anmviews.erase(it);
    }
  }
}
