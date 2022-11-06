#pragma once
#include <glm/glm.hpp>
#include <memory>

using namespace glm;
namespace MR
{
    struct IntersectionInfo
    {
        vec3 normal;
        vec3 position;
        float distance;
    };
}