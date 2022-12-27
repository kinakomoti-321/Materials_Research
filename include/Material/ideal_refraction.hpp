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
    class IdealRefraction : public BSDF
    {
    private:
        vec3 _rho;
        float _ior;

    public:
        IdealRefraction()
        {
            _rho = vec3(0);
            _ior = 1.0;
        }
        IdealRefraction(const vec3 &rho, const float &ior)
        {
            _rho = rho;
            _ior = ior;
        }

        vec3 sampleBSDF(const vec3 &wo, vec3 &wi, float &pdf, std::shared_ptr<MR::Sampler> &sampler) const override
        {
            vec3 normal;
            float ior1, ior2;
            // ガラスの中かどうかの判定、ガラスの中であれば法線は逆にして使用する
            if (wo.y > 0.0)
            {
                // ガラスの外
                normal = vec3(0, 1, 0);
                ior1 = 1.0;
                ior2 = _ior;
            }
            else
            {
                normal = vec3(0, -1, 0);
                ior1 = _ior;
                ior2 = 1.0;
            }

            // フレネルの計算
            float fresnel = BSDFMath::Shlick_Fresnel(wo, normal, ior1, ior2);
            vec3 bsdf;
            // 反射か屈折の選択

            if (fresnel > sampler->getSample())
            {
                pdf = fresnel;
                // 反射、IdealReflectionと同様に評価する
                wi = MR_Math::reflect(wo, normal);
                bsdf = fresnel * _rho / std::abs(wi.y);
            }
            else
            {
                pdf = 1.0f - fresnel;
                // 全反射かどうかの判定
                if (MR_Math::refract(wo, normal, ior1, ior2, wi))
                {
                    // 屈折
                    bsdf = (1.0f - fresnel) * ior1 * ior1 * _rho / (std::abs(wi.y) * ior2 * ior2);
                }
                else
                {
                    // 全反射
                    wi = MR_Math::reflect(wo, normal);
                    bsdf = fresnel * _rho / std::abs(wi.y);
                }
            }

            return bsdf;
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
