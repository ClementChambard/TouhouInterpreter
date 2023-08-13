#include "./StdFile.h"
#include <algorithm>

#include "../AnmOpener/AnmManager.h"
#include "./stdOpener.h"
#include <Engine.hpp>
#include <NSEngine.h>
#include "../GlobalData.h"
#include "../Supervisor.h"

extern float SURF_ORIGIN_FULL_X_;
extern float SURF_ORIGIN_FULL_Y_;

namespace StdOpener {

void StdFile::interrupt(int s) { pending_switch_label = s; }

StdFile::StdFile(std::string const& filename) {
    thstd_t* std = std_read_file(filename);

    for (auto e : std->entries) {
        entries.push_back({});
        entries.back().layer = e->header->layer;
        entries.back().x = e->header->x;
        entries.back().y = e->header->y;
        entries.back().z = e->header->z;
        entries.back().width = e->header->width;
        entries.back().height = e->header->height;
        entries.back().depth = e->header->depth;
        // std::cout << "Entry #" << i++ << ":\n";
        // std::cout << "    layer = " << entries.back().layer  << "\n";
        // std::cout << "        x = " << entries.back().x      << "\n";
        // std::cout << "        y = " << entries.back().y      << "\n";
        // std::cout << "        z = " << entries.back().z      << "\n";
        // std::cout << "    width = " << entries.back().width  << "\n";
        // std::cout << "   height = " << entries.back().height << "\n";
        // std::cout << "    depth = " << entries.back().depth  << "\n";
        // std::cout << "    quads = {\n";
        for (auto q : e->quads) {
            entries.back().quads.push_back({ q->size,
                q->script_index, q->x, q->y, q->z, q->width, q->height });
            // std::cout << "        {" << q->size<< ", " << q->script_index
            // << ", " << q->x<< ", " << q->y<< ", " << q->z<< ", " <<
            // q->width<< ", " << q->height << "}\n";
        }
        // std::cout << "}\n";
    }

    // std::transform(std->instances.begin(), std->instances.end(),
    // faces.begin(), [](std_object_instance_t* i) -> face {
    //     return { i->object_id, i->x, i->y, i->z };
    // });
    for (auto i : std->instances)
        faces.push_back({ i->object_id, i->x, i->y, i->z });

    uint32_t offset = 0;
    for (auto i : std->instrs) {
        script.push_back({ i->time, i->type, offset, {} });
        offset += 8;
        if (i->size > sizeof(*i)) {
            uint16_t nbArgs = (i->size - sizeof(*i)) / 4;
            offset += nbArgs * 4;
            for (uint16_t j = 0; j < nbArgs; j++) {
                int32_t S = *reinterpret_cast<int32_t*>(
                    reinterpret_cast<char*>(i) + 8 + j * 4);
                script.back().args.push_back({ .S = S });
            }
        }
    }

    anm_file_name = std->header_10->anm_name;
    std_free(std);
}

StdFile::~StdFile() {
    Clear();
}

void StdFile::Init() {
    anm_file = AnmManager::LoadFile(
        GLOBALS.inner.STAGE_NUM % 2 + 3, anm_file_name);
    for (auto f : faces)
        spawnFace(f);
    std::sort(bgVms.begin(), bgVms.end(),
              [](AnmVM* const& a, AnmVM* const& b) {
                    return a->getLayer() < b->getLayer();
              });
    theCam = NSEngine::activeCamera3D();
}

void StdFile::Clear() {
    for (auto vm : bgVms)
        delete vm;
    bgVms.clear();
    anm_file->Cleanup();
}

void StdFile::Update() {
    for (auto vm : bgVms)
        if (vm)
            vm->update();
    /* CHECK FOR INTERRUPTIONS */
    if (pending_switch_label != 0) {
        // find label
        uint32_t j = 0;
        for (auto i : script) {
            if (i.type == 16 && i.args[0].S == pending_switch_label) {
                instr = j;
                time = i.time;
                stopped = false;
            }
            j++;
        }
        pending_switch_label = 0;
    }

    /* WAITING */
    if (stopped)
        return;

    /* RUN INSTRUCTIONS */
    uint32_t oldinstr = instr;
#define in script[instr]
    while (in.time <= time && !stopped) {
        execInstr(in);
        if (oldinstr != instr)
            execInstr(in);
        instr++;
        oldinstr = instr;
    }
#undef in

    time++;
}

void StdFile::Draw() {
    if (bgVms.size() == 0)
        return;
    baseShader.start();
    float savedPosx = SURF_ORIGIN_FULL_X_;
    float savedPosy = SURF_ORIGIN_FULL_Y_;
    SURF_ORIGIN_FULL_X_ = 0;
    SURF_ORIGIN_FULL_Y_ = 0;
    baseShader.SetCameraPosition(theCam->getPosition());
    glActiveTexture(GL_TEXTURE0);
    NSEngine::toggleCulling(false);
    if (NSEngine::getInstance()->flags().flags.wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    float mi = 1000000.f, ma = 1000000.f;
    glm::vec4 col = theCam->getFog(mi, ma);
    col.w = 1.0f;
    if (Inputs::Keyboard().Down(NSK_f5))
        mi = 1000000.f, ma = 1000000.f;
    sprBatch.begin();

    int l = bgVms[0]->getLayer();
    int m = bgVms[0]->getMode();
    int z = bgVms[0]->getZdis();
    for (auto vm : bgVms) {
        if (!vm)
            continue;
        if (vm->getLayer() != l || vm->getMode() != m || vm->getZdis() != z) {
            sprBatch.end();
            if (m < 4) {
                baseShader.SetProjectionMatrix(glm::mat4(1.f));
                baseShader.SetViewMatrix(theCam->getCamStatic());
            } else {
                baseShader.SetProjectionMatrix(theCam->getProjection(false));
                baseShader.SetViewMatrix(theCam->getView(false));
            }
            if (z || m < 4)
                glDisable(GL_DEPTH_TEST);
            else
                glEnable(GL_DEPTH_TEST);
            baseShader.SetFog(m < 4 ? 1000000.f : mi, m < 4
                              ? 1000000.f : ma, col);
            sprBatch.renderBatch();
            sprBatch.begin();
            l = vm->getLayer();
            m = vm->getMode();
            z = vm->getZdis();
        }
        vm->draw(&sprBatch);
    }

    sprBatch.end();
    if (z)
        glDisable(GL_DEPTH_TEST);
    else
        glEnable(GL_DEPTH_TEST);
    if (m < 4) {
        baseShader.SetProjectionMatrix(glm::mat4(1.f));
        baseShader.SetViewMatrix(theCam->getCamStatic());
    } else {
        baseShader.SetProjectionMatrix(theCam->getProjection(false));
        baseShader.SetViewMatrix(theCam->getView(false));
    }
    baseShader.SetFog(m < 4 ? 1000000.f : mi, m < 4 ? 1000000.f : ma, col);

    sprBatch.renderBatch();

    glDisable(GL_DEPTH_TEST);
    // NSEngine::toggleCulling(true);
    baseShader.stop();
    NSEngine::TextureManager::ResetTexture();
    glDisable(GL_BLEND);
    if (NSEngine::getInstance()->flags().flags.wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    SURF_ORIGIN_FULL_X_ = savedPosx;
    SURF_ORIGIN_FULL_Y_ = savedPosy;
}

void StdFile::spawnFace(StdFile::face const& f) {
    const entry& e = entries[f.entry_id];
    for (auto q : e.quads) {
        float x = f.x + q.x;
        float y = f.y + q.y;
        float z = f.z + q.z;
        auto vm = new AnmVM();
        anm_file->copyFromLoaded(vm, q.script_index);
        vm->update();
        vm->setEntityPos(x, y, z);
        vm->setLayer(e.layer);
        auto s = vm->getSprite();
        vm->setScale2(q.width / s.w, q.height / s.h);
        bgVms.push_back(vm);
    }
}

template <typename T, typename _Predicate>
size_t insertRemoveNullptr(const T& val, std::vector<T>& vec,
                           _Predicate predicate) {
    size_t i = 0;
    int nullptrcnt = 0;
    while (i < vec.size()) {
        if (vec[i] == nullptr)
            nullptrcnt++;
        while (i + nullptrcnt < vec.size() && vec[i + nullptrcnt] == nullptr)
            nullptrcnt++;
        if (i + nullptrcnt >= vec.size())
            break;
        vec[i] = vec[i + nullptrcnt];
        i++;
    }
    vec.resize(vec.size() - nullptrcnt);
    i = 0;
    size_t ret = static_cast<size_t>(-1);
    T toInsert = val;
    while (i < vec.size()) {
        if (toInsert != val || predicate(val, vec[i])) {
            if (toInsert == val)
                ret = i;
            T temp = toInsert;
            toInsert = vec[i];
            vec[i] = temp;
        }
        i++;
    }
    if (ret == static_cast<size_t>(-1))
        ret = vec.size();
    vec.push_back(toInsert);
    return ret;
}

void StdFile::execInstr(StdFile::instruction const& i) {
    // return;
    if (theCam == nullptr)
        return;
#define S(n) (i.args[n].S)
#define f(n) (i.args[n].f)
#define a(n) ((i.args[n].S >> 24) & 255)
#define r(n) ((i.args[n].S >> 16) & 255)
#define g(n) ((i.args[n].S >> 8) & 255)
#define b(n) ((i.args[n].S >> 0) & 255)
    switch (i.type) {
    case 0:  // stop
        stopped = true;
        return;
    case 1:  // jmp
        for (uint32_t j = 0; j < script.size(); j++)
            if (script[j].offset == (uint32_t)S(0))
                instr = j;
        time = S(1);
        return;
    case 2:  // pos
        theCam->setPosition(f(0), -f(1), f(2));
        return;
    case 3:  // posTime
        theCam->PositionTime(S(0), S(1), f(2), -f(3), f(4));
        return;
    case 4:  // facing
        theCam->setLook(f(0), -f(1), f(2));
        return;
    case 5:  // facingTime
        theCam->LookTime(S(0), S(1), f(2), -f(3), f(4));
        return;
    case 6:  // up
        theCam->setUp(f(0), -f(1), f(2));
        return;
    case 7:  // fov
        theCam->setFov(f(0));
        return;
    case 8:  // fog
        theCam->setFog(r(0), g(0), b(0), a(0), f(1), f(2));
        return;
    case 9:  // fogTime
        theCam->FogTime(S(0), S(1), r(2), g(2), b(2), a(2), f(3), f(4));
        return;
    case 10:  // posBezier
        theCam->PositionBezier(S(0), f(2), f(3), f(4), f(5), f(6), f(7),
                               f(8), f(9), f(10));
        return;
    case 11:  // facingBezier
        theCam->LookBezier(S(0), f(2), f(3), f(4), f(5), f(6), f(7),
                           f(8), f(9), f(10));
        return;
    case 12:  // rockingMode
        return;
    case 13:  // bgColor
        theCam->setClearColor(r(0), g(0), b(0));
        return;
    case 14: {  // sprite
        // std::cout << S(0) << ":" << S(1) << " on " << S(2) << "  ";
        if (anmSlots[S(0)] != -1) {
            delete bgVms[anmSlots[S(0)]];
            bgVms[anmSlots[S(0)]] = nullptr;
            anmSlots[S(0)] = -1;
        }
        AnmVM* vm = new AnmVM();
        anm_file->copyFromLoaded(vm, S(1));
        vm->update();
        vm->setLayer(S(2));
        std::cout << "bgVms size : " << bgVms.size() <<
                "  anmSlots[" << S(0) << "] : ";
        static const auto predicate =
                [](AnmVM* const& a, AnmVM* const& b) {
                    return a->getLayer() <= b->getLayer();
                };
        anmSlots[S(0)] = insertRemoveNullptr(vm, bgVms, predicate);
        std::cout << anmSlots[S(0)] << "\n";
        return;
    }
    case 15:  // nop
        return;
    case 16:  // interruptLabel
        return;
    case 17:  // distortion
        return;
    case 18:  // upTime
        theCam->UpTime(S(0), S(1), f(2), -f(3), f(4));
        return;
    case 19:  // interruptBG
        for (auto vm : bgVms)
            vm->interrupt(7 + S(0));
        return;
    case 20:  // drawDistance
        // unused
        return;
    case 21:  // fovTime
        theCam->FovTime(S(0), S(1), f(2));
        return;
    }
#undef S
#undef f
#undef a
#undef r
#undef g
#undef b
}

}  // namespace StdOpener
