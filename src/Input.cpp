#include "./Input.h"
#include <InputManager.h>

InputStruct_t INPUT_STRUCT{};

void InputStruct_t::Update() {
    // right left down up X X X shoot
    input = 0;
    if (Inputs::Keyboard().Down(NSK_return)) input |= 0x80000;
    if (Inputs::Keyboard().Down(NSK_escape)) input |= 0x00100;
    if (Inputs::Keyboard().Down(NSK_right )) input |= 0x00080;
    if (Inputs::Keyboard().Down(NSK_left  )) input |= 0x00040;
    if (Inputs::Keyboard().Down(NSK_down  )) input |= 0x00020;
    if (Inputs::Keyboard().Down(NSK_up    )) input |= 0x00010;
    if (Inputs::Keyboard().Down(NSK_lshift)) input |= 0x00008;
    // 0x00004
    if (Inputs::Keyboard().Down(NSK_x     )) input |= 0x00002;
    if (Inputs::Keyboard().Down(NSK_w     )) input |= 0x00001;
}
