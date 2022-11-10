#pragma once

#include <glm/glm.hpp>
#include <Material/bsdf.hpp>
#include <memory>
#include <string>
#include <Image/texture.hpp>
#include <Material/lambert.hpp>

using namespace glm;

namespace MR
{
    // Infomation of Material
    // for Debug or additional material informaltion
    struct MaterialInfo
    {
        vec3 basecolor = vec3(0);
        std::shared_ptr<Texture> basecolor_tex = nullptr;

        float metallic = 0;
        std::shared_ptr<Texture> metallic_tex = nullptr;

        float roughness = 0;
        std::shared_ptr<Texture> roughness_tex = nullptr;

        float specular = 0;
        std::shared_ptr<Texture> specular_tex = nullptr;

        float sheen = 0;
        std::shared_ptr<Texture> sheen_tex = nullptr;

        float subsurface = 0;
        std::shared_ptr<Texture> subsurface_tex = nullptr;

        float ior = 1.0;
        float transmittion = 0;

        float absorbtion = 0;
    };

    class Material
    {
    public:
        virtual std::shared_ptr<MR_BSDF::BSDF> getMaterial(const vec2 &uv) const = 0;
        virtual MaterialInfo getMaterialInfomation() const = 0;
        virtual std::string getMaterialName() const = 0;
        virtual std::string getMaterialType() const = 0; // ex. Lambert,Phong

        virtual void writeTextures() const = 0;
    };

    class Diffuse_Lambert : public Material
    {
    private:
        MaterialInfo _matinfo;
        std::string _matname;

    public:
        Diffuse_Lambert(const std::string &matname, MaterialInfo &info) : _matname(matname), _matinfo(info) {}

        std::shared_ptr<MR_BSDF::BSDF> getMaterial(const vec2 &uv) const override
        {
            if (_matinfo.basecolor_tex == nullptr)
                return std::make_shared<MR_BSDF::Lambert>(_matinfo.basecolor);
            else
                return std::make_shared<MR_BSDF::Lambert>(_matinfo.basecolor_tex->getPixel(uv.x, uv.y));
        };
        MaterialInfo getMaterialInfomation() const override
        {
            return _matinfo;
        };
        std::string getMaterialName() const override
        {
            return _matname;
        };
        std::string getMaterialType() const override
        {
            return "Lambert";
        };
    };
    /*
        class Specular_Phong : public Material
        {
            std::shared_ptr<MR_BSDF::BSDF> getMaterial(const vec2 &uv) const {

            };
            MaterialInfo getMaterialInfomation(const vec2 &uv) const {

            };
            std::string getMaterialName() const {

            };
            std::string getMaterialType() const {

            }; // ex. Lambert,Phong
        };

        class Specular_Blinn_Phong : public Material
        {
        };
        class Specular_Microfacet_Beckmann : public Material
        {
        };

        class Specular_Microfacet_GGX : public Material
        {
        };

        class DisneyBRDF : public Material
        {
        };

        class DisneyBSDF : public Material
        {
        };
    */
}