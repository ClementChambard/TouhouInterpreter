#ifndef LASERCURVE_H_
#define LASERCURVE_H_

#include "Laser.h"
#include <AnmOpener/AnmVM.h>

struct LaserCurveInner_t {

};

class LaserCurve : public Laser {

    public:
        LaserCurve();
        ~LaserCurve() override;

        LaserCurveInner_t& getInner() { return inner; }

    private:
        LaserCurveInner_t inner;
        NSEngine::AnmVM vm1, vm2;
        // undefined4 x17
};

#endif // LASERCURVE_H_
