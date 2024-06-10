#ifndef ANMFILE_H_
#define ANMFILE_H_

#include "./AnmVM.h"
#include <map>
#include <vector>
#include <defines.h>
#include <string>

namespace anm {

struct File {
    File() {}
    File(cstr filename, u32 slot);
    void Open(cstr filename, u32 slot);
    void Cleanup();
    ~File();
    
    VM getPreloaded(usize id) const;
    bytes getScript(usize id) const;
    Sprite getSprite(usize id) const;
    u32 getTextureFromName(cstr name) const;
    cstr getName() const { return name.c_str(); }
    usize nbSprites() const { return sprites.size(); }
    usize nbScripts() const { return scripts.size(); }
    
    void setSprite(VM* vm, usize sprite_id);
    
    void copyFromLoaded(VM* vm, i32 id);
    
    void load_external_vm(VM* vm, i32 id);
    ID spawnVMExt(i32 id, f32 rot, glm::vec3 const& pos, i32 mode,
                  i32 layer = -1, VM** out_vm = nullptr);
    ID createEffect(i32 id, i32 layer = -1, VM** output = nullptr);
    ID createEffectFront(i32 id, i32 layer = -1, VM** output
                         = nullptr);
    ID createEffectPos(i32 id, f32 rot, glm::vec3 const& pos,
                       i32 layer = -1, VM** out_vm = nullptr);
    ID new_vm_ui_back(i32 id, VM** out_vm = nullptr);
    ID create_child_vm(i32 id, VM *parent, i32 mode);
    ID new_root(i32 id, VM *parent);
    
    u32 getSlot() const { return slot; }
    
    u32 slot = 0;
    i32 some_ctr = 0;
    std::string name = "";
    std::map<std::string, u32> textures;
    std::vector<Sprite> sprites;
    std::vector<VM> preloaded;
    std::vector<bytes> scripts;
};

} // namespace anm

#endif // ANMFILE_H_
