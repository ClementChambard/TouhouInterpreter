#include "Spellcard.h"
#include "EnemyManager.h"
#include "AnmOpener/AnmManagerN.h"

Spellcard* Spellcard::GetInstance()
{
    static Spellcard* inst = new Spellcard();
    return inst;
}

Spellcard::Spellcard()
{
    f_on_tick = new UpdateFunc([this](){ return this->on_tick(); });
    f_on_draw = new UpdateFunc([this](){ return this->on_draw(); });
    UPDATE_FUNC_REGISTRY->register_on_tick(f_on_tick, 32);
    UPDATE_FUNC_REGISTRY->register_on_draw(f_on_draw, 12);
}

Spellcard::~Spellcard()
{
    UPDATE_FUNC_REGISTRY->unregister(f_on_tick);
    UPDATE_FUNC_REGISTRY->unregister(f_on_draw);
}

void Spellcard::update()
{
    auto vm = AnmManagerN::getVM(spell_circle_anmid);
    if (!vm) { spell_circle_anmid = 0; }
    else {
        auto e = EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->getData()->boss_ids[bossId]);
        if(e) {
            spell_circle_x = (e->getData()->final_pos.pos.x - spell_circle_x) * 0.05 + spell_circle_x;
            spell_circle_y = (e->getData()->final_pos.pos.y - spell_circle_y) * 0.05 + spell_circle_y;
            spell_circle_z = (e->getData()->final_pos.pos.z - spell_circle_z) * 0.05 + spell_circle_z;
            vm->setEntityPos(spell_circle_x, spell_circle_y, spell_circle_z);
        }
        else {
            AnmManagerN::deleteVM(spell_circle_anmid);
            spell_circle_anmid = 0;
        }
    }
}

void Spellcard::Stop()
{
    AnmManagerN::deleteVM(spell_circle_anmid);
    AnmManagerN::deleteVM(spell_bg_anm_id);
    spell_circle_anmid = 0;
}

#include "Hardcoded.h"

void Spellcard::Init(int /*id*/, int time, int /*mode*/, std::string /*name*/, int bossId)
{
    std::cout << "Spell card " << "  -> " << time << "\n";
    this->bossId = bossId;
    auto e = EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->getData()->boss_ids[bossId]);
    if(e) {
        spell_circle_anmid = AnmManagerN::SpawnVM(1, spellcard_anm_1);
        auto vm = AnmManagerN::getVM(spell_circle_anmid);
        spell_circle_x = e->getData()->final_pos.pos.x;
        spell_circle_y = e->getData()->final_pos.pos.y;
        spell_circle_z = e->getData()->final_pos.pos.z;
        vm->setEntityPos(spell_circle_x, spell_circle_y, spell_circle_z);

        vm->update();
        auto l = vm->getChild();
        while (l) { if (l->value) l->value->setI(2, time); l = l->next; }
    }
    AnmManagerN::getVM(AnmManagerN::SpawnVM(1, spellcard_anm_2));
    spell_bg_anm_id = AnmManagerN::SpawnVM(STAGE_DATA_TABLE[Globals::get()->stage_id]["boss_data"][bossId]["spell_bg_anim_index"].asInt(),
                                           STAGE_DATA_TABLE[Globals::get()->stage_id]["boss_data"][bossId]["spell_bg_anm_script"].asInt());
    AnmManagerN::SpawnVM(STAGE_DATA_TABLE[Globals::get()->stage_id]["boss_data"][bossId]["spell_portrait_anim_index"].asInt(),
                         STAGE_DATA_TABLE[Globals::get()->stage_id]["boss_data"][bossId]["spell_portrait_anm_script"].asInt());
}
