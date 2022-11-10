#pragma once
#include <glm/glm.hpp>
#include <cmath>

using namespace glm;
namespace MR_Math
{
    inline void tangentBasis(const vec3 n, vec3 &t, vec3 &b)
    {
        if (std::abs(n[1]) < 0.9f)
        {
            t = cross(n, vec3(0, 1, 0));
        }
        else
        {
            t = cross(n, vec3(0, 0, -1));
        }
        t = normalize(t);
        b = cross(t, n);
        b = normalize(b);
    }

    inline float absdot(const vec3 &a, const vec3 &b)
    {
        return std::abs(dot(a, b));
    }

    inline vec3 reflect(const vec3 &v, const vec3 &n)
    {
        return -v + 2.0f * dot(v, n) * n;
    }

    inline vec3 localToWorld(const vec3 &v, const vec3 &lx, const vec3 &ly,
                             const vec3 &lz)
    {
        return vec3(v[0] * lx[0] + v[1] * ly[0] + v[2] * lz[0],
                    v[0] * lx[1] + v[1] * ly[1] + v[2] * lz[1],
                    v[0] * lx[2] + v[1] * ly[2] + v[2] * lz[2]);
    }

    inline vec3 worldtoLocal(const vec3 &v, const vec3 &lx, const vec3 &ly, const vec3 &lz)
    {
        return vec3(v[0] * lx[0] + v[1] * lx[1] + v[2] * lx[2],
                    v[0] * ly[0] + v[1] * ly[1] + v[2] * ly[2],
                    v[0] * lz[0] + v[1] * lz[1] + v[2] * lz[2]);
    }
    inline float norm(const vec3 &v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    inline float norm2(const vec3 &v)
    {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }

    inline bool refract(const vec3 &v, const vec3 &n, float ior1, float ior2,
                        vec3 &r)
    {
        const vec3 t_h = -ior1 / ior2 * (v - dot(v, n) * n);

        // 全反射
        if (norm(t_h) > 1.0)
            return false;

        const vec3 t_p = -std::sqrt(std::max(1.0f - norm2(t_h), 0.0f)) * n;
        r = t_h + t_p;

        return true;
    }
}