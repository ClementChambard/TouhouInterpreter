#ifndef INPUT_H_
#define INPUT_H_

#include <cstdint>

struct InputStruct_t {
    uint8_t input = 0;

    void Update();
};

extern InputStruct_t INPUT_STRUCT;

#endif // INPUT_H_
