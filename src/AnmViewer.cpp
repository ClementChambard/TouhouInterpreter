#include "./AnmViewer.hpp"
#include "./AnmOpener/AnmManager.h"
#include "./Player.h"
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

#include <Engine.hpp>
#include <NSEngine.h>
#include <string>

AnmViewer *ANM_VIEWER_PTR = nullptr;

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

AnmViewer::AnmViewer() {
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

void TextureViewerWindow(bool *open) {
  if (!*open)
    return;
  ImGui::Begin("Texture viewer", open);
  ImGui::PushID("TextureVwr");
  static int selected = -1;
  static int textureId = 0;
  static std::string anmfilename = "-";
  if (AnmSlotSelector("Slot", &anmfilename, &selected)) {
    textureId = 0;
  }
  if (selected < 0 || selected > 30 ||
      anmfilename[anmfilename.size() - 1] == '-') {
    ImGui::PopID();
    ImGui::End();
    return;
  }
  AnmFile *f = AnmManager::getLoaded(selected);
  ImGui::InputInt("texture num", &textureId);
  if (textureId < 0)
    textureId = 0;
  if ((size_t)textureId >= f->textures.size())
    textureId = f->textures.size() - 1;
  ImGui::Text("%s, texture %d/%zd", anmfilename.c_str(), textureId + 1,
              f->textures.size());
  int tid = std::next(f->textures.begin(), textureId)->second;
  auto tex = NSEngine::TextureManager::GetTexture(tid);
  ImGui::Image((void *)(int64_t)tex->id,
               {(float)tex->width, (float)tex->height});
  static int spriteId = 0;
  auto r = getSpriteRange(f, tid);
  if (r.first == -1) {
    ImGui::PopID();
    ImGui::End();
    return;
  }
  ImGui::InputInt("sprite id", &spriteId);
  if (r.first > spriteId)
    spriteId = r.first;
  if (r.last < spriteId)
    spriteId = r.last;
  auto sp = f->sprites[spriteId];
  auto t = NSEngine::TextureManager::GetTextureID(sp.texID);
  ImGui::Image((void *)(int64_t)t, {sp.w, sp.h}, {sp.u1, sp.v1},
               {sp.u2, sp.v2});
  ImGui::PopID();
  ImGui::End();
}

void AnmView::renderInList() {
  if (ok)
    ok = AnmManager::isAlive(anmId);
  if (!ok) {
    windowOpen = false;
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0, 0, 1.0));
    ImGui::Text("%d", anmId);
    ImGui::PopStyleColor();
    return;
  }
  ImGui::PushID(("AnmId" + std::to_string(anmId)).c_str());
  ImGui::Text("%d", anmId);
  if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    windowOpen = !windowOpen;
  }
  ImGui::SameLine();
  if (ImGui::Button("X")) {
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
  static int slot = -1;
  static int script = 0;
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
  if (ImGui::Button("Create")) {
    ANM_VIEWER_PTR->anim(AnmManager::SpawnVM(slot, script));
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
    (*vm)(AnmManager::getLoaded(vm->anm_loaded_index)
              ->getPreloaded(vm->script_id));
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
    FLAG_COMBOBOX(rendermode, "Normal", "RotateZ", "", "", "", "", "", "", "3D",
                  "Textured circle", "", "", "", "Textured arc even",
                  "Textured arc", "", "Rectangle", "Polygon", "Polygon outline",
                  "Ring", "Rectangle grad", "Rectangle rot",
                  "Rectangle rot grad", "", "Cylinder", "3D Textured ring",
                  "Line horiz", "Rectangle outline")
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
}

void AnmViewer::on_draw() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void AnmViewer::check_anmviews() {
  for (auto it = m_anmviews.begin(); it != m_anmviews.end(); it++) {
    if (!it->ok) {
      it = --m_anmviews.erase(it);
    }
  }
}
