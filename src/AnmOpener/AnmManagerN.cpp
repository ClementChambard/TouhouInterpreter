#include "AnmBitflags.h"
#include "AnmManagerN.h"
#include <TextureManager.h>
#include <DrawFuncs.h>
#include <NSlist.h>

std::array<AnmFile, 31> AnmManagerN::loadedFiles;
AnmVMList* AnmManagerN::first = nullptr;
AnmVMList* AnmManagerN::last = nullptr;
AnmVMList* AnmManagerN::uiFirst = nullptr;
AnmVMList* AnmManagerN::uiLast = nullptr;
AnmFastVM  AnmManagerN::fastArray[8191];
AnmFastVMList* AnmManagerN::fastFirst = nullptr;
int32_t AnmManagerN::last_id_discriminator = 0;

void AnmManagerN::Init()
{
    first = new AnmVMList();
    uiFirst = new AnmVMList();
    last = new AnmVMList();
    uiLast = new AnmVMList();

    first->next = last;
    last->previous = first;
    uiLast->previous = uiFirst;
    uiFirst->next = uiLast;

    fastFirst = new AnmFastVMList();

    for (size_t i = 0; i < 8191; i++)
    {
        fastArray[i].freelistNode = new AnmFastVMList();
        fastArray[i].freelistNode->value = &(fastArray[i]);
        fastArray[i].fastID = i;
        fastArray[i].isAlive = false;
        ListUtil::listInsertAfter(fastFirst, fastArray[i].freelistNode);
    }
}

void AnmManagerN::Cleanup()
{
    /* DESTROY HEAP VMS IN WORLD LIST */
    auto n = first;
    while(n->next != last)
    {
        if ((n->next->value->id.val & AnmID::fastIdMask) == AnmID::fastIdMask) delete n->next->value;
        else n = n->next;
    }

    /* DESTROY HEAP VMS IN UI LIST */
    n = uiFirst;
    while(n->next != uiLast)
    {
        if ((n->next->value->id.val & AnmID::fastIdMask) == AnmID::fastIdMask) delete n->next->value;
        else n = n->next;
    }

    /* CLEAN FAST VMS */
    for (size_t i = 0; i < 8191; i++)
    {
        delete fastArray[i].freelistNode;
        fastArray[i].vm.destroy();
    }

    /* CLEAN PREINIT VM */
    for (auto f : loadedFiles)
        f.Cleanup();
}

AnmVM* AnmManagerN::SpawnVMExt(size_t slot, size_t script)
{
    AnmVM* activeVM = new AnmVM(loadedFiles[slot].getPreloaded(script));
    activeVM->id = AnmID::fastIdMask;
    activeVM->id.setDiscriminator(last_id_discriminator++);
    activeVM->script_id = script;
    return activeVM;
}

uint32_t AnmManagerN::SpawnVM(size_t slot, size_t script, bool ui, bool front)
{
    if (loadedFiles[slot].name == "notLoaded") return -1;

    AnmVM* activeVM;

    auto n = fastFirst->next;
    if (n == nullptr)
    {
        //no free spot in the array
        activeVM = SpawnVMExt(slot, script);
    }
    else
    {
        ListUtil::listRemoveNode(n);
        n->value->fastID.setDiscriminator(++last_id_discriminator);
        n->value->isAlive = true;
        n->value->vm(loadedFiles[slot].getPreloaded(script));
        activeVM = &n->value->vm;
        activeVM->id = n->value->fastID;
    }

    if (ui)
    {
        if (front) activeVM->nodeInGlobalList = ListUtil::listInsertAfter(uiFirst, activeVM);
        else activeVM->nodeInGlobalList = ListUtil::listInsertBefore(uiLast, activeVM);
    }
    else
    {
        if (front) activeVM->nodeInGlobalList = ListUtil::listInsertAfter(first, activeVM);
        else activeVM->nodeInGlobalList = ListUtil::listInsertBefore(last, activeVM);
    }
    activeVM->script_id = script; // FIXME: this is supposed to be correct from the beginning
    return activeVM->id.val;
}

void AnmManagerN::killAll()
{
    /* DESTROY HEAP VMS IN WORLD LIST */
    auto n = first;
    while(n->next != last)
    {
        if ((n->next->value->id.val & AnmID::fastIdMask) == AnmID::fastIdMask) delete n->next->value;
        else n = n->next;
    }

    /* DESTROY HEAP VMS IN UI LIST */
    n = uiFirst;
    while(n->next != uiLast)
    {
        if ((n->next->value->id.val & AnmID::fastIdMask) == AnmID::fastIdMask) delete n->next->value;
        else n = n->next;
    }

    /* CLEAN FAST VMS */
    for (size_t i = 0; i < 8191; i++)
    {
        //delete fastArray[i].freelistNode;
        fastArray[i].vm.destroy();
    }

    /* CLEAN PREINIT VM */
    //for (auto f : loadedFiles)
        //f.Cleanup();
}

void AnmManagerN::deleteVM(uint32_t id)
{
    if ((id & AnmID::fastIdMask) == AnmID::fastIdMask)
    {
        for (auto node = first->next; node != last; node = node->next)
            if (node && node->value->id == id)
            {
                auto vm = node->value;
                vm->destroy();
                delete vm;
                return;
            }
        for (auto node = uiFirst->next; node != uiLast; node = node->next)
            if (node && node->value->id == id)
            {
                auto vm = node->value;
                vm->destroy();
                delete vm;
                return;
            }
        return;
    }
    if (fastArray[id & AnmID::fastIdMask].fastID != id) return;
    fastArray[id & AnmID::fastIdMask].vm.destroy();
    fastArray[id & AnmID::fastIdMask].isAlive = false;
    ListUtil::listInsertAfter(fastFirst, fastArray[id & AnmID::fastIdMask].freelistNode);
}

