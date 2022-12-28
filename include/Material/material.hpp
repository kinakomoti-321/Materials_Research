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
#include <Material/cook_torrance.hpp>
#include <Material/ggx.hpp>
#include <Material/disney_brdf.hpp>

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
            MR_BSDF::Disney_Param param;
            if (_matinfo.basecolor_tex == nullptr)
                param.basecolor = _matinfo.basecolor;
            else
                param.basecolor = _matinfo.basecolor_tex->getPixel(uv.x, uv.y);

            param.roughness = 0.0;
            param.metallic = 0.0;
            param.basecolor = vec3(0.8);
            return std::make_shared<MR_BSDF::DisneyBRDF>(param);
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
}