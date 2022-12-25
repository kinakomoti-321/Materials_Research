#pragma once
#include <glm/glm.hpp>
#include <Material/bsdf.hpp>
#include <Sampler/sampler.hpp>
#include <Core/constant.hpp>
#include <Math/vecop.hpp>
#include <Math/bsdf_math.hpp>

using namespace glm;

namespace MR_BSDF
{
    class Phong : public BSDF
    {
    private:
        vec3 rho;
        float n;

    public:
        Phong()
        {
            rho = vec3(0);
            n = 2;
        }
        Phong(const vec3) {}
    }
}
