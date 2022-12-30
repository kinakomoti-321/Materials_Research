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
            const float tan2theta = BSDFMath::tan2Theta(wm);
            if (std::isinf(tan2theta))
                return 0;
            const float cos4theta = BSDFMath::cos2Theta(wm) * BSDFMath::cos2Theta(wm);
            const float term = 1.0f + tan2theta / (_alpha * _alpha);
            return 1.0f / ((PI * _alpha * _alpha * cos4theta) * term * term);

            /*
            const float term1 = wm.y * wm.y * (wm.x * wm.x + wm.z * wm.z) / (_alpha * _alpha);
            return 1.0 / (PI * _alpha * _alpha * term1 * term1);
            */
        }

        float lambda(const vec3 &w) const
        {
            /*
            float abstan = BSDFMath::absTanTheta(w);
            if (std::isinf(abstan))
                return 0;

            float delta = _alpha * abstan;
            return (-1.0f + std::sqrt(1.0f + delta * delta)) / 2.0f;
            */

            float term = _alpha * _alpha * (w.x * w.x + w.z * w.z) / (w.y * w.y);
            return (-1.0 + std::sqrt(1.0f + term)) / 2.0;
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

        AnisotropicGGX(const vec3 &F0, const float &alpha_x, const float &alpha_y)
        {
            _F0 = F0;
            _alpha_x = alpha_x * alpha_x;
            _alpha_y = alpha_y * alpha_y;
        }

        float ggx_D(const vec3 &wm) const
        {
            float term = wm.x * wm.x / (_alpha_x * _alpha_x) + wm.z * wm.z / (_alpha_y * _alpha_y) + wm.y * wm.y;
            return 1.0f / (PI * _alpha_x * _alpha_y * term * term);
        }

        float lambda(const vec3 &w) const
        {
            float term = (w.x * w.x * _alpha_x * _alpha_x + w.z * w.z * _alpha_y * _alpha_y) / (w.y * w.y);
            return (-1.0f + std::sqrt(1.0f + term)) / 2.0f;
        }

        float smith_G1(const vec3 &w) const
        {
            return 1.0f / (lambda(w) + 1.0f);
        }

        float smith_G2(const vec3 &wo, const vec3 &wi, const vec3 &wm) const
        {
            return smith_G1(wo) * smith_G1(wi);
            // Hight correlated Smith shadowing-masking;
            // return 1.0 / (1.0 + lambda(wo) + lambda(wi));
        }

        vec3 sampling_m(float u, float v, float &pdf) const
        {
            float tanTerm = std::tan(2.0f * PI * u) * _alpha_x / _alpha_y;
            float phi = std::atan(tanTerm);
            if (u >= 0.75)
            {
                phi += 2.0f * PI;
            }
            else if (u > 0.25)
            {
                phi += PI;
            }

            const float cosphi = std::cos(phi);
            const float sinphi = std::sin(phi);

            const float term_alpha = cosphi * cosphi / (_alpha_x * _alpha_x) + sinphi * sinphi / (_alpha_y * _alpha_y);
            const float term = v / ((1.0f - v) * term_alpha);

            float theta = std::atan(std::sqrt(term));

            const vec3 wh = MR_Math::sphericalDirection(theta, phi);
            pdf = ggx_D(wh) * BSDFMath::cosTheta(wh);

            return wh;
        }

        vec3 visibleNormalSampling(const vec3 &V_, float u, float v, float &pdf) const
        {
            vec3 Vh = normalize(vec3(_alpha_x * V_.x, V_.y, _alpha_y * V_.z));

            vec3 n = vec3(0, 1, 0);
            if (Vh.y > 0.99)
                n = vec3(0, 0, -1);

            vec3 T1 = normalize(cross(Vh, n));
            vec3 T2 = cross(T1, Vh);

            float r = std::sqrt(u);
            float phi = 2.0 * PI * v;
            float t1 = r * std::cos(phi);
            float t2 = r * std::sin(phi);
            float s = 0.5 * (1.0 + Vh.y);
            t2 = (1.0 - s) * sqrt(1.0 - t1 * t1) + s * t2;

            vec3 Nh = t1 * T1 + t2 * T2 + sqrt(max(0.0f, 1.0f - t1 * t1 - t2 * t2)) * Vh;

            vec3 Ne = normalize(vec3(_alpha_x * Nh.x, Nh.y, _alpha_y * Nh.z));

            return Ne;
        }

        vec3 sampleBSDF(const vec3 &wo, vec3 &wi, float &pdf, std::shared_ptr<MR::Sampler> &sampler) const override
        {
            // vec3 wm = sampling_m(sampler->getSample(), sampler->getSample(), pdf);
            vec3 wm = visibleNormalSampling(wo, sampler->getSample(), sampler->getSample(), pdf);
            wi = MR_Math::reflect(wo, wm);

            if (wi.y < 0.0)
            {
                pdf = 1.0;
                return vec3(0);
            }

            float D = ggx_D(wm);
            float G = smith_G2(wo, wi, wm);
            vec3 F = BSDFMath::Shlick_Fresnel(wo, wm, _F0);

            float in = BSDFMath::cosTheta(wi);
            float on = BSDFMath::cosTheta(wo);

            // VisibleNormal
            pdf = D * smith_G1(wo) * std::max(dot(wo, wm), 0.0f) / (on * 4.0f * dot(wm, wi));

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
}
