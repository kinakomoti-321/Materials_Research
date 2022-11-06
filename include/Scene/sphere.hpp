#pragma once

#include <glm/glm.hpp>
#include <cmath>
#include <Core/ray.hpp>
#include <Core/info.hpp>
#include <Math/vecop.hpp>

using namespace glm;
namespace MR
{
    class Sphere
    {
    private:
        vec3 center;  //中心位置
        float radius; //半径

    public:
        Sphere(const vec3 &center, const float radius) : center(center), radius(radius){};

        //衝突判定
        // false : 衝突しなかった、true : 衝突する
        // ray 受け取ったレイ、これと衝突するか判定する

        bool intersect(const Ray &ray, IntersectionInfo &info) const
        {
            vec3 oc = ray.origin - center;
            float b = dot(ray.direction, oc);
            float c = MR_Math::norm(oc) * MR_Math::norm(oc) - radius * radius;

            float D = b * b - c;

            //判定式 衝突しなかった場合
            if (D < 0)
            {
                return false;
            }

            //衝突する場合の処理
            float t1 = -b - std::sqrt(D);
            float t2 = -b + std::sqrt(D);

            float t = t1;
            // t1,t2は負ではない事、最小交差距離以上であること、最大交差距離以下であることが求められる
            if (t < ray.tmin || t > ray.tmax)
            {
                t = t2;
                if (t < ray.tmin || t > ray.tmax)
                {
                    // t1,t2が制限に満たない場合、衝突しないものとする
                    return false;
                }
            }

            info.distance = t;
            info.position = ray(t);
            info.normal = normalize(info.position - center);

            return true;
        }
    };
}