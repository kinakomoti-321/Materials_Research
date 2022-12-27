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
    class GGX : public BSDF
    {
    private:
        vec3 _F0;
        float _alpha;

    public:
        GGX()
        {
            _F0 = vec3(0);
            _alpha = 1.0;
        }

        GGX(const vec3 &F0, const float &alpha)
        {
            _F0 = F0;
            _alpha = alpha * alpha;
        }

        float ggx_D(const vec3 &wm) const
        {
            /*
            const float tan2theta = BSDFMath::tan2Theta(wm);
            if (std::isinf(tan2theta))
                return 0;
            const float cos4theta = BSDFMath::cos2Theta(wm) * BSDFMath::cos2Theta(wm);
            const float term = 1.0f + tan2theta / (_alpha * _alpha);
            return 1.0f / ((PI * _alpha * _alpha * cos4theta) * term * term);
            */
            const float term1 = wm.y * wm.y * (wm.x * wm.x + wm.z * wm.z) / (_alpha * _alpha);
            return 1.0 / (PI * _alpha * _alpha * term1 * term1);
        }

        float lambda(const vec3 &w) const
        {
            /*
            float delta = std::max(_alpha * BSDFMath::tanTheta(w), 0.0f);
            return (-1 + std::sqrt(1.0 + delta * delta)) / 2.0f;
            */
            float term = _alpha * _alpha * (w.x * w.x + w.z * w.z) / (w.y * w.y);
            return -1.0 + std::sqrt(1.0f + term) / 2.0;
        }

        float smith_G1(const vec3 &w) const
        {
            return 1 / (lambda(w) + 1.0f);
        }

        float smith_G2(const vec3 &wo, const vec3 &wi, const vec3 &wm) const
        {
            // return G1(wo) * G1(wi);

            // Hight correlated Smith shadowing-masking;
            return 1.0 / (1.0 + lambda(wo) + lambda(wi));
        }

        vec3 sampling_m(float u, float v, float &pdf) const
        {
            float theta_m = std::atan(_alpha * std::sqrt(v) / std::sqrt(std::max(1.0f - v, 0.0f)));
            float phi_m = 2.0f * PI * u;

            vec3 wm = MR_Math::sphericalDirection(theta_m, phi_m);
            pdf = ggx_D(wm) * BSDFMath::cosTheta(wm);

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

            float D = ggx_D(wm);
            float G = smith_G2(wo, wi, wm);
            vec3 F = BSDFMath::Shlick_Fresnel(wi, wm, _F0);

            float in = BSDFMath::cosTheta(wi);
            float on = BSDFMath::cosTheta(wo);
            // return F * G / (in * on);
            vec3 bsdf = F * G * D / (4.0f * in * on);
            if (isnan(bsdf.x) || isinf(bsdf.x))
            {
                pdf = 1.0;
                return vec3(0);
            }
            return bsdf;
        };

        vec3 evaluateBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wm = normalize(wo + wi);

            float D = ggx_D(wm);
            float G = smith_G2(wo, wi, wm);
            vec3 F = BSDFMath::Shlick_Fresnel(wo, wm, _F0);

            float in = BSDFMath::cosTheta(wi);
            float on = BSDFMath::cosTheta(wo);

            return F * G * D / (4.0f * in * on);
        };
        float pdfBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wm = normalize(wo + wi);
            return ggx_D(wm) * BSDFMath::cosTheta(wm) / (4.0f * dot(wi, wm));
        };
    };

    /*
    class AnisotropicGGX : public BSDF
    {
    private:
        vec3 _F0;
        float _alpha_x;
        float _alpha_y;

    public:
        AnisotropicGGX()
        {
            _F0 = vec3(0);
            _alpha_x = 1.0;
            _alpha_y = 1.0;
        }

        AnisotropicGGX(const vec3 &F0, const float &alpha)
        {
            _F0 = F0;
            _alpha_x = alpha * alpha;
            _alpha_y = alpha * alpha;
        }

        float ggx_D(const vec3 &wm) const
        {
            float cos_theta = BSDFMath::cosTheta(wm);
            float cos_theta4 = cos_theta * cos_theta * cos_theta * cos_theta;
            float tan_theta = BSDFMath::tanTheta(wm);
            float delta1 = _alpha * _alpha + tan_theta * tan_theta;

            return _alpha * _alpha / (PI * cos_theta4 * delta1 * delta1);
        }

        float lambda(const vec3 &w) const
        {
            float tan_theta = BSDFMath::tanTheta(w);
            float delta1 = 1.0f + 1.0f / (_alpha * _alpha * tan_theta * tan_theta);
            return (-1 + std::sqrt(std::max(delta1, 0.0f))) / 2.0f;
        }

        float smith_G1(const vec3 &w) const
        {
            return 1 / (lambda(w) + 1.0f);
        }

        float smith_G2(const vec3 &wo, const vec3 &wi, const vec3 &wm) const
        {
            // return G1(wo) * G1(wi);

            // Hight correlated Smith shadowing-masking;
            return 1.0 / (1.0 + lambda(wo) + lambda(wi));
        }

        vec3 sampling_m(float u, float v, float &pdf) const
        {
            float theta_m = std::atan(_alpha * std::sqrt(u / (1.0 - u)));
            float phi_m = 2.0 * PI * v;

            vec3 wm = MR_Math::sphericalDirection(theta_m, phi_m);
            pdf = ggx_D(wm) * BSDFMath::cosTheta(wm);

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

            float D = ggx_D(wm);
            float G = smith_G2(wo, wi, wm);
            vec3 F = BSDFMath::Shlick_Fresnel(wo, wm, _F0);

            float in = BSDFMath::cosTheta(wo);
            float on = BSDFMath::cosTheta(wo);
            // return F * G / (in * on);
            vec3 bsdf = F * G * D / (4.0f * in * on);
            if (isnan(bsdf.x) || isinf(bsdf.x))
            {
                pdf = 1.0;
                return vec3(0);
            }
            return bsdf;
        };

        vec3 evaluateBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wm = normalize(wo + wi);

            float D = ggx_D(wm);
            float G = smith_G2(wo, wi, wm);
            vec3 F = BSDFMath::Shlick_Fresnel(wo, wm, _F0);

            float in = BSDFMath::cosTheta(wo);
            float on = BSDFMath::cosTheta(wo);

            return F * G * D / (4.0f * in * on);
        };
        float pdfBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wm = normalize(wo + wi);
            return ggx_D(wm) * BSDFMath::cosTheta(wm) / (4.0f * dot(wi, wm));
        };
    };
    */
}
