#pragma once
#include <glm/glm.hpp>

using namespace glm;
namespace MR
{
    class BSDF
    {
    public:
        virtual vec3 sampleBSDF(const vec3 &wo, vec3 &wi, float &pdf) const = 0;
        virtual vec3 evaluateBSDF(const vec3 &wo, const vec3 &wi) const = 0;
        virtual float psdBSDF(const vec3 &wo, const vec3 &wi) const = 0;
    };
}