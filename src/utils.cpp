#include "utils.h"

float positiveFmod(float val, float modulo) {
    float ans = std::fmod(val, modulo);
    if (ans < 0.0f)
        ans += modulo;
    return ans;
}
