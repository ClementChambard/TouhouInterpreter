#include "./App.hpp"

int main(int argc, char** argv) {
    if (argc == 1) return 1;
    App(argc, argv).run();
    return 0;
}
