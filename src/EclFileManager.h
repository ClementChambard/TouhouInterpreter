#ifndef ECLMANAGER_H_
#define ECLMANAGER_H_

#include "EclRaw.h"
#include <vector>
#include <StdOpener/StdFile.h>

class EclFileManager {
    public:
        static EclFileManager* GetInstance();

        ~EclFileManager() { CloseEcl(); }

        void LoadEcl(std::string file);
        void CloseEcl();
        void ListSubs();

        int getSubId(std::string subName) { for (int i = 0; i < loaded_subs.size(); i++) if (subName == loaded_subs[i].name) return i; return -1; }
        char* getSubStartPtr(int id) { return ((char*)loaded_subs[id].subHeader)+sizeof(EclRawSub_t); }
        std::string getSubName(int id) { if (id < 0 || id >= loaded_subs.size()) return "void"; return loaded_subs[id].name; }
        StdOpener::StdFile* stdf = nullptr;

    private:
        EclFileManager() {}

        void LoadEcli(std::string file);

        std::vector<EclSubPtr_t> loaded_subs;
        std::vector<EclRaw_t*> loaded_files;
};

#endif // ECLMANAGER_H_
