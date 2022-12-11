#include "Laser.h"

Laser::~Laser()
{
    next->prev = this->prev;
    if (this->prev) prev->next = next;
}
