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
    class Pathtracer : public Integrator
    {
    public:
        vec3 integrate(const MR::Ray &camera_ray, const MR::Scene &scene, std::shared_ptr<MR::Sampler> sampler) const override
        {
            Ray ray = camera_ray;
            unsigned int MAX_DEPTH = 10;
            vec3 LTE = vec3(0);
            vec3 throughput = vec3(1);
            float rossian_p = 1.0;
            for (int i = 0; i < MAX_DEPTH; i++)
            {
                rossian_p = std::max(std::max(std::max(throughput.x, throughput.y), throughput.z), 1.0f);

                if (rossian_p < sampler->getSample())
                {
                    break;
                }
                throughput /= rossian_p;
                IntersectionInfo info;
                if (!scene.intersect(ray, info))
                {
                    LTE = scene.getSkyLTE(ray.direction) * throughput;
                    break;
                }

                auto &bsdf = info.bsdf;
                auto &material_info = info.mat_info;

                if (length(material_info.emmision) > 0.0f)
                {
                    LTE = material_info.emmision * throughput;
                    break;
                }

                vec3 wo = -ray.direction;
                vec3 normal = info.normal;
                vec3 wi, local_wi, t, b;
                float pdf;

                MR_Math::tangentBasis(normal, t, b);

                vec3 local_wo = MR_Math::worldtoLocal(wo, t, normal, b);

                vec3 e_bsdf = bsdf->sampleBSDF(local_wo, local_wi, pdf, sampler);
                wi = MR_Math::localToWorld(local_wi, t, normal, b);

                float cosine = MR_Math::absdot(normal, wi);

                throughput *= e_bsdf * cosine / pdf;

                ray = Ray(info.position + 0.001f * normal, wi);
            }

            return LTE;
        };
        std::string getType() const { return "Classical Raytracer"; };
    };
}