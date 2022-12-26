#pragma once
#include <glm/glm.hpp>
#include <Sampler/sampler.hpp>
#include <memory>

using namespace glm;
namespace MR_BSDF
{
    class BSDF
    {
    public:
        virtual vec3 sampleBSDF(const vec3 &wo, vec3 &wi, float &pdf, std::shared_ptr<MR::Sampler> &sampler) const = 0;
        virtual vec3 evaluateBSDF(const vec3 &wo, const vec3 &wi) const = 0;
        virtual float pdfBSDF(const vec3 &wo, const vec3 &wi) const = 0;
    };

}