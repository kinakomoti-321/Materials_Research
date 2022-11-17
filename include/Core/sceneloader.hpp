#pragma once

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
#include <nlohmann/json.hpp>

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
            }

            // SceneModel
            {

            }

            // IBL
            {

            }

            // Sampler
            {
                std::string sampler_str = jobj["Integrator"];
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
                if (inter_str == "pathtracer")
                {
                    _renderInfo.integrator = std::make_shared<Pathtracer>();
                }
                else if (inter_str == "basecolor_check")
                {
                    _renderInfo.integrator = std::make_shared<BaseColorChecker>();
                }
            }

            //
        }
        catch (std::exception &e)
        {
            std::cerr << "Caught exception: " << e.what() << "\n";
            return false;
        }
        return true;
    }
}