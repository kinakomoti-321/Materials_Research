#pragma once
#include <glm/glm.hpp>
#include <Material/bsdf.hpp>

using namespace glm;

namespace MR_BSDF
{
    class Lambert : public BSDF
    {
    private:
        vec3 _rho;

    public:
        Lambert() { _rho = vec3(0); }
        Lambert(const vec3 rho) : _rho(rho) {}

        vec3 sampleBSDF(const vec3 &wo, vec3 &wi, float &pdf) const override
        {
            return vec3(0);
        };
        vec3 evaluateBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            return vec3(0);
        };
        float psdBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            return 0;
        };
    };
}