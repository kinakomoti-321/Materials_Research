#pragma once

#include <Sampler/sampler.hpp>
#include <Core/ray.hpp>
#include <Image/image.hpp>
#include <spdlog/spdlog.h>
#include <vector>
#include <Scene/sphere.hpp>
#include <Material/material.hpp>

namespace MR
{
    class Scene
    {
    private:
        std::vector<Sphere> _sphere;

    public:
        Scene() {}
        ~Scene() {}

        void sceneLoadGLTF()
        {
        }

        void addSphere(glm::vec3 origin, float radius)
        {
            _sphere.push_back(Sphere(origin, radius));
        }

        void sceneBuild()
        {
        }

        bool intersect(const Ray &ray, IntersectionInfo &info)
        {
            IntersectionInfo mininfo;
            mininfo.distance = ray.tmax;
            bool intersection = false;
            for (int i = 0; i < _sphere.size(); i++)
            {
                IntersectionInfo ninfo;
                if (_sphere[i].intersect(ray, ninfo))
                {
                    if (mininfo.distance > ninfo.distance)
                    {
                        mininfo = ninfo;
                        intersection = true;
                    }
                }
            }

            return intersection;
        }
    };
}