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
    struct Disney_Param
    {
        vec3 basecolor = vec3(0);
        float subsurface = 0;
        float metallic = 0;
        float roughness = 0;
        float anisotropic = 0;
        float specular = 0;
        float speculartint = 0;
        float sheen = 0;
        float sheentint = 0;
        float clearcoat = 0;
        float clearcoatGloss = 0;
    };

    class DisneyBRDF : public BSDF
    {
    private:
        Disney_Param _param;

        float alpha_x;
        float alpha_y;
        float alpha_c;

        float Y(const vec3 &rgb) const
        {
            return 0.212639 * rgb.x + 0.715169 * rgb.y + 0.072192 * rgb.z;
        }

        float FD90(const vec3 &w) const
        {
            return 0.5 + 2.0 * _param.roughness * BSDFMath::cos2Theta(w);
        }

        float FSS90(const vec3 &w) const
        {
            return _param.roughness * BSDFMath::cosTheta(w);
        }

        float GTR1_D(const vec3 &wh, const float alpha) const
        {
            float term1 = alpha * alpha - 1.0;
            float cosm2 = BSDFMath::cos2Theta(wh);

            return term1 / (PI * std::log(alpha * alpha) * (1.0f + term1 * cosm2));
        }

        float ggx_D(const vec3 &wm, float _alpha_x, float _alpha_y) const
        {
            float term = wm.x * wm.x / (_alpha_x * _alpha_x) + wm.z * wm.z / (_alpha_y * _alpha_y) + wm.y * wm.y;
            return 1.0f / (PI * _alpha_x * _alpha_y * term * term);
        }

        float lambda(const vec3 &w, float _alpha_x, float _alpha_y) const
        {
            float term = (w.x * w.x * _alpha_x * _alpha_x + w.z * w.z * _alpha_y * _alpha_y) / (w.y * w.y);
            return (-1.0f + std::sqrt(1.0f + term)) / 2.0f;
        }

        float smith_G1(const vec3 &w, float _alpha_x, float _alpha_y) const
        {
            return 1.0f / (lambda(w, _alpha_x, _alpha_y) + 1.0f);
        }

        float smith_G2(const vec3 &wo, const vec3 &wi, const vec3 &wm, float _alpha_x, float _alpha_y) const
        {
            // return smith_G1(wo) * smith_G1(wi);
            //  Hight correlated Smith shadowing-masking;
            return 1.0 / (1.0 + lambda(wo, _alpha_x, _alpha_y) + lambda(wi, _alpha_x, _alpha_y));
        }

        vec3 specularHalfSampling(float u, float v, float _alpha_x, float _alpha_y) const
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

            return wh;
        }

        vec3 clearcoatHalfSampling(float u, float v, float alpha) const
        {
            // GTR1 Importance sampling
            float theta_m = std::sqrt((1.0f - std::pow(alpha * alpha, 1.0f - u)) / (1.0f - alpha * alpha));
            float phi_m = 2.0f * PI * v;
            return MR_Math::sphericalDirection(theta_m, phi_m);
        }

        vec3 diffuseSampling(float u, float v) const
        {
            // Cosine Importance Sampling
            const float theta = 0.5f * std::acos(1.0f - 2.0f * u);
            const float phi = 2.0f * PI * v;
            return MR_Math::sphericalDirection(theta, phi);
        }

    public:
        DisneyBRDF()
        {
            alpha_x = 0;
            alpha_y = 0;
            alpha_c = 0;
        };
        DisneyBRDF(const Disney_Param &param) : _param(param)
        {
            float aspect = std::sqrt(1.0 - 0.9 * _param.anisotropic);
            alpha_x = std::max(0.001f, _param.roughness * _param.roughness / aspect);
            alpha_y = std::max(0.001f, _param.roughness * _param.roughness * aspect);
            alpha_c = MR_Math::lerp(0.1, 0.001, _param.clearcoatGloss);
        };

        vec3 sampleBSDF(const vec3 &wo, vec3 &wi, float &pdf, std::shared_ptr<MR::Sampler> &sampler) const override
        {
            vec3 wm;
            // Sampling
            {
                float weight_d = (1.0f - _param.metallic);
                float weight_s = 1.0f;
                float weight_c = _param.clearcoat;

                float sum_weight = weight_d + weight_s + weight_c;

                float c_d = weight_d / sum_weight;
                float c_s = weight_s / sum_weight;
                float c_c = weight_c / sum_weight;

                float p = sampler->getSample();

                float u = sampler->getSample();
                float v = sampler->getSample();

                if (c_d > p)
                {
                    // Diffuseを選択
                    wi = diffuseSampling(u, v);
                    wm = normalize(wo + wi);
                }
                else if (c_d + c_s > p)
                {
                    // Specularを選択
                    wm = specularHalfSampling(u, v, alpha_x, alpha_y);
                    wi = MR_Math::reflect(wo, wm);
                }
                else
                {
                    // ClearCoatを選択
                    wm = clearcoatHalfSampling(u, v, alpha_c);
                    wi = MR_Math::reflect(wo, wm);
                }

                float half_jocobian = 1.0f / (4.0f * dot(wi, wm));

                float d_pdf = invPI * BSDFMath::cosTheta(wi);
                float s_pdf = ggx_D(wm, alpha_x, alpha_y) * BSDFMath::cosTheta(wm) * half_jocobian;
                float c_pdf = GTR1_D(wm, alpha_c) * BSDFMath::cosTheta(wm) * half_jocobian;

                pdf = d_pdf * c_d + s_pdf * c_s + c_pdf * c_c;
            }

            if (wi.y < 0.0)
            {
                pdf = 1.0;
                return vec3(0);
            }

            vec3 Diffuse;
            vec3 Subsurface;
            vec3 Specular;
            vec3 Sheen;
            vec3 Clearcoat;

            float in = BSDFMath::absCosTheta(wi);
            float on = BSDFMath::absCosTheta(wo);

            // Diffuse
            {
                float FD90o = FD90(wo);
                float FD90i = FD90(wi);

                float termo = 1.0f + (FD90o - 1.0f) * std::pow((1.0 - BSDFMath::cosTheta(wo)), 5.0f);
                float termi = 1.0f + (FD90i - 1.0f) * std::pow((1.0 - BSDFMath::cosTheta(wi)), 5.0f);

                Diffuse = _param.basecolor * termo * termi / PI;
            }

            // Subsurface
            {
                float Fss90o = FSS90(wo);
                float Fss90i = FSS90(wi);

                float termo = (1.0f * Fss90o - 1.0) * std::pow(1.0f - BSDFMath::cosTheta(wo), 5.0);
                float termi = (1.0f * Fss90i - 1.0) * std::pow(1.0f - BSDFMath::cosTheta(wi), 5.0);
                float term = 1.0 / (BSDFMath::cosTheta(wo) + BSDFMath::cosTheta(wi)) - 0.5;

                Subsurface = _param.basecolor * 1.25f * (termo * termi * term + 1.0f);
            }

            vec3 rho_tint = _param.basecolor / Y(_param.basecolor);
            // Specular
            {
                vec3 rho_specular = MR_Math::lerp(vec3(1.0), rho_tint, _param.speculartint);
                vec3 FS0 = MR_Math::lerp(0.08f * _param.specular * rho_specular, _param.basecolor, _param.metallic);

                float s_D = ggx_D(wm, alpha_x, alpha_y);
                float s_G = smith_G2(wo, wi, wm, alpha_x, alpha_y);
                vec3 s_F = BSDFMath::Shlick_Fresnel(wo, wm, FS0);

                Specular = s_D * s_G * s_F / (4.0f * in * on);
            }

            // Sheen
            {
                vec3 rho_sheen = MR_Math::lerp(vec3(1.0), rho_tint, _param.sheentint);
                Sheen = _param.sheen * rho_sheen * std::pow(1.0f - dot(wi, wm), 5.0f);
            }

            // ClearCoat
            {
                float c_D = GTR1_D(wm, alpha_c);
                float c_G = smith_G2(wo, wi, wm, alpha_c, alpha_c);
                vec3 c_F = BSDFMath::Shlick_Fresnel(wo, wm, vec3(0.04));

                Clearcoat = _param.clearcoat * 0.25f * c_D * c_G * c_F / (4.0f * in * on);
            }
            return (MR_Math::lerp(Diffuse, Subsurface, _param.subsurface) + Sheen) * (1.0f - _param.metallic) + Specular + Clearcoat;
        }

        vec3 evaluateBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wm = normalize(wo + wi);
            vec3 Diffuse;
            vec3 Subsurface;
            vec3 Specular;
            vec3 Sheen;
            vec3 Clearcoat;

            float in = BSDFMath::absCosTheta(wi);
            float on = BSDFMath::absCosTheta(wo);

            // Diffuse
            {
                float FD90o = FD90(wo);
                float FD90i = FD90(wi);

                float termo = 1.0f + (FD90o - 1.0f) * std::pow((1.0 - BSDFMath::cosTheta(wo)), 5.0f);
                float termi = 1.0f + (FD90i - 1.0f) * std::pow((1.0 - BSDFMath::cosTheta(wi)), 5.0f);

                Diffuse = _param.basecolor * termo * termi / PI;
            }

            // Subsurface
            {
                float Fss90o = FSS90(wo);
                float Fss90i = FSS90(wi);

                float termo = (1.0f * Fss90o - 1.0) * std::pow(1.0f - BSDFMath::cosTheta(wo), 5.0);
                float termi = (1.0f * Fss90i - 1.0) * std::pow(1.0f - BSDFMath::cosTheta(wi), 5.0);
                float term = 1.0 / (BSDFMath::cosTheta(wo) + BSDFMath::cosTheta(wi)) - 0.5;

                Subsurface = _param.basecolor * 1.25f * (termo * termi * term + 1.0f);
            }

            vec3 rho_tint = _param.basecolor / Y(_param.basecolor);
            // Specular
            {
                vec3 rho_specular = MR_Math::lerp(vec3(1.0), rho_tint, _param.speculartint);
                vec3 FS0 = MR_Math::lerp(0.08f * _param.specular * rho_specular, _param.basecolor, _param.metallic);

                float s_D = ggx_D(wm, alpha_x, alpha_y);
                float s_G = smith_G2(wo, wi, wm, alpha_x, alpha_y);
                vec3 s_F = BSDFMath::Shlick_Fresnel(wo, wm, FS0);

                Specular = s_D * s_G * s_F / (4.0f * in * on);
            }

            // Sheen
            {
                vec3 rho_sheen = MR_Math::lerp(vec3(1.0), rho_tint, _param.sheentint);
                Sheen = _param.sheen * rho_sheen * std::pow(1.0f - dot(wi, wm), 5.0f);
            }

            // ClearCoat
            {
                float c_D = GTR1_D(wm, alpha_c);
                float c_G = smith_G2(wo, wi, wm, alpha_c, alpha_c);
                vec3 c_F = BSDFMath::Shlick_Fresnel(wo, wm, vec3(0.04));

                Clearcoat = _param.clearcoat * 0.25f * c_D * c_G * c_F / (4.0f * in * on);
            }

            return (MR_Math::lerp(Diffuse, Subsurface, _param.subsurface) + Sheen) * (1.0f - _param.metallic) + Specular + Clearcoat;
        }
        float pdfBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wm = normalize(wo + wi);
            float weight_d = (1.0f - _param.metallic);
            float weight_s = 1.0f;
            float weight_c = _param.clearcoat;

            float sum_weight = weight_d + weight_s + weight_c;

            float c_d = weight_d / sum_weight;
            float c_s = weight_s / sum_weight;
            float c_c = weight_c / sum_weight;

            float half_jocobian = 1.0f / (4.0f * dot(wi, wm));

            float d_pdf = invPI * BSDFMath::cosTheta(wi);
            float s_pdf = ggx_D(wm, alpha_x, alpha_y) * BSDFMath::cosTheta(wm) * half_jocobian;
            float c_pdf = GTR1_D(wm, alpha_c) * BSDFMath::cosTheta(wm) * half_jocobian;

            return d_pdf * c_d + s_pdf * c_s + c_pdf * c_c;
        }
    };
}