#pragma once

#include <Core/ray.hpp>
#include <Scene/scene.hpp>
#include <Sampler/sampler.hpp>
#include <glm/glm.hpp>
#include <memory>

namespace MR
{
    class Integrator
    {
    public:
        virtual glm::vec3 integrate(const MR::Ray &camera_ray, const MR::Scene &scene, std::shared_ptr<MR::Sampler> sampler) const = 0;
        virtual std::string getType() const = 0;
    };
}