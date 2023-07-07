#ifndef STDFILE_H_
#define STDFILE_H_

#include <iostream>
#include <vector>
#include <string>

#include "../AnmOpener/AnmVM.h"
#include <AnimShader.h>
#include <Camera3D.h>
#include <SpriteBatch.h>

namespace StdOpener {

class StdFile {

public:
    explicit StdFile(std::string const& filename);
    ~StdFile();

    void Init();
    void Clear();

    void Update();
    void Draw();

    void interrupt(int s);

private:
    struct quad {
        uint16_t size;
        uint16_t script_index;
        float x = 0, y = 0, z = 0;
        float width = 256, height = 256;
    };

    struct entry {
        uint16_t layer = 0;
        float x = 0, y = 0, z = 0;
        float width = 256, height = 256, depth = 256;
        std::vector<quad> quads;
    };

    struct face {
        size_t entry_id;
        float x = 0, y = 0, z = 0;
    };

    union instrArg {
        int S;
        float f;
    };
    struct instruction {
        uint32_t time;
        uint16_t type;
        uint32_t offset;
        std::vector<instrArg> args;
    };

    std::string anm_file_name;

    std::vector<entry> entries;
    std::vector<face> faces;
    std::vector<instruction> script;
    NSEngine::Camera3D* theCam = nullptr;
    bool stopped = false;

    uint32_t instr = 0;
    uint32_t time = 0;
    uint16_t pending_switch_label = 0;

    int anmSlots[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
    std::vector<AnmVM*> bgVms;

    void spawnFace(face const& f);
    void execInstr(instruction const& i);

    NSEngine::SpriteBatch sprBatch;
    NSEngine::AnimShader baseShader;
};

}  // namespace StdOpener

#endif
