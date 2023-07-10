#ifndef ANMFILE_H_
#define ANMFILE_H_

#include <map>
#include <vector>
#include <string>
#include "anmOpener.h"
#include "AnmVM.h"

class AnmFile {

    public:
        AnmFile() {}
        AnmFile(std::string const& filename, uint32_t slot);
        void Open(std::string const& filename, uint32_t slot);
        void Cleanup();
        ~AnmFile();

        AnmVM getPreloaded(size_t id) const;
        int8_t* getScript(size_t id) const;
        AnmSprite getSprite(size_t id) const;
        uint32_t getTextureFromName(std::string const& name) const;
        std::string const& getName() const { return name; }
        size_t nbSprites() const { return sprites.size(); }
        size_t nbScripts() const { return scripts.size(); }

        void setSprite(AnmVM* vm, size_t sprite_id);

        uint32_t getSlot() const { return slot; }

    // private:
        uint32_t slot = 0;
        std::string name = "notLoaded";
        std::map<std::string, uint32_t> textures;
        std::vector<AnmSprite> sprites;
        std::vector<AnmVM> preloaded;
        std::vector<int8_t*> scripts;

        friend class AnmManager;
};

#endif // ANMFILE_H_
