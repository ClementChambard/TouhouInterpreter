#include "./AnmVM.h"
#include "./AnmManager.h"

#include <math/Random.h>

namespace anm {

int& VM::check_ref(int i) {
    if      (i >= 10000 && i < 10004) return int_script_vars[static_cast<size_t>(i-10000)];
    else if (i == 10008) return __script_var_8;
    else if (i == 10009) return __script_var_9;
    else if (i == 10029) return rand_param_int;
    else return int_script_vars[3];
}

float& VM::check_ref(float f) {
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

int VM::check_val(int i) {
    if      (i >= 10000 && i < 10004) return int_script_vars[static_cast<size_t>(i-10000)];
    else if (i >= 10004 && i < 10008) return float_script_vars[static_cast<size_t>(i-10004)];
    else if (i == 10008) return __script_var_8;
    else if (i == 10009) return __script_var_9;
    else if (i == 10022) return static_cast<int>(Random::Float01() * rand_param_int);
    else if (i == 10027) return rand_param_one;
    else if (i == 10028) return rand_param_pi;
    else if (i == 10029) return rand_param_int;
    else return i;
}

float VM::check_val(float f) {
    if      (f >= 10000.f && f <  10004.f) return int_script_vars[static_cast<size_t>(f-10000.f)];
    else if (f >= 10004.f && f <  10008.f) return float_script_vars[static_cast<size_t>(f-10004.f)];
    else if (f == 10033.f) return __script_vars_33_34_35.x;
    else if (f == 10034.f) return __script_vars_33_34_35.y;
    else if (f == 10035.f) return __script_vars_33_34_35.z;
    else if (f == 10008.f) return __script_var_8;
    else if (f == 10009.f) return __script_var_9;
    else if (f == 10010.f || f == 10030.f) return Random::Floatm11() * rand_param_pi;
    else if (f == 10011.f || f == 10031.f) return Random::Float01() * rand_param_one;
    else if (f == 10012.f || f == 10032.f) return Random::Floatm11() * rand_param_one;
    else if (f == 10013.f) return pos.x;
    else if (f == 10014.f) return pos.y;
    else if (f == 10015.f) return pos.z;
    else if (f == 10016.f) return anm::get_3d_camera()->position.x + anm::get_3d_camera()->__rocking_vector_1.x;
    else if (f == 10017.f) return anm::get_3d_camera()->position.y + anm::get_3d_camera()->__rocking_vector_1.y;
    else if (f == 10018.f) return anm::get_3d_camera()->position.z + anm::get_3d_camera()->__rocking_vector_1.z;
    else if (f == 10019.f) return anm::get_3d_camera()->facing_normalized.x;
    else if (f == 10020.f) return anm::get_3d_camera()->facing_normalized.y;
    else if (f == 10021.f) return anm::get_3d_camera()->facing_normalized.z;
    else if (f == 10022.f) return static_cast<int>(Random::Float01() * rand_param_int);
    else if (f == 10023.f) return rotation.x;
    else if (f == 10024.f) return rotation.y;
    else if (f == 10025.f) return rotation.z;
    else if (f == 10026.f) return getRotation().z;
    else if (f == 10027.f) return rand_param_one;
    else if (f == 10028.f) return rand_param_pi;
    else if (f == 10029.f) return rand_param_int;
    else return f;
}

} // namespace anm
