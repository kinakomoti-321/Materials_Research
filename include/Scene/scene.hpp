#pragma once

#include <Sampler/sampler.hpp>
#include <Core/ray.hpp>
#include <Image/image.hpp>
#include <spdlog/spdlog.h>
#include <Core/logger.hpp>
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
            startLog("Scene Build");
            endLog("Scene Build");
        }

        bool intersect(const Ray &ray, IntersectionInfo &info) const
        {
            IntersectionInfo mininfo;
            mininfo.distance = ray.tmax;
            bool intersection = false;
            for (int i = 0; i < _sphere.size(); i++)
            {
                IntersectionInfo ninfo;
                std::cout << "intersection" << std::endl;
                if (_sphere[i].intersect(ray, ninfo))
                {
                    if (mininfo.distance > ninfo.distance)
                    {
                        mininfo = ninfo;
                        intersection = true;
                    }
                }
            }

            MaterialInfo mat_info;
            mat_info.basecolor = vec3(1);

            info.mat_info = mat_info;
            return intersection;
        }

        vec3 getSkyLTE(const vec3 &direction) const
        {
            return vec3(0);
        }
    };
}