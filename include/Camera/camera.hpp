#pragma once

#include <Core/ray.hpp>
#include <string>

namespace MR
{
    class Camera
    {
    public:
        virtual MR::Ray getCameraRay(float u, float v, float &c_weight) const = 0;
        virtual std::string getCameraType() const = 0;
    };

}