bool AnmManagerN::isAlive(uint32_t id)
{
    if ((id & AnmID::fastIdMask) == AnmID::fastIdMask)
    {
        for (auto node = first->next; node != last; node = node->next)
            if (node && node->value->id == id)
                return true;
        for (auto node = uiFirst->next; node != uiLast; node = node->next)
            if (node && node->value->id == id)
                return true;
        return false;
    }
    if (fastArray[id & AnmID::fastIdMask].fastID != id) return false;
    return fastArray[id & AnmID::fastIdMask].isAlive;
}

AnmVM* AnmManagerN::getVM(uint32_t id)
{
    if (id == 0) return nullptr;
    if ((id & AnmID::fastIdMask) == AnmID::fastIdMask)
    {
        for (auto node = first->next; node != last; node = node->next)
            if (node && node->value->id == id)
                return node->value;
        for (auto node = uiFirst->next; node != uiLast; node = node->next)
            if (node && node->value->id == id)
                return node->value;
        return nullptr;
    }
    if (fastArray[id & AnmID::fastIdMask].fastID != id) return nullptr;
    if (fastArray[id & AnmID::fastIdMask].isAlive) return &(fastArray[id & AnmID::fastIdMask].vm);
    return nullptr;
}

AnmFile* AnmManagerN::LoadFile(size_t slot, std::string filename)
{
    if (loadedFiles[slot].name != "notLoaded") { /* do something */ }
    loadedFiles[slot].Open(filename, slot);
    return &loadedFiles[slot];
}

void AnmManagerN::update(bool /*printInstr*/)
{
    on_tick_ui();
    on_tick_world();
}

int AnmManagerN::getFreeAnm()
{
    int n = 0;
    for (int i = 0; i < 8191; i++) if (!fastArray[i].isAlive) n++;
    return n;
}

void AnmManagerN::on_tick_world()
{
    for (auto node = first->next; node != last; node = node->next)
    {
        if (node->value)
        {
            node->value->update();
            bool del = (node->value->bitflags.activeFlags & 1) && !(node->value->bitflags.activeFlags & 2);
            if (del)
            {
                int id = node->value->id.val;
                node = node->previous;
                node->next->value->destroy();
                if (fastArray[id & AnmID::fastIdMask].fastID != id) continue;
                fastArray[id & AnmID::fastIdMask].isAlive = false;
                ListUtil::listInsertAfter(fastFirst, fastArray[id & AnmID::fastIdMask].freelistNode);
            }
        }
    }
}

void AnmManagerN::on_tick_ui()
{
    for (auto node = uiFirst->next; node != uiLast; node = node->next)
    {
        if (node->value)
        {
            node->value->update();
            bool del = (node->value->bitflags.activeFlags & 1) && !(node->value->bitflags.activeFlags & 2);
            if (del)
            {
                int id = node->value->id.val;
                node = node->previous;
                node->next->value->destroy();
                if (fastArray[id & AnmID::fastIdMask].fastID != id) continue;
                fastArray[id & AnmID::fastIdMask].isAlive = false;
                ListUtil::listInsertAfter(fastFirst, fastArray[id & AnmID::fastIdMask].freelistNode);
            }
        }
    }
}

void AnmManagerN::draw()
{
    for (auto node = first->next; node != last; node = node->next)
        if (node->value) node->value->draw();
    for (auto node = uiFirst->next; node != uiLast; node = node->next)
        if (node->value) node->value->draw();
}

void AnmManagerN::on_draw(uint32_t layer)
{
    for (auto node = first->next; node != last; node = node->next)
        if (node->value && node->value->layer == layer) node->value->draw();
    for (auto node = uiFirst->next; node != uiLast; node = node->next)
        if (node->value && node->value->layer == layer) node->value->draw();
}

void AnmManagerN::drawSprite(size_t slot, size_t& spriteID)
{
    if (spriteID == (size_t)-1) spriteID = 0;
    if (spriteID >= loadedFiles[slot].sprites.size()) spriteID = loadedFiles[slot].sprites.size()-1;
    AnmSprite s = loadedFiles[slot].getSprite(spriteID);
    int tw = 1, th = 1;
    NSEngine::TextureManager::GetTextureSize(s.texID, tw, th);
    float u1 = s.x/(float)tw, u2 = (s.x+s.w)/(float)tw;
    float v1 = s.y/(float)th, v2 = (s.y+s.h)/(float)th;
    NSEngine::draw_quad_tex_2d(NSEngine::TextureManager::GetTextureID(s.texID), {-s.w/2.f, +s.h/2.f, u1, v1}, {+s.w/2.f, +s.h/2.f, u2, v1}, {+s.w/2.f, -s.h/2.f, u2, v2}, {-s.w/2.f, -s.h/2.f, u1, v2});
}

void AnmManagerN::drawTexture(size_t slot, size_t& texID)
{
    std::map<std::string, uint32_t>& texturesmap = loadedFiles[slot].textures;
    if (texID == (size_t)-1) texID = 0;
    if (texID >= texturesmap.size()) texID = texturesmap.size()-1;
    size_t i = 0;
    uint32_t tex = -1;
    for (auto t : texturesmap)
        if (i++ == texID) { tex = t.second; break; }
    int w = 0, h = 0;
    NSEngine::TextureManager::GetTextureSize(tex, w, h);
    NSEngine::draw_quad_tex_2d(NSEngine::TextureManager::GetTextureID(tex), {-w/2.f, +h/2.f, 0.f, 0.f}, {+w/2.f, +h/2.f, 1.f, 0.f}, {+w/2.f, -h/2.f, 1.f, 1.f}, {-w/2.f, -h/2.f, 0.f, 1.f});
}
