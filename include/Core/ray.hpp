#pragma once
#include <glm/glm.hpp>
#include <cmath>
#include <embree3/rtcore.h>

using namespace glm;
namespace MR
{
    struct Ray
    {
        vec3 origin;
        vec3 direction;
        float tmin = 1e-3;
        float tmax = 1e5;

        Ray()
        {
            origin = vec3(0);
            direction = vec3(0);
        }
        Ray(const vec3 origin, const vec3 direction) : origin(origin), direction(direction) {}

        vec3 operator()(float t) const
        {
            return origin + t * direction;
        }
    };

    RTCRayHit rayConvertRTCRayHit(const Ray &r)
    {
        RTCRayHit ray;
        ray.ray.org_x = r.origin.x;
        ray.ray.org_y = r.origin.y;
        ray.ray.org_z = r.origin.z;

        ray.ray.dir_x = r.direction.x;
        ray.ray.dir_y = r.direction.y;
        ray.ray.dir_z = r.direction.z;

        ray.ray.tnear = r.tmin;
        ray.ray.tfar = r.tmax;

        ray.hit.geomID = RTC_INVALID_GEOMETRY_ID;

        return ray;
    }
}