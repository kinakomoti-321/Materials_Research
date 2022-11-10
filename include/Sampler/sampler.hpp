#pragma once
#include <limits.h>
#include <memory>

namespace MR
{
    class Sampler
    {
    public:
        virtual float getSample() = 0;
        virtual void setSeed(uint64_t inseed) = 0;
        virtual std::string getSamplerName() const = 0;
        virtual std::shared_ptr<Sampler> clone() const = 0;
    };
}