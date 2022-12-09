#ifndef LASERINFINITE_H_
#define LASERINFINITE_H_

#include "Laser.h"
#include <AnmOpener/AnmVM.h>

struct LaserInfiniteInner_t {

};

class LaserInfinite : public Laser {

    public:
        LaserInfinite();
        ~LaserInfinite() override;

        LaserInfiniteInner_t& getInner() { return inner; }

    private:
        LaserInfiniteInner_t inner;
        int32_t __field_94c;
        NSEngine::AnmVM vm1, vm2;

};

#endif // LASERINFINITE_H_
