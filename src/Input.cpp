#include "./Input.h"
#include <input.hpp>

InputStruct_t INPUT_STRUCT{};

void InputStruct_t::Update() {
    // right left down up X X X shoot
    input = 0;
    if (ns::keyboard::down(ns::Key::RETURN)) input |= 0x80000;
    if (ns::keyboard::down(ns::Key::ESCAPE)) input |= 0x00100;
    if (ns::keyboard::down(ns::Key::RIGHT )) input |= 0x00080;
    if (ns::keyboard::down(ns::Key::LEFT  )) input |= 0x00040;
    if (ns::keyboard::down(ns::Key::DOWN  )) input |= 0x00020;
    if (ns::keyboard::down(ns::Key::UP    )) input |= 0x00010;
    if (ns::keyboard::down(ns::Key::LSHIFT)) input |= 0x00008;
    // 0x00004
    if (ns::keyboard::down(ns::Key::X     )) input |= 0x00002;
    if (ns::keyboard::down(ns::Key::W     )) input |= 0x00001;
}
