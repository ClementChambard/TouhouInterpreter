#include "./Hardcoded.h"
#include <asserts.h>
#include <fstream>
#include <logger.h>
#include <json/json.h>

HC_StageData STAGE_DATA_TABLE[8];
HC_PlayerData PLAYER_DATA;
HC_BulletType *BULLET_TYPE_TABLE;
HC_ItemAnmScripts ITEM_ANM_SCRIPT_IDS;
HC_LaserData LASER_DATA;
HC_GuiAnms GUI_ANMS;
HC_EffectData EFFECT_DATA;

int TOUHOU_VERSION = 0;

void load_player_data(Json::Value const &value) {
  PLAYER_DATA.option_count = value.get("option_count", 4).asInt();
  Json::Value const &pl_arr = value.get("players", Json::Value{});
  if (!pl_arr.isArray())
    return;
  usize n = pl_arr.size();
  PLAYER_DATA.players = new HC_PlayerData::HC_PlayerInfo[n];
  for (usize i = 0; i < n; i++) {
    auto &p = PLAYER_DATA.players[i];
    Json::Value const &v = pl_arr.get(i, Json::Value{});
    p.msg_face_anim_script = v.get("msg_face_anim_script", 0).asInt();
    p.option = v.get("option", 0).asInt();
    p.option_max = v.get("option_max", 0).asInt();
    p.hitbox = v.get("hitbox", 1.0f).asFloat();
    p.grazebox = v.get("grazebox", 1.0f).asFloat();
    p.itembox = v.get("itembox", 1.0f).asFloat();
    p.attractbox = v.get("attractbox", 1.0f).asFloat();
    Json::Value const &st = v.get("shottypes", Json::Value{});
    p.shot_type_count = st.isArray() ? st.size() : 0;
    NS_ASSERT(p.shot_type_count <
              HC_PlayerData::HC_PlayerInfo::MAX_SHOT_TYPE_COUNT);
    for (usize j = 0; j < p.shot_type_count; j++) {
      Json::Value const &v = st.get(j, Json::Value{});
      auto &s = p.shot_types[j];
      s.anm = v.get("anm", "pl00.anm").asString();
      s.sht = v.get("sht", "pl00.sht").asString();
      s.msg = v.get("msg", 0).asInt();
      Json::Value const &opl = v.get("option_pos_lookup", Json::Value{});
      s.option_pos_lookup_cnt = opl.isArray() ? opl.size() : 0;
      NS_ASSERT(s.option_pos_lookup_cnt <
                HC_PlayerData::HC_PlayerInfo::MAX_OPTION_POS_LOOKUP);
      for (usize k = 0; k < s.option_pos_lookup_cnt; k++) {
        s.option_pos_lookup[k] = opl.get(k, 0).asInt();
      }
    }
  }
}

void load_item_anm_scripts(Json::Value const &array) {
  for (usize i = 0; i < 32; i++) {
    Json::Value const &v = array.get(i, Json::Value{});
    auto &ik = ITEM_ANM_SCRIPT_IDS.item_kinds[i];
    ik.id_1 = v.get("id_1", 3).asInt();
    ik.id_2 = v.get("id_2", 3).asInt();
  }
}

void load_bullet_table(Json::Value const &array) {
  if (!array.isArray())
    return;
  usize n = array.size();
  BULLET_TYPE_TABLE = new HC_BulletType[n];
  for (usize i = 0; i < n; i++) {
    auto &bt = BULLET_TYPE_TABLE[i];
    Json::Value const &v = array.get(i, Json::Value{});
    bt.script = v.get("script", 0).asInt();
    bt.default_radius = v.get("default_radius", 1.f).asFloat();
    bt.default_layer = v.get("default_layer", 0).asInt();
    bt.__field_10c = v.get("__field_10c", 6).asInt();
    bt.__field_110 = v.get("__field_110", 6).asInt();
    bt.__field_114 = v.get("__field_114", 0).asInt();
    Json::Value const &col = v.get("colors", Json::Value{});
    for (usize j = 0; j < 16; j++) {
      Json::Value const &v = col.get(j, Json::Value{});
      bt.colors[j].main_sprite_id = v.get("main_sprite_id", 0).asInt();
      bt.colors[j].spawn_sprite_id = v.get("spawn_sprite_id", 0).asInt();
      bt.colors[j].cancel_sprite_id = v.get("cancel_sprite_id", 0).asInt();
      bt.colors[j].cancel_script = v.get("cancel_script", 0).asInt();
    }
  }
}

void load_laser_data(Json::Value const &value) {
  LASER_DATA.spawn_anm_first = value.get("spawn_anm_first", 56).asInt();
  auto &lc = LASER_DATA.laser_curve;
  Json::Value const &v = value.get("laser_curve", Json::Value{});
  lc.anm_type_0 = v.get("anm_type_0", 322).asInt();
  lc.anm_first = v.get("anm_first", 142).asInt();
  lc.sprite_first = v.get("sprite_first", 524).asInt();
}

void load_effect_data(Json::Value const &value) {
  EFFECT_DATA.eff2_startanm = value.get("eff2_startanm", 150).asInt();
  EFFECT_DATA.popup_text_sprite = value.get("popup_text_sprite", 259).asInt();
}

