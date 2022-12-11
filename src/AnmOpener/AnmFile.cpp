#include "AnmFile.h"
#include "anmOpener.h"
#include <TextureManager.h>
#include <ImageLoader.h>
#include <GameSprites.h>

uint32_t textureFromImage(AnmOpener::image_t const& img)
{
    uint32_t texID = 0;
    GLuint glTexID = 0;
    glGenTextures(1, &glTexID);

    glBindTexture(GL_TEXTURE_2D, glTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    texID = NSEngine::TextureManager::AddTexture(glTexID, img.width, img.height);
    return texID;
}

void AnmSprite::genTexCoords(float tw, float th)
{
    u1 = x / tw;
    v1 = y / th;
    u2 = (x+w) / tw;
    v2 = (y+h) / th;
}

AnmFile::AnmFile(std::string const& filename, uint32_t slot)
{
    Open(filename, slot);
}

void AnmFile::Open(std::string const& filename, uint32_t slot)
{
    Cleanup();
    name = filename;
    AnmOpener::anm_archive_t* archive = AnmOpener::anm_read_file(filename);
    uint32_t scrID = 0;
    for (auto entry : archive->entries)
    {
        AnmOpener::image_t img = AnmOpener::anm_get_image(entry);
        uint32_t tex = textureFromImage(img);

        delete[] img.data;
        textures.insert(std::pair<std::string, uint32_t>(entry->name, tex));
        int w, h;
        NSEngine::TextureManager::GetTextureSize(tex, w, h);
        for (auto spr : entry->sprites)
        {
            sprites.push_back({tex, spr->x+entry->header->x, spr->y+entry->header->y, spr->w, spr->h});
            sprites.back().genTexCoords(w, h);
        }
        for (uint32_t scrid = 0; scrid < entry->scripts.size(); scrid++)
        {
            // generate data
            uint32_t size = 0;
            for (auto i : entry->scripts[scrid]->instrs) size += i->length;
            scripts.push_back(new int8_t[size+2]);
            uint32_t off = 0;
            for (auto ins : entry->scripts[scrid]->instrs)
            {
                for (uint32_t i = 0; i < ins->length; i++)
                {
                    scripts.back()[off++] = reinterpret_cast<int8_t*>(ins)[i];
                }
            }
            scripts.back()[size+0] = -1;
            scripts.back()[size+1] = -1;
            preloaded.emplace_back(scrID++, slot);
        }
    }

    AnmOpener::anm_free(archive);
    int i = 0;
    std::cout << "Opened Anm : " << filename << "\n";
}

void AnmFile::Cleanup()
{
    for (auto s : scripts) delete[] s;
    for (auto vm : preloaded)
        vm.destroy();
    preloaded.clear();
    scripts.clear();
    sprites.clear();
}

AnmFile::~AnmFile()
{
    //for (auto s : scripts) delete s;
}

int8_t* AnmFile::getScript(size_t id) const { if (scripts.size() == 0) return {}; return scripts[id%scripts.size()]; }
AnmVM AnmFile::getPreloaded(size_t id) const { if (preloaded.size() == 0) return {}; return preloaded[id%preloaded.size()]; }
AnmSprite AnmFile::getSprite(size_t id) const { if (sprites.size() == 0) return {}; return sprites[id%sprites.size()]; }
uint32_t AnmFile::getTextureFromName(std::string const& name) const { return textures.at(name); }
