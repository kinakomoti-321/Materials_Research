#pragma once
#include <Core/ray.hpp>
#include <Scene/scene.hpp>
#include <Sampler/sampler.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <cmath>
#include <Integrator/integrator.hpp>

using namespace glm;
namespace MR
{
    class NormalChecker : public Integrator
    {
    public:
        vec3 integrate(const MR::Ray &camera_ray, const MR::Scene &scene, std::shared_ptr<MR::Sampler> sampler) const override
        {
            Ray ray = camera_ray;
            unsigned int MAX_DEPTH = 10;
            vec3 LTE = vec3(0);
            IntersectionInfo info;
            if (!scene.intersect(ray, info))
            {
                LTE = scene.getSkyLTE(ray.direction);
                return LTE;
            }
            auto &bsdf = info.bsdf;
            auto &material_info = info.mat_info;
            LTE = 0.5f * (info.normal + vec3(1.0));

            return LTE;
        };
        std::string getType() const { return "Classical Raytracer"; };
    };
}