#ifndef INTERP_H
#define INTERP_H


#include <cinttypes>
#include <Interpolator_impl.h>
#include <glm/glm.hpp>

template<typename T>
struct Interp {
    T initial = T{};
    T goal = T{};
    T bezier_1 = T{};
    T bezier_2 = T{};
    T current = T{};
    int32_t time = 0;
    int32_t end_time = 0;
    int32_t method = 0;

    void start(T begin, T end, int32_t time, int32_t mode) {
        initial = begin;
        current = initial;
        goal = end;
        bezier_1 = {};
        bezier_2 = {};
        end_time = time;
        method = mode;
        this->time = 0;
    }

    void start_ex(T begin, T end, T b1, T b2, int32_t time, int32_t mode) {
        initial = begin;
        current = initial;
        goal = end;
        bezier_1 = b1;
        bezier_2 = b2;
        end_time = time;
        method = mode;
        this->time = 0;
    }

    void start_bezier(T begin, T end, T b1, T b2, int32_t time) {
        initial = begin;
        current = initial;
        goal = end;
        bezier_1 = b1;
        bezier_2 = b2;
        end_time = time;
        method = 8;
        this->time = 0;
    }

    T step() {
        if (end_time != 0) {
            time++;
            if (time < end_time || end_time < 0) {
                if (method == 7) {
                    initial += goal;
                    current = initial;
                } else if (method == 17) {
                    initial += bezier_2;
                    bezier_2 += goal;
                    current = initial;
                } else if (method == 8) {
                    float x = static_cast<float>(time) /
                              static_cast<float>(end_time);
                    T a0 = initial;
                    T a1 = bezier_1;
                    T a2 = 3.f * (goal - initial) - 2.f * bezier_1 - bezier_2;
                    T a3 = 2.f * (initial - goal) + bezier_1 + bezier_2;
                    current = a0 + a1 * x + a2 * x * x + a3 * x * x * x;
                } else {
                    current = (goal - initial) * NSEngine::FuncGetVal(method)(
                        static_cast<float>(time) / static_cast<float>(end_time))
                        + initial;
                }
                return current;
            }
            time = end_time;
            end_time = 0;
        }
        if (method != 7 && method != 17) {
            return goal;
        }
        return initial;
    }
};

struct InterpStrange {
    glm::vec3 current;
    glm::vec3 initial;
    glm::vec3 goal;
    glm::vec3 bezier_1;
    glm::vec3 bezier_2;
    int32_t time;
    int32_t end_time;
    int32_t method_for_1d[3];
    int32_t method_for_3d;
    int32_t flags;

    void start(glm::vec3 begin, glm::vec3 end, int32_t time, int32_t mode) {
        initial = begin;
        goal = end;
        bezier_1 = {};
        bezier_2 = {};
        end_time = time;
        method_for_3d = mode;
        this->time = 0;
        flags = 0;
    }

    void start_curve(glm::vec3 begin, glm::vec3 end, int32_t time,
                     int32_t mode_x, int32_t mode_y, int32_t mode_z) {
        initial = begin;
        goal = end;
        bezier_1 = {};
        bezier_2 = {};
        end_time = time;
        method_for_1d[0] = mode_x;
        method_for_1d[1] = mode_y;
        method_for_1d[2] = mode_z;
        this->time = 0;
        flags = 1;
    }

    void start_bezier(glm::vec3 begin, glm::vec3 end, glm::vec3 b1,
                      glm::vec3 b2, int32_t time) {
        initial = begin;
        goal = end;
        bezier_1 = b1;
        bezier_2 = b2;
        end_time = time;
        method_for_3d = 8;
        this->time = 0;
        flags = 0;
    }

    glm::vec3 step() {
        if (end_time != 0) {
            if (end_time < 0 || ++time < end_time) {
                if (flags == 0) {
                    if (method_for_3d == 7) {
                        initial += goal;
                        current = initial;
                    } else if (method_for_3d == 17) {
                        initial += bezier_2;
                        bezier_2 += goal;
                        current = initial;
                    } else if (method_for_3d == 8) {
                        float x = static_cast<float>(time) /
                                  static_cast<float>(end_time);
                        glm::vec3 a0 = initial;
                        glm::vec3 a1 = bezier_1;
                        glm::vec3 a2 = 3.f * (goal - initial) - 2.f *
                                       bezier_1 - bezier_2;
                        glm::vec3 a3 = 2.f * (initial - goal)
                                     + bezier_1 + bezier_2;
                        current = a0 + a1 * x + a2 * x * x + a3 * x * x * x;
                    } else {
                        current = (goal - initial) *
                            NSEngine::FuncGetVal(method_for_3d)(
                                static_cast<float>(time) /
                                static_cast<float>(end_time)) + initial;
                    }
                } else {
                    for (int i = 0; i < 3; i++) {
                        if (method_for_1d[i] == 7) {
                            initial[i] += goal[i];
                            current[i] = initial[i];
                        } else if (method_for_1d[i] == 17) {
                            initial[i] += bezier_2[i];
                            bezier_2[i] += goal[i];
                            current[i] = initial[i];
                        } else if (method_for_1d[i] == 8) {
                            float x = static_cast<float>(time) /
                                      static_cast<float>(end_time);
                            float a0 = initial[i];
                            float a2 = 3.f * (goal[i] - initial[i]);
                            float a3 = 2.f * (initial[i] - goal[i]);
                            current[i] = a0 + a2 * x * x + a3 * x * x * x;
                        } else {
                            current[i] = (goal[i] - initial[i]) *
                                NSEngine::FuncGetVal(method_for_1d[i])(
                                static_cast<float>(time) /
                                static_cast<float>(end_time)) + initial[i];
                        }
                    }
                }
                return current;
            }
            time = end_time;
            end_time = 0;
        }
        if (method_for_3d != 7 && method_for_3d != 17) {
            return goal;
        }
        return initial;
    }
};
#endif
