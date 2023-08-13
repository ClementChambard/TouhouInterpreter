#include "./Input.h"
#include <InputManager.h>

InputStruct_t INPUT_STRUCT{};

void InputStruct_t::Update() {
    // right left down up X X X shoot
    input = 0;
    if (Inputs::Keyboard().Down(NSK_right )) input |= 0x80;
    if (Inputs::Keyboard().Down(NSK_left  )) input |= 0x40;
    if (Inputs::Keyboard().Down(NSK_down  )) input |= 0x20;
    if (Inputs::Keyboard().Down(NSK_up    )) input |= 0x10;
    if (Inputs::Keyboard().Down(NSK_lshift)) input |= 0x08;
    if (Inputs::Keyboard().Down(NSK_x     )) input |= 0x02;
    if (Inputs::Keyboard().Down(NSK_w     )) input |= 0x01;
}
