#ifndef HARDCODED_H_
#define HARDCODED_H_

#include <defines.h>
#include <string>
#include <vector>

extern struct HC_StageData {
  i32 id;
  std::string std_filename;
  std::string ecl_filename;
  std::string stage_bgm_filename;
  std::string boss_bgm_filename;
  std::vector<std::string> msg_filenames;
  std::string stage_logo_anm_filename;
  i32 stage_bgm_id;
  i32 boss_bgm_id;
  i32 unknown;
  struct {
    i32 spell_bg_anim_index;
    i32 spell_bg_anm_script;
    i32 stage_bg_visible_during_spell;
    i32 spell_portrait_anim_index;
    i32 spell_portrait_anm_script;
    i32 msg_ename_anim_index;
    i32 msg_ename_anm_script;
    i32 msg_face_anim_index;
    i32 msg_face_anm_script;
    i32 front_ename_anm_script;
  } boss_data[4];
} STAGE_DATA_TABLE[8];

extern struct HC_GuiAnms {
  i32 get_spell_card_bonus;
  i32 bonus_failed;
  i32 full_power;
  i32 high_score;
  i32 extend;
  i32 challenge_next_stage;
  i32 item_get_border_line;
  i32 stars_first;
  i32 difficulty_top_first;
  i32 difficulty_side_first;
  i32 spellcard_endtime;
  i32 boss_indicator;
  i32 shottype_side_first;
  i32 demo_play;
  i32 stage_clear;
  i32 speech_bubble;
  i32 speech_bubble_child;
  i32 boss_name_first;
  i32 boss_bar_first;
} GUI_ANMS;

extern struct HC_EffectData {
  i32 eff2_startanm;
  i32 popup_text_sprite;
} EFFECT_DATA;

extern struct HC_ItemAnmScripts {
  struct {
    i32 id_1, id_2;
  } item_kinds[32];
} ITEM_ANM_SCRIPT_IDS;

extern struct HC_BulletType {
  i32 script;
  struct {
    i32 main_sprite_id;
    i32 spawn_sprite_id;
    i32 cancel_sprite_id;
    i32 cancel_script;
  } colors[16];
  f32 default_radius;
  i32 default_layer;
  i32 __field_10c;
  i32 __field_110;
  i32 __field_114;
} *BULLET_TYPE_TABLE;

extern struct HC_LaserData {
  i32 spawn_anm_first;
  struct {
    i32 anm_type_0;
    i32 anm_first;
    i32 sprite_first;
  } laser_curve;
} LASER_DATA;

extern struct HC_PlayerData {
  struct HC_PlayerInfo {
    static constexpr u32 MAX_SHOT_TYPE_COUNT = 4;
    static constexpr u32 MAX_OPTION_POS_LOOKUP = 16;
    u32 shot_type_count;
    struct {
      std::string anm;
      std::string sht;
      u32 msg;
      u32 option_pos_lookup_cnt;
      u32 option_pos_lookup[MAX_OPTION_POS_LOOKUP];
    } shot_types[MAX_SHOT_TYPE_COUNT];
    i32 msg_face_anim_script;
    i32 option;
    i32 option_max;
    f32 hitbox;
    f32 grazebox;
    f32 itembox;
    f32 attractbox;
  } *players;
  u32 option_count;
} PLAYER_DATA;

extern int TOUHOU_VERSION;

void Hardcoded_Load();
void Hardcoded_Unload();

#endif // HARDCODED_H_
