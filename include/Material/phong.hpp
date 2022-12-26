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
    class Phong : public BSDF
    {
    private:
        vec3 _rho;
        float _n;

        vec3 cosine_n_sampling(float u, float v, float &pdf) const
        {
            // 1-uとuは確率的に等価なのでuと置いている
            float a = std::acos(std::pow(u, 1 / (1.0 + _n)));
            float phi = 2.0 * PI * v;

            pdf = (_n + 1) * std::pow(std::cos(a), _n) / (2.0 * PI);

            return MR_Math::sphericalDirection(a, phi);
        }

    public:
        Phong()
        {
            _rho = vec3(0);
            _n = 2;
        }
        Phong(const vec3 &rho, const float &n) : _rho(rho), _n(n) {}

        vec3 sampleBSDF(const vec3 &wo, vec3 &wi, float &pdf, std::shared_ptr<MR::Sampler> &sampler) const override
        {
            // 反射ベクトルの計算
            vec3 wr = MR_Math::reflect(wo, vec3(0, 1, 0));

            // wrを法線とする接空間の基底を計算
            vec3 rt, rb;
            MR_Math::tangentBasis(wr, rt, rb);

            // 出射方向ベクトルの計算
            vec3 wi_r = cosine_n_sampling(sampler->getSample(), sampler->getSample(), pdf);
            wi = MR_Math::localToWorld(wi_r, rt, wr, rb);

            // Normalized factor
            float normalize_fac;
            {
                // Phong
                // normalize_fac = (_n + 1) / (2 * PI);

                // Modify Phong
                normalize_fac = (_n + 2) / (2 * PI);
            }

            // BSDFの計算
            vec3 bsdf;
            {
                // Phong
                // bsdf = _rho * normalize_fac * std::pow(wi_r.y, _n)/BSDFMath::cosTheta(wi);

                // Modify Phong
                bsdf = _rho * normalize_fac * std::pow(wi_r.y, _n);
            }
            return bsdf;
        };

        vec3 evaluateBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wr = MR_Math::reflect(wo, vec3(0, 1, 0));

            // Normalized factor
            float normalize_fac;
            // Phong
            // normalize_fac = 2 * PI / (_n + 1);

            // Modify Phong
            normalize_fac = 2 * PI / (_n + 2);

            vec3 bsdf;
            // Phong
            // bsdf = _rho * normalize_fac * std::pow(std::abs(dot(wr, wi)), _n)/BSDFMath::cosTheta(wi);

            // Modify Phong
            bsdf = _rho * normalize_fac * std::pow(std::abs(dot(wr, wi)), _n);
            return bsdf;
        };
        float pdfBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wr = MR_Math::reflect(wo, vec3(0, 1, 0));
            return (_n + 1) * pow(dot(wr, wi), _n) / (2.0f * PI);
        };
    };
}
