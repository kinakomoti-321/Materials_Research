#pragma once
#include <glm/glm.hpp>
#include <cmath>
#include <algorithm>

using namespace glm;

namespace BSDFMath
{
    inline float cosTheta(const vec3 &w) { return w.y; }
    inline float cos2Theta(const vec3 &w) { return w.y * w.y; }
    inline float sinTheta(const vec3 &w) { return std::sqrt(std::max(1.0f - cosTheta(w) * cosTheta(w), 0.0f)); }
    inline float tanTheta(const vec3 &w) { return sinTheta(w) / cosTheta(w); }
    inline float tan2Theta(const vec3 &w) { return tanTheta(w) * tanTheta(w); }

    inline float cosPhi(const vec3 &w)
    {
        float sintheta = sinTheta(w);
        if (sintheta == 0)
            return 1.0f;
        return std::clamp(w[0] / sintheta, -1.0f, 1.0f);
    }
    inline float sinPhi(const vec3 &w)
    {
        float sintheta = sinTheta(w);
        if (sintheta == 0)
            return 0.0f;
        return std::clamp(w[2] / sintheta, -1.0f, 1.0f);
    }

    inline float cosPhi2(const vec3 &w) { return cosPhi(w) * cosPhi(w); }
    inline float sinPhi2(const vec3 &w) { return sinPhi(w) * sinPhi(w); }
}