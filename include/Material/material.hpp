#pragma once

#include <glm/glm.hpp>
#include <Material/bsdf.hpp>
#include <memory>
#include <string>

using namespace glm;

namespace MR
{
    // Infomation of Material
    // for Debug or additional material informaltion
    struct MaterialInfo
    {
        vec3 basecolor;
        float metallic;
        float roughness;
        float specular;
        float sheen;
        float subsurface;
        float ior;
        float transmittion;
        float absorbtion;
    };

    class Material
    {
    public:
        virtual std::shared_ptr<BSDF> getMaterial(const vec2 &uv) const = 0;
        virtual MaterialInfo getMaterialInfomation(const vec2 &uv) const = 0;
        virtual std::string getMaterialName() const = 0;
        virtual std::string getMaterialType() const = 0; // ex. Lambert,Phong

        virtual void writeTextures() const = 0;
    };

    class Lambert : public Material
    {
    };

    class Phong : public Material
    {
    };

    class Blinn_Phong : public Material
    {
    };
    class Microfacet_Beckmann : public Material
    {
    };

    class Microfacet_GGX : public Material
    {
    };

    class DisneyBRDF : public Material
    {
    };

    class DisneyBSDF : public Material
    {
    };
}