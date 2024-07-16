#ifndef GUIMSGVM_INCLUDED_H
#define GUIMSGVM_INCLUDED_H

#include "Anm/AnmVM.h"
#include "MenuHelper.hpp"
#include <Timer.hpp>

struct MsgIns_t {
    int16_t time = 0;
    uint8_t opcode = 0;
    uint8_t argsize = 0;
    uint8_t data[];
};

struct GuiMsgVm_t {
    int32_t field_0x0 = 0;
    ns::Timer_t __timer_4 = 0;
    ns::Timer_t time_in_script = 0;
    ns::Timer_t pause_timer = 0;
    anm::ID anm_id_player_face = 0;
    anm::ID anm_id_enemy_face[4] = {};
    anm::ID __anm_id_idfk = 0;
    anm::ID anm_id_text_line_1 = 0;
    anm::ID anm_id_text_line_2 = 0;
    anm::ID anm_id_furigana_1 = 0;
    anm::ID anm_id_furigana_2 = 0;
    anm::ID anm_id_intro = 0;
    anm::ID anm_id_speechBubble = 0;
    anm::ID __anm_id_70 = 0;
    int32_t __unused_menu_time = 0;
    int32_t __unused_menu_state = 0;
    MenuHelper_t __unused_menu_control = {};
    int32_t field_0x154 = 0;
    MsgIns_t* current_instr = nullptr;
    ns::vec3 __vec3_0{16.0, 0.0, 0.0};
    ns::vec3 __vec3_1{16.0, 0.0, 0.0};
    ns::vec3 __vec3_2{16.0, 0.0, 0.0};
    ns::vec3 __vec3_3{16.0, 0.0, 0.0};
    int32_t __dword_incremented_by_enemyAppear = 0;
    int32_t flags = 0;
    int32_t next_text_line = 0;
    int32_t field_0x198 = 0;
    int32_t active_side = 0;
    ns::Color text_colors[4] {c_black, c_black, c_black, c_black};
    ns::vec3 callout_pos{384.0, 640.0, 0.0};
    float speechBubbleLength = 320.f;
    int32_t field_0x1c0 = 0;
    int32_t speechBubbleType = 0;

    explicit GuiMsgVm_t(MsgIns_t* instr);
    ~GuiMsgVm_t();

    int tick();

    void hideVms();
    void unhideVms();

private:
    void setBubbleLength(float l);
    void OpenSpeechBubble(int bubble, ns::vec3 const& pos, float length);
};

#endif // !GUIMSGVM_INCLUDED_H
