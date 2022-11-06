#pragma once
#include <Core/ray.hpp>
#include <Scene/scene.hpp>
#include <Sampler/sampler.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <Integrator/integrator.hpp>

using namespace glm;
namespace MR
{
    class ClassicalRaytrace : public Integrator
    {
    public:
        glm::vec3 integrate(const MR::Ray &camera_ray, const MR::Scene &Scene, std::shared_ptr<MR::Sampler> sampler) const {

        };
        std::string getType() const { return "Classical Raytracer"; };
    };
}