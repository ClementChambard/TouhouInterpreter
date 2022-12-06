#include "Spellcard.h"
#include "EnemyManager.h"
#include <AnmOpener/AnmManagerN.h>

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
    auto vm = NSEngine::AnmManagerN::getVM(spell_circle_anmid);
    if (!vm) { spell_circle_anmid = 0; }
    else {
        auto e = EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->getData()->boss_ids[0]);
        if(e) {
            spell_circle_x = (e->getData()->final_pos.pos.x - spell_circle_x) * 0.05 + spell_circle_x;
            spell_circle_y = (e->getData()->final_pos.pos.y - spell_circle_y) * 0.05 + spell_circle_y;
            spell_circle_z = (e->getData()->final_pos.pos.z - spell_circle_z) * 0.05 + spell_circle_z;
            vm->setEntityPos(spell_circle_x, spell_circle_y, spell_circle_z);
        }
        else {
            NSEngine::AnmManagerN::deleteVM(spell_circle_anmid);
            spell_circle_anmid = 0;
        }
    }
}

void Spellcard::Stop()
{
    NSEngine::AnmManagerN::deleteVM(spell_circle_anmid);
    spell_circle_anmid = 0;
}

void Spellcard::Init(int id, int time, int mode, std::string name)
{
    std::cout << "Spell card " << "  -> " << time << "\n";
    auto e = EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->getData()->boss_ids[0]);
    if(e) {
        spell_circle_anmid = NSEngine::AnmManagerN::SpawnVM(1, spellcard_anm_1);
        auto vm = NSEngine::AnmManagerN::getVM(spell_circle_anmid);
        spell_circle_x = e->getData()->final_pos.pos.x;
        spell_circle_y = e->getData()->final_pos.pos.y;
        spell_circle_z = e->getData()->final_pos.pos.z;
        vm->setEntityPos(spell_circle_x, spell_circle_y, spell_circle_z);

        vm->update();
        auto l = vm->getChild();
        while (l) { if (l->value) l->value->setI(2, time); l = l->next; }
    }
    NSEngine::AnmManagerN::getVM(NSEngine::AnmManagerN::SpawnVM(1, spellcard_anm_2));

}
