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
    class ClassicalRaytrace : public Integrator
    {
    public:
        vec3 integrate(const MR::Ray &camera_ray, const MR::Scene &scene, std::shared_ptr<MR::Sampler> sampler) const override
        {
            Ray ray = camera_ray;
            unsigned int MAX_DEPTH;
            vec3 LTE = vec3(0);

            for (int i = 0; i < MAX_DEPTH; i++)
            {
                IntersectionInfo info;
                if (scene.intersect(ray, info))
                {
                    LTE = scene.getSkyLTE(ray.direction);
                    break;
                }

                auto &bsdf = info.bsdf;
                auto &material_info = info.mat_info;
                LTE = material_info.basecolor * std::max(dot(info.normal, normalize(vec3(1, 1, 1))), 0.0f);
                break;
            }

            return LTE;
        };
        std::string getType() const { return "Classical Raytracer"; };
    };
}