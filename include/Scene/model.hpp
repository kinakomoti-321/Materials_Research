#pragma once
#include <vector>
#include <Material/material.hpp>
#include <memory>

namespace MR
{
    struct ModelInfo
    {
        std::vector<float> vertices;
        std::vector<unsigned int> vert_indices;
        std::vector<float> normals;
        std::vector<float> texcoord;

        std::vector<unsigned int> mat_indices;
        std::vector<std::shared_ptr<Material>> materials;

        std::vector<unsigned int> light_indices;
    };
}