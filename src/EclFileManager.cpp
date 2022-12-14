#include "EclFileManager.h"
#include "EclRaw.h"
#include "AnmOpener/AnmManagerN.h"
#include "Hardcoded.h"
#include <sys/stat.h>

EclFileManager* EclFileManager::GetInstance()
{
    static EclFileManager* instance = new EclFileManager();
    return instance;
}

inline bool file_exists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

void EclFileManager::LoadEcl(std::string file)
{
    if (!file_exists(file)) file = STAGE_DATA_TABLE[Globals::get()->stage_id]["ecl_filename"].asString();

    std::cout << "LOADING " << file << "\n";
    CloseEcl();
    LoadEcli(file);
    std::cout << "DONE     loaded " << loaded_subs.size() << " subs in " << loaded_files.size() << " file\n";

    std::string logoFile = STAGE_DATA_TABLE[Globals::get()->stage_id]["stage_logo_anm_filename"].asString();
    if (file_exists(logoFile)) AnmManagerN::LoadFile(8, logoFile);

    std::string stdFile = STAGE_DATA_TABLE[Globals::get()->stage_id]["std_filename"].asString();
    if (file_exists(stdFile)) {
        stdf = new StdOpener::StdFile(stdFile);
        stdf->Init();
    }
}

void EclFileManager::LoadEcli(std::string file)
{
    EclRaw_t* ecl = ecl_open(file);
    if (!ecl) return;
    loaded_files.push_back(ecl);
    std::cout << "  --> " << ecl->subs.size() << " subs loaded in file " << file << "\n";
    for (auto sp : ecl->subs)
        loaded_subs.push_back(sp);
    for (auto ecli : ecl->ecli_list)
        LoadEcli(ecli);
    for (int i = 0; i < ecl->anim_list.size(); i++)
        AnmManagerN::LoadFile(i+2, ecl->anim_list[i]);
}

void EclFileManager::CloseEcl()
{
    loaded_subs.clear();
    for (auto f : loaded_files) ecl_close(f);
    loaded_files.clear();
    if (stdf) delete stdf;
    stdf = nullptr;
}

void EclFileManager::ListSubs()
{
    int i = 0;
    for (EclSubPtr_t s : loaded_subs)
    {
        std::string sn = s.name; while (sn.length() < 30) { sn = " " + sn; if (sn.length() < 30) sn = sn + " "; }
        std::cout << sn;
        if (!(++i%5)) std::cout << "\n"; else std::cout << "---";
    }
    std::cout << "\n";
}
