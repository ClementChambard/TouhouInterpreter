#ifndef ANMMANAGER_H_
#define ANMMANAGER_H_

#include "AnmFile.h"
#include "AnmVM.h"
#include <array>

class AnmManager {

    public:

        static void Init();
        static void Cleanup();

        static uint32_t SpawnVM(size_t slot, size_t script, bool ui = false, bool front = false);
        static AnmVM* SpawnVMExt(size_t slot, size_t script);

        static AnmVM* getVM(uint32_t id);
        static AnmVM* getVM(AnmID id) { return getVM(id.val); }

        static void deleteVM(uint32_t id);
        static void killAll();
        static bool isAlive(uint32_t id);
        static AnmFile* LoadFile(size_t slot, std::string filename);

        static void update(bool printInstr = false);
        static void draw();
        static void on_tick_world();
        static void on_tick_ui();
        static void on_draw(uint32_t layer);

        static void drawSprite(size_t slot, size_t& spriteID);
        static void drawTexture(size_t slot, size_t& texID);

        static int getFreeAnm();

        static AnmFile* getLoaded(int i) { return &loadedFiles[i]; }

        static void interrupt_tree(AnmID id, int interrupt) { auto vm = getVM(id.val); if (vm) vm->interruptRec(interrupt); }
    private:

        //216 unknown bytes + int32 useless_count

        static AnmVMList* first;
        static AnmVMList* last;
        static AnmVMList* uiFirst;
        static AnmVMList* uiLast;

        static AnmFastVM fastArray[8191];

        //lolk fields

        static AnmFastVMList* fastFirst;
        //int32 unknown

        static std::array<AnmFile, 31> loadedFiles;

        static int32_t last_id_discriminator;

        static bool initialized;

        friend class AnmVM;

};



#endif // ANMMANAGER_H_
