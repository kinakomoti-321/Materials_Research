#pragma once
#include <glm/glm.hpp>
#include <Material/bsdf.hpp>
#include <Sampler/sampler.hpp>
#include <Core/constant.hpp>
#include <Math/vecop.hpp>
#include <Math/bsdf_math.hpp>

using namespace glm;

namespace MR_BSDF
{
    class Lambert : public BSDF
    {
    private:
        vec3 _rho;

        // コサイン重点的サンプリング
        vec3 cosineSampling(const float u, const float v, float &pdf) const
        {
            const float theta = 0.5f * std::acos(1.0f - 2.0f * u);
            const float phi = 2.0f * PI * v;
            pdf = invPI * std::cos(theta);
            return MR_Math::sphericalDirection(theta, phi);
        }

    public:
        Lambert() { _rho = vec3(0); }
        Lambert(const vec3 rho) : _rho(rho) {}

        vec3 sampleBSDF(const vec3 &wo, vec3 &wi, float &pdf, std::shared_ptr<MR::Sampler> &sampler) const override
        {
            wi = cosineSampling(sampler->getSample(), sampler->getSample(), pdf);
            return _rho * invPI;
        };
        vec3 evaluateBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            return _rho * invPI;
        };
        float psdBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            return invPI * BSDFMath::cosTheta(wi);
        };
    };
}