#include "./EclFileManager.h"
#include "./EnemyManager.h"
#include "../Anm/AnmManager.h"
#include "../Bullet/BulletManager.h"
#include "./EclRaw.h"
#include "../Hardcoded.h"
#include <sys/stat.h>
#include <string>
#include <logger.h>

EclFileManager* EclFileManager::GetInstance() {
    static EclFileManager* instance = new EclFileManager();
    return instance;
}

inline bool file_exists(cstr name) {
    struct stat buffer;
    return (stat(name, &buffer) == 0);
}

void EclFileManager::LoadEcl(cstr file) {
    std::string f = file;
    if (!file_exists(file))
        f = STAGE_DATA_TABLE[GLOBALS.inner.STAGE_NUM]
            ["ecl_filename"].asString();

    NS_INFO("LOADING %s", f.c_str());
    CloseEcl();
    LoadEcli(f.c_str());
    NS_INFO("DONE     loaded %lld subs in %lld file", loaded_subs.size(), loaded_files.size());

    ENEMY_MANAGER_PTR->loadedAnms[0] = BulletManager::GetInstance()->bullet_anm;
    ENEMY_MANAGER_PTR->loadedAnms[1] = anm::getLoaded(8);
}

void EclFileManager::LoadEcli(cstr file) {
    EclRaw_t* ecl = ecl_open(file);
    if (!ecl)
        return;
    loaded_files.push_back(ecl);
    NS_INFO("  --> %lld subs loaded in file %s", ecl->subs.size(), file);
    for (auto e : ecl->subs)
        loaded_subs.push_back(e);
    for (auto ecli : ecl->ecli_list)
        LoadEcli(ecli);
    for (size_t i = 0; i < ecl->anim_list.size(); i++)
        ENEMY_MANAGER_PTR->loadedAnms[2+i] =
            anm::loadFile(i + 10, ecl->anim_list[i]);
}

void EclFileManager::CloseEcl() {
    loaded_subs.clear();
    for (auto f : loaded_files)
        ecl_close(f);
    loaded_files.clear();
}
