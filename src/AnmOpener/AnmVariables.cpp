#include "AnmVM.h"
#include <NSEngine.h>
#include <math/Random.h>

int& AnmVM::check_ref(int i)
{
    if      (i >= 10000 && i < 10004) return int_script_vars[static_cast<size_t>(i-10000)];
    else if (i == 10008) return __script_var_8;
    else if (i == 10009) return __script_var_9;
    //else if (i == 10029) return 65535;
    else return rand_param_int;
}

float& AnmVM::check_ref(float f)
{
    if      (f >= 10004 && f < 10008) return float_script_vars[static_cast<size_t>(f-10004)];
    else if (f == 10013.f) return pos.x;
    else if (f == 10014.f) return pos.y;
    else if (f == 10015.f) return pos.z;
    else if (f == 10023.f) return rotation.x;
    else if (f == 10024.f) return rotation.y;
    else if (f == 10025.f) return rotation.z;
    else if (f == 10027.f) return rand_param_one;
    else if (f == 10028.f) return rand_param_pi;
    else if (f == 10033.f) return __script_vars_33_34_35.x;
    else if (f == 10034.f) return __script_vars_33_34_35.y;
    else if (f == 10035.f) return __script_vars_33_34_35.z;
    else return float_script_vars[3];
}

int AnmVM::check_val(int i)
{
    if      (i >= 10000 && i < 10004) return int_script_vars[static_cast<size_t>(i-10000)];
    else if (i == 10008) return __script_var_8;
    else if (i == 10009) return __script_var_9;
    else if (i == 10022) return static_cast<int>(Random::Float01() * 65535);
    else if (i == 10029) return 65535;
    else return i;
}

float AnmVM::check_val(float f)
{
    if      (f >= 10004.f && f <  10008.f) return float_script_vars[static_cast<size_t>(f-10004.f)];
    else if (f == 10010.f || f == 10030.f) return Random::Floatm11() * rand_param_pi;
    else if (f == 10011.f || f == 10031.f) return Random::Float01() * rand_param_one;
    else if (f == 10012.f || f == 10032.f) return Random::Floatm11() * rand_param_one;
    else if (f == 10013.f) return pos.x;
    else if (f == 10014.f) return pos.y;
    else if (f == 10015.f) return pos.z;
    else if (f == 10016.f) return (NSEngine::activeCamera3D() ? NSEngine::activeCamera3D()->getPosition().x : 0.f);
    else if (f == 10017.f) return (NSEngine::activeCamera3D() ? NSEngine::activeCamera3D()->getPosition().y : 0.f);
    else if (f == 10018.f) return (NSEngine::activeCamera3D() ? NSEngine::activeCamera3D()->getPosition().z : 0.f);
    else if (f == 10019.f) return (NSEngine::activeCamera3D() ? NSEngine::activeCamera3D()->getLook().x : 0.f);
    else if (f == 10020.f) return (NSEngine::activeCamera3D() ? NSEngine::activeCamera3D()->getLook().y : 0.f);
    else if (f == 10021.f) return (NSEngine::activeCamera3D() ? NSEngine::activeCamera3D()->getLook().z : 1.f);
    else if (f == 10023.f) return rotation.x;
    else if (f == 10024.f) return rotation.y;
    else if (f == 10025.f) return rotation.z;
    else if (f == 10026.f) return rotation.z + (parent_vm ? parent_vm->rotation.z : 0);
    else if (f == 10027.f) return rand_param_one;
    else if (f == 10028.f) return rand_param_pi;
    else if (f == 10033.f) return __script_vars_33_34_35.x;
    else if (f == 10034.f) return __script_vars_33_34_35.y;
    else if (f == 10035.f) return __script_vars_33_34_35.z;
    else return f;
}
