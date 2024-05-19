#ifndef ANMVIEWER_INCLUDED_H
#define ANMVIEWER_INCLUDED_H

#include <vector>
#include "./AnmOpener/AnmVM.h"

struct AnmView {
    int anmId;
    bool ok = true;
    bool windowOpen = false;
    bool followPlayer = false;
    int pauseInstr = -1;
    int parentId = 0;
    bool spriteShowOpen = false;
    anm::VM* vm = nullptr;

    void renderInList();
};

class AnmViewer {
public:
    AnmViewer();
    ~AnmViewer();

    void on_tick();
    void on_draw();

    std::vector<AnmView> &views() { return m_anmviews; }
    void anim(int id) { m_anmviews.push_back({id, true, false, false, -1, 0, false}); }
    void animP(int id, int pid) { m_anmviews.push_back({id, true, false, false, -1, pid, false}); }
    void animPtr(anm::VM* vm) {
        m_anmviews.push_back({0, true, false, false, -1, 0, false, vm});
    }
private:
    void check_anmviews();

    std::vector<AnmView> m_anmviews;
};

extern AnmViewer *ANM_VIEWER_PTR;

#endif // !ANMVIEWER_INCLUDED_H
