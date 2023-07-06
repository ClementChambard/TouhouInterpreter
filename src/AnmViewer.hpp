#ifndef ANMVIEWER_INCLUDED_H
#define ANMVIEWER_INCLUDED_H

#include <string>
#include <vector>

struct AnmView {
  int anmId;
  bool ok = true;
  bool windowOpen = false;
  bool followPlayer = false;
  int pauseInstr = -1;

  void renderInList();
};

class AnmViewer {
public:
  AnmViewer();
  ~AnmViewer();

  void on_tick();
  void on_draw();

  std::vector<AnmView> &views() { return m_anmviews; }
  void anim(int id) { m_anmviews.push_back({id, true, false, false, -1}); }

private:
  void check_anmviews();

  std::vector<AnmView> m_anmviews;
};

extern AnmViewer *ANM_VIEWER_PTR;

#endif // !ANMVIEWER_INCLUDED_H
