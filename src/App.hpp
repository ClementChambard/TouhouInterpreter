#include <Engine.hpp>
#include "EnemyManager.h"
#include "BulletManager.h"
#include "Laser/LaserManager.h"

#define WINDOW_WIDTH (384*2/*1920*/)
#define WINDOW_HEIGHT (448*2/*1080*/)

class App : public NSEngine::IEngine {

public:
    App(int argc, char** argv) : IEngine(WINDOW_WIDTH, WINDOW_HEIGHT, "ecl viewer"), m_argc(argc), m_argv(argv) {}
    ~App() override {}

    void on_create() override;
    void on_update() override;
    void on_render() override;
    void on_destroy() override;

private:

    int m_argc;
    char** m_argv;

    EnemyManager* em;
    BulletManager* bm;
    LaserManager lm;

};
