#include <NSEngine.hpp>

#define WINDOW_WIDTH  (/*1920*/1280/*640*/)
#define WINDOW_HEIGHT (/*1080*/960 /*480*/)

class App : public ns::IEngine {
public:
    App(int argc, char** argv)
        : IEngine(WINDOW_WIDTH, WINDOW_HEIGHT, "ecl viewer")
        , m_argc(argc)
        , m_argv(argv) {}
    ~App() override {}

    void on_create() override;
    void on_update() override;
    void on_render() override;
    void on_destroy() override;

private:
    int m_argc = 0;
    char** m_argv = nullptr;
};
