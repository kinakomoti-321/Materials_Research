#pragma once
#include <glm/glm.hpp>
#include <Material/material.hpp>
#include <Material/bsdf.hpp>
#include <memory>

using namespace glm;
namespace MR
{
    struct IntersectionInfo
    {
        vec3 normal;
        vec3 position;
        float distance;
        vec2 texcoord;

        unsigned int geometryID;
        unsigned int instanceID;

        std::shared_ptr<MR_BSDF::BSDF> bsdf;
        MaterialInfo mat_info;
    };
}