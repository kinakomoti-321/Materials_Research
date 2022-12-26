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
    class BlinnPhong : public BSDF
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
            // pdf = std::pow(std::cos(a), _n);

            return MR_Math::sphericalDirection(a, phi);
        }

    public:
        BlinnPhong()
        {
            _rho = vec3(0);
            _n = 2;
        }
        BlinnPhong(const vec3 &rho, const float &n) : _rho(rho), _n(n) {}

        vec3 sampleBSDF(const vec3 &wo, vec3 &wi, float &pdf, std::shared_ptr<MR::Sampler> &sampler) const override
        {
            // ハーフベクトルのサンプリング&入射ベクトルの計算
            vec3 wh = cosine_n_sampling(sampler->getSample(), sampler->getSample(), pdf);

            wi = MR_Math::reflect(wo, wh);
            if (wi.y < 0.0)
            {
                pdf = 1.0;
                return vec3(0);
            }

            // PDFのヤコビアン
            pdf /= 4.0f * dot(wi, wh);

            // BSDF計算(Normalized Modify Blinn-Phong)
            vec3 bsdf = _rho * std::pow(wh.y, _n) * (_n + 8) / (8.0f * PI);

            return bsdf;
        };

        vec3 evaluateBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wh = normalize(wo + wi);

            // BSDF計算(Normalized Modify Blinn-Phong)
            vec3 bsdf = _rho * std::pow(wh.y, _n) * (_n + 8) / (8.0f * PI);

            return bsdf;
        };
        float pdfBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wh = normalize(wo + wi);
            float pdf = (_n + 1) * std::pow(wh.y, _n) / (2.0 * PI);
            return pdf / (4.0f * dot(wi, wh));
        };
    };
}
