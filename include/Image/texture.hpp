#pragma once
#include <Image/image_io.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <string>
#include <cmath>
#include <algorithm>

namespace MR_Image
{
    class Texture
    {
    private:
        Image _image;
        std::string _texname;

    public:
        Texture(const std::string &texname, const glm::vec3 &color) : _texname(texname)
        {
            _image.setPixel(0, 0, color);
        }
        Texture(const std::string &texname, const std::string &filepath) : _texname(texname)
        {
            imageLoderPNG(filepath, _image);
        }

        glm::vec3 getPixel(float u, float v) const
        {
            unsigned int width = _image.getWidth();
            unsigned int height = _image.getHeight();

            unsigned int i = std::clamp(static_cast<unsigned int>(std::fmod(u, 1.0f) * width), (unsigned int)0, width - 1);
            unsigned int j = std::clamp(static_cast<unsigned int>(std::fmod(v, 1.0f) * height), (unsigned int)0, height - 1);

            if (i >= width || j >= height)
            {
                spdlog::error("[Image] index out of bounds ({0},{1})" + _texname, i, j);
                std::exit(EXIT_FAILURE);
            }

            return _image.getPixel(i, j);
        }

        void writePPM(const std::string &filename, bool check_gamma = true) const
        {
            _image.writePPM(filename, check_gamma);
        }
        void writePNG(const std::string &filename, bool check_gamma = true) const
        {
            _image.writePNG(filename, check_gamma);
        }
    };
}