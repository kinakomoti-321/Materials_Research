#pragma once

#include <Scene/scene.hpp>
#include <Sampler/sampler.hpp>
#include <Integrator/integrate.hpp>

namespace MR
{
    class Renderer
    {
    private:
        unsigned int width;
        unsigned int height;

    public:
        Renderer(unsigned int width, unsigned int height) : width(width), height(height) {}
    };
}