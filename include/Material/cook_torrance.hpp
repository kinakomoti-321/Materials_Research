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
    class CookTorrance : public BSDF
    {
    private:
        vec3 _F0;
        float _alpha;

    public:
        CookTorrance()
        {
            _F0 = vec3(0);
            _alpha = 1.0;
        }

        CookTorrance(const vec3 &F0, const float &alpha)
        {
            _F0 = F0;
            _alpha = alpha * alpha;
        }

        float beckmann_D(const vec3 &wm) const
        {
            float cos_m = BSDFMath::cosTheta(wm);
            float tan_m = BSDFMath::tanTheta(wm);
            float fac_1 = 1.0 / (PI * _alpha * _alpha * cos_m * cos_m * cos_m * cos_m);
            float fac_2 = std::exp(-tan_m * tan_m / (_alpha * _alpha));

            return fac_1 * fac_2;
        }

        float vcavity_G(const vec3 &wo, const vec3 &wi, const vec3 &wm) const
        {
            float wi_g = 2.0f * wi.y * wm.y / dot(wi, wm);
            float wo_g = 2.0f * wo.y * wm.y / dot(wo, wm);

            return std::min(1.0f, std::max(wi_g, wo_g));
        }

        vec3 sampling_m(const float u, const float v, float &pdf) const
        {
            float theta_m = std::atan(std::sqrt(std::max(-_alpha * _alpha * std::log(1.0f - u), 0.0f)));
            float phi_m = 2 * PI * v;

            vec3 wm = MR_Math::sphericalDirection(theta_m, phi_m);
            pdf = beckmann_D(wm) * wm.y;
            return wm;
        }

        vec3 sampleBSDF(const vec3 &wo, vec3 &wi, float &pdf, std::shared_ptr<MR::Sampler> &sampler) const override
        {
            vec3 wm = sampling_m(sampler->getSample(), sampler->getSample(), pdf);
            wi = MR_Math::reflect(wo, wm);
            pdf /= 4.0f * dot(wi, wm);

            if (wi.y < 0.0)
            {
                pdf = 1.0;
                return vec3(0);
            }

            float D = beckmann_D(wm);
            float G = vcavity_G(wo, wi, wm);
            vec3 F = BSDFMath::Shlick_Fresnel(wo, wm, _F0);

            float in = BSDFMath::cosTheta(wo);
            float on = BSDFMath::cosTheta(wo);
            // return F * G / (in * on);
            return F * G * D / (4.0f * in * on);
        };

        vec3 evaluateBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wm = normalize(wo + wi);

            float D = beckmann_D(wm);
            float G = vcavity_G(wo, wi, wm);
            vec3 F = BSDFMath::Shlick_Fresnel(wo, wm, _F0);

            float in = BSDFMath::cosTheta(wo);
            float on = BSDFMath::cosTheta(wo);

            return F * G * D / (4.0f * in * on);
        };
        float pdfBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wm = normalize(wo + wi);
            return beckmann_D(wm) * BSDFMath::cosTheta(wm) / (4.0f * dot(wi, wm));
        };
    };
}
