#pragma once
#include <Camera/camera.hpp>
#include <Math/vecop.hpp>
#include <glm/glm.hpp>

using namespace glm;

namespace MR
{
    // Pinhole Camera
    class Pinhole : public Camera
    {
    private:
        vec3 c_origin;
        vec3 c_direction;
        vec3 c_up;
        vec3 c_side;
        float f;

    public:
        Pinhole(const vec3 &origin, const vec3 &direction, float fov) : c_origin(origin), c_direction(direction)
        {
            MR_Math::tangentBasis(c_direction, c_up, c_side);
            f = 2.0f / std::tan(fov);
        }

        MR::Ray getCameraRay(float u, float v, float &c_weight) const override
        {
            MR::Ray ray;
            ray.origin = c_origin;
            ray.direction = normalize(c_direction - c_side * v + c_up * u);
            c_weight = 1.0;

            return ray;
        }

        std::string getCameraType() const override
        {
            return "PinholeCamera";
        }
    };
}