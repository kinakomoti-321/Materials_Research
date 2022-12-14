#pragma once

#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <Scene/scene.hpp>
#include <Core/renderer.hpp>

#include <Sampler/rng.hpp>

#include <Integrator/basecolor_checker.hpp>
#include <Integrator/classical.hpp>
#include <Integrator/normal_checker.hpp>
#include <Integrator/texcoord_checker.hpp>
#include <Integrator/pathtrace.hpp>

#include <Camera/camera.hpp>
#include <Camera/pinhole.hpp>

using json = nlohmann::json;

namespace MR
{
    bool sceneLoader(const std::string &filename, RenderInformation &_renderInfo)
    {
        try
        {
            std::ifstream ifs(filename);
            std::string jsonstr;

            if (ifs.fail())
            {
                std::cout << "File " << filename << " not found" << std::endl;
                return false;
            }

            std::string str;
            while (std::getline(ifs, str))
            {
                jsonstr += str + "\n";
            }

            auto jobj = json::parse(jsonstr.c_str());

            // Image Setting
            {
                _renderInfo.width = jobj["Image"]["width"];
                _renderInfo.height = jobj["Image"]["height"];
                _renderInfo.sample = jobj["Image"]["sample"];
                _renderInfo.name = jobj["Image"]["name"];
            }

            // SceneModel
            {
                _renderInfo.filepath = jobj["Scene"]["Filepath"];
                _renderInfo.filename = jobj["Scene"]["Filename"];
                _renderInfo.scene.oblLoad(_renderInfo.filepath, _renderInfo.filename);
            }

            // IBL
            {
                bool hdr_use = jobj["IBL"]["HDRI_Use"];
                auto enviroment = jobj["IBL"]["default_enviroment"];
                vec3 default_environt = vec3(enviroment[0], enviroment[1], enviroment[2]);

                if (hdr_use)
                {
                    std::string HDRI_path = jobj["IBL"]["HDRI_path"];
                    _renderInfo.scene.buildHDRI(HDRI_path);
                }
                else
                {
                    _renderInfo.scene.buildHDRI(default_environt);
                }
            }

            // Sampler
            {
                std::string sampler_str = jobj["Sampler"];
                if (sampler_str == "RNG")
                {
                    _renderInfo.sampler = std::make_shared<RNGrandom>();
                }
                else
                {
                    _renderInfo.sampler = std::make_shared<RNGrandom>();
                }
            }

            // Integrator
            {
                std::string inter_str = jobj["Integrator"];
                if (inter_str == "Pathtracer")
                {
                    _renderInfo.integrator = std::make_shared<Pathtracer>();
                }
                else if (inter_str == "basecolor_check")
                {
                    _renderInfo.integrator = std::make_shared<BaseColorChecker>();
                }
                else if (inter_str == "texcoord_check")
                {
                    _renderInfo.integrator = std::make_shared<TexcoordChecker>();
                }
                else
                {
                    _renderInfo.integrator = std::make_shared<BaseColorChecker>();
                }
            }

            // Camera
            {
                std::string camera_type = jobj["Camera"];
                if (camera_type == "Pinhole")
                {
                }
                else
                {
                }
            }

            // Scene Build
            _renderInfo.scene.sceneBuild();
        }
        catch (std::exception &e)
        {
            std::cerr << "Caught exception: " << e.what() << "\n";
            return false;
        }
        return true;
    }
}