#pragma once
#include <glm/glm.hpp>
#include <cmath>

using namespace glm;
namespace MR
{
    struct Ray
    {
        vec3 origin;
        vec3 direction;
        float tmin = 1e-3;
        float tmax = 1e5;

        Ray();
        Ray(const vec3 origin, const vec3 direction) : origin(origin), direction(direction) {}

        vec3 operator()(float t) const
        {
            return origin + t * direction;
        }
    };
}