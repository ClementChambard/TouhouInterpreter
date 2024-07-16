#ifndef ECLMANAGER_H_
#define ECLMANAGER_H_

#include "./EclRaw.h"
#include <vector>
#include <defines.h>
#include <cstring>

class EclFileManager {
public:
    static EclFileManager* GetInstance();

    ~EclFileManager() { CloseEcl(); }

    void LoadEcl(cstr file);
    void CloseEcl();

    usize getSubId(cstr subName) {
        for (usize i = 0; i < loaded_subs.size(); i++)
            if (strcmp(subName, loaded_subs[i].name) == 0)
                return i;
        return -1;
    }
    cstr getSubStartPtr(usize id) {
        if (id >= loaded_subs.size())
            return nullptr;
        return reinterpret_cast<cstr>(loaded_subs[id].subHeader)
               + sizeof(EclRawSub_t);
    }
    cstr getSubName(usize id) {
        if (id >= loaded_subs.size())
            return "void";
        return loaded_subs[id].name;
    }

private:
    EclFileManager() { }

    void LoadEcli(cstr file);

    std::vector<EclSubPtr_t> loaded_subs;
    std::vector<EclRaw_t*> loaded_files;
};

#endif // ECLMANAGER_H_
