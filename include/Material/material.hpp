#pragma once

#include <glm/glm.hpp>
#include <Material/bsdf.hpp>
#include <memory>
#include <string>
#include <Image/texture.hpp>
#include <Material/lambert.hpp>
#include <Material/phong.hpp>
#include <Material/blinn_phong.hpp>
#include <Material/ideal_reflection.hpp>
#include <Material/ideal_refraction.hpp>

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

        float clearcoat = 0;

        float sheentint = 0;
        float speculartint = 0;

        float ior = 1.0;
        float transmittion = 0;

        float absorbtion = 0;

        vec3 emmision = vec3(0);

        std::shared_ptr<Texture> normal_tex = nullptr;
    };

    class Material
    {
    private:
        MaterialInfo _matinfo;
        std::string _matname;

    public:
        Material(const std::string &matname, MaterialInfo &info) : _matname(matname), _matinfo(info) {}

        std::shared_ptr<MR_BSDF::BSDF> getMaterial(const vec2 &uv) const
        {
            vec3 _basecolor;
            if (_matinfo.basecolor_tex == nullptr)
                _basecolor = _matinfo.basecolor;
            else
                _basecolor = _matinfo.basecolor_tex->getPixel(uv.x, uv.y);

            return std::make_shared<MR_BSDF::IdealRefraction>(vec3(1.0), 1.0);
        };
        MaterialInfo getMaterialInfomation() const
        {
            return _matinfo;
        };
        std::string getMaterialName() const
        {
            return _matname;
        };
        std::string getMaterialType() const
        {
            return "Lambert";
        };
        vec3 getTex(const vec2 &uv, const unsigned int index) const
        {
            if (_matinfo.basecolor_tex == nullptr)
                return _matinfo.basecolor;
            else
                return _matinfo.basecolor_tex->getPixel(uv.x, uv.y);
        };
        virtual void writeTextures() const
        {
            std::cout << "test" << std::endl;
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