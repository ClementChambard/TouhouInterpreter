#include "AnmViewer.hpp"
#include "AnmOpener/AnmManager.h"
#include "Player.h"
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

#include <Engine.hpp>
#include <NSEngine.h>
#include <string>

AnmViewer *ANM_VIEWER_PTR = nullptr;

class ImGuiEventProcessor : public NSEngine::IEventProcessor {
public:
  void ProcessEvent(SDL_Event *e, bool &noKeyboard, bool &noMouse) override {
    ImGui_ImplSDL2_ProcessEvent(e);

    ImGuiIO &io = ImGui::GetIO();
    noKeyboard = io.WantCaptureKeyboard;
    noMouse = io.WantCaptureMouse;
  }
};

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
  NSEngine::engineData::eventProcessors.push_back(new ImGuiEventProcessor());
  NSEngine::InputManager::SetAsEventProcessor();
}

AnmViewer::~AnmViewer() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
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
  static int slot = 0;
  static int script = 0;
  ImGui::InputInt("Slot", &slot);
  ImGui::InputInt("Script", &script);
  if (ImGui::Button("Create")) {
    ANM_VIEWER_PTR->anim(AnmManager::SpawnVM(slot, script));
  }
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
  ImGui::Text("Instr: %d, return: %d@%d", vm->instr_offset,
              vm->interrupt_return_offset, vm->interrupt_return_time);
  if (ImGui::Button("Interrupt")) {
    ImGui::OpenPopup("InterruptVM");
  }
  ImGui::SameLine();
  if (ImGui::Button("Reset")) {
    (*vm)(AnmManager::getLoaded(vm->anm_loaded_index)
              ->getPreloaded(vm->script_id));
  }
  ImGui::Checkbox("Follow player", &v->followPlayer);
  int spid = vm->sprite_id;
  ImGui::InputInt("sprite_id", &spid);
  if (spid != vm->sprite_id)
    AnmManager::getLoaded(vm->anm_loaded_index)->setSprite(vm, spid);
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
