#include "./EclFileManager.h"
#include "./EnemyManager.h"
#include "./AnmOpener/AnmManager.h"
#include "./BulletManager.h"
#include "./EclRaw.h"
#include "./Hardcoded.h"
#include <sys/stat.h>
#include <string>
#include <Error.h>

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

    ns::info("LOADING", f);
    CloseEcl();
    LoadEcli(f.c_str());
    ns::info("DONE     loaded", loaded_subs.size(), "subs in", loaded_files.size(), "file");

    ENEMY_MANAGER_PTR->loadedAnms[0] = BulletManager::GetInstance()->bullet_anm;
    ENEMY_MANAGER_PTR->loadedAnms[1] = anm::getLoaded(8);
}

void EclFileManager::LoadEcli(cstr file) {
    EclRaw_t* ecl = ecl_open(file);
    if (!ecl)
        return;
    loaded_files.push_back(ecl);
    ns::info("  -->", ecl->subs.size(), "subs loaded in file", file);
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
