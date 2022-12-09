#include "Laser.h"

int Laser::method_18()
{
    next->prev = this->prev;
    if (this->prev) prev->next = next;
    return 0;
}