void load_gui_anms(Json::Value const &value) {
  GUI_ANMS.get_spell_card_bonus = value.get("get_spell_card_bonus", 61).asInt();
  GUI_ANMS.bonus_failed = value.get("bonus_failed", 62).asInt();
  GUI_ANMS.full_power = value.get("full_power", 63).asInt();
  GUI_ANMS.high_score = value.get("high_score", 64).asInt();
  GUI_ANMS.extend = value.get("extend", 65).asInt();
  GUI_ANMS.challenge_next_stage = value.get("challenge_next_stage", 66).asInt();
  GUI_ANMS.item_get_border_line = value.get("item_get_border_line", 69).asInt();
  GUI_ANMS.stars_first = value.get("stars_first", 70).asInt();
  GUI_ANMS.difficulty_top_first = value.get("difficulty_top_first", 81).asInt();
  GUI_ANMS.difficulty_side_first =
      value.get("difficulty_side_first", 87).asInt();
  GUI_ANMS.spellcard_endtime = value.get("spellcard_endtime", 96).asInt();
  GUI_ANMS.boss_indicator = value.get("boss_indicator", 112).asInt();
  GUI_ANMS.shottype_side_first = value.get("shottype_side_first", 113).asInt();
  GUI_ANMS.demo_play = value.get("demo_play", 122).asInt();
  GUI_ANMS.stage_clear = value.get("stage_clear", 123).asInt();
  GUI_ANMS.speech_bubble = value.get("speech_bubble", 287).asInt();
  GUI_ANMS.speech_bubble_child = value.get("speech_bubble_child", 183).asInt();
  GUI_ANMS.boss_name_first = value.get("boss_name_first", 167).asInt();
  GUI_ANMS.boss_bar_first = value.get("boss_bar_first", 39).asInt();
}

void load_stage_data(Json::Value const &value) {
  for (usize i = 0; i < 8; i++) {
    auto &sd = STAGE_DATA_TABLE[i];
    Json::Value const &v = value.get(i, Json::Value{});
    sd.id = v.get("id", i32(i)).asInt();
    auto i_str = std::to_string(i);
    sd.std_filename = v.get("std_filename", "st0" + i_str + ".std").asString();
    sd.ecl_filename = v.get("ecl_filename", "st0" + i_str + ".ecl").asString();
    sd.stage_bgm_filename = v.get("stage_bgm_filename", "").asString();
    sd.boss_bgm_filename = v.get("boss_bgm_filename", "").asString();
    sd.stage_logo_anm_filename =
        v.get("stage_logo_anm_filename", "st0" + i_str + "logo.anm").asString();
    sd.stage_bgm_id = v.get("stage_bgm_id", 0).asInt();
    sd.boss_bgm_id = v.get("boss_bgm_id", 0).asInt();
    sd.unknown = v.get("unknown", 0).asInt();
    Json::Value const &msg = v.get("msg_filenames", Json::Value{});
    usize msg_cnt = msg.isArray() ? msg.size() : 0;
    sd.msg_filenames.reserve(msg_cnt);
    for (auto v : msg) {
      sd.msg_filenames.push_back(v.asString());
    }
    Json::Value const &arr = v.get("boss_data", Json::Value{});
    for (usize j = 0; j < 4; j++) {
      auto &b = sd.boss_data[j];
      Json::Value const &v = arr.get(j, Json::Value{});
      b.spell_bg_anim_index = v.get("spell_bg_anim_index", 0).asInt();
      b.spell_bg_anm_script = v.get("spell_bg_anm_script", 0).asInt();
      b.stage_bg_visible_during_spell =
          v.get("stage_bg_visible_during_spell", 0).asInt();
      b.spell_portrait_anim_index =
          v.get("spell_portrait_anim_index", 0).asInt();
      b.spell_portrait_anm_script =
          v.get("spell_portrait_anm_script", 0).asInt();
      b.msg_ename_anim_index = v.get("msg_ename_anim_index", 0).asInt();
      b.msg_ename_anm_script = v.get("msg_ename_anm_script", 0).asInt();
      b.msg_face_anim_index = v.get("msg_face_anim_index", 0).asInt();
      b.msg_face_anm_script = v.get("msg_face_anm_script", 0).asInt();
      b.front_ename_anm_script = v.get("front_ename_anm_script", 0).asInt();
    }
  }
}

void prepare_vals(Json::Value const &data) {
  load_bullet_table(data.get("bullet_type_table", Json::Value{}));
  load_item_anm_scripts(data.get("item_anm_script_ids", Json::Value{}));
  load_laser_data(data.get("laser_data", Json::Value{}));
  load_player_data(data.get("player_data", Json::Value{}));
  load_stage_data(data.get("stage_data_table", Json::Value{}));
  TOUHOU_VERSION = data.get("version", 0).asInt();
  load_gui_anms(data.get("gui", Json::Value{}));
  load_effect_data(data.get("effect", Json::Value{}));
}

void Hardcoded_Load() {
  std::ifstream file("hardcoded.json");

  if (file.fail()) {
    NS_WARN("no hardcoded data in pwd: checking in parend directory");
    file.open("../hardcoded.json");
    if (file.fail()) {
      NS_ERROR("hardcoded data not found");
      return;
    }
    Json::Reader reader;
    Json::Value v;
    reader.parse(file, v);
    file.close();
    prepare_vals(v);
    return;
  }

  Json::Reader reader;

  Json::Value v;
  reader.parse(file, v);
  file.close();
  prepare_vals(v);
}

void Hardcoded_Unload() {
  delete[] BULLET_TYPE_TABLE;
  delete[] PLAYER_DATA.players;
}
