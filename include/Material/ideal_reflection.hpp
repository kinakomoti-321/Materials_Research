#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <Material/bsdf.hpp>
#include <Sampler/sampler.hpp>
#include <Core/constant.hpp>
#include <Math/vecop.hpp>
#include <Math/bsdf_math.hpp>
#include <Core/constant.hpp>
#include <Core/logger.hpp>

using namespace glm;

namespace MR_BSDF
{
    class IdealReflection : public BSDF
    {
    private:
        vec3 _F0;

    public:
        IdealReflection()
        {
            _F0 = vec3(0);
        }
        IdealReflection(const vec3 &F0)
        {
            _F0 = F0;
        }

        vec3 sampleBSDF(const vec3 &wo, vec3 &wi, float &pdf, std::shared_ptr<MR::Sampler> &sampler) const override
        {
            vec3 wr = MR_Math::reflect(wo, vec3(0, 1, 0));
            wi = wr;

            pdf = 1.0;

            return BSDFMath::Shlick_Fresnel(wo, vec3(0, 1, 0), _F0) / wi.y;
        };

        vec3 evaluateBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            return vec3(0);
        };
        float pdfBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            return 0;
        };
    };
}
