#include "Laser.h"

Laser::~Laser()
{
    if (next) next->prev = prev;
    if (prev) prev->next = next;
}
