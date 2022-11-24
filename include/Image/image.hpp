#pragma once
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stb_image_write.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <Image/tone_map.hpp>

namespace MR
{
    class Image
    {
    private:
        float *_pixel;
        unsigned int _width;
        unsigned int _height;

    public:
        Image() : _width(1), _height(1) { _pixel = new float[3]; };
        Image(unsigned int width, unsigned int height) : _width(width), _height(height) { _pixel = new float[width * height * 3]; };
        ~Image() { delete[] _pixel; }

        void resize(unsigned int width, unsigned int height)
        {
            delete[] _pixel;

            // Reset
            _width = width;
            _height = height;
            _pixel = new float[3 * _width * _height];
        }

        void setPixel(unsigned int i, unsigned int j, const glm::vec3 &color)
        {
            if (i >= _width || j >= _height)
            {
                spdlog::error("[Image] index out of bounds ({0},{1})", i, j);
                std::exit(EXIT_FAILURE);
            }
            const unsigned int index = 3 * (i + j * _width);
            _pixel[index] = color.x;
            _pixel[index + 1] = color.y;
            _pixel[index + 2] = color.z;
        }

        unsigned int getWidth() const
        {
            return _width;
        }
        unsigned int getHeight() const
        {
            return _height;
        }

        glm::vec3 getPixel(unsigned int i, unsigned int j) const
        {
            if (i >= _width || j >= _height)
            {
                spdlog::error("[Image] index out of bounds ({0},{1})", i, j);
                std::exit(EXIT_FAILURE);
            }
            const unsigned int index = 3 * (i + j * _width);
            return glm::vec3(_pixel[index], _pixel[index + 1], _pixel[index + 2]);
        }

        glm::vec3 getPixel(float i, float j) const
        {
            if (i > 1.0f || j > 1.0f || i < 0.0f || j < 0.0f)
            {
                spdlog::error("[Image] index out of bounds ({0},{1})", i, j);
                std::exit(EXIT_FAILURE);
            }

            unsigned int w = std::clamp(static_cast<unsigned int>(_width * i), (unsigned int)0, _width - 1);
            unsigned int h = std::clamp(static_cast<unsigned int>(_height * j), (unsigned int)0, _height - 1);
            const unsigned int index = 3 * (w + h * _width);
            return glm::vec3(_pixel[index], _pixel[index + 1], _pixel[index + 2]);
        }

        void writePPM(const std::string &filename, bool check_gamma = true) const
        {
            std::ofstream file(filename + ".ppm");

            if (!file)
            {
                std::cerr << "failed to open " << filename << std::endl;
                return;
            }

            file << "P3" << std::endl;
            file << _width << " " << _height << std::endl;
            file << "255" << std::endl;
            for (int j = 0; j < _height; ++j)
            {
                for (int i = 0; i < _width; ++i)
                {
                    const int idx = 3 * i + 3 * _width * j;
                    float R = _pixel[idx];
                    float G = _pixel[idx + 1];
                    float B = _pixel[idx + 2];
                    if (check_gamma)
                    {
                        R = std::pow(R, 1 / 2.2f);
                        G = std::pow(G, 1 / 2.2f);
                        B = std::pow(B, 1 / 2.2f);
                    }

                    // 各成分を[0, 255]に含まれるように変換し出力
                    file << static_cast<unsigned int>(std::clamp(255.0f * R, 0.0f, 255.0f))
                         << " ";
                    file << static_cast<unsigned int>(std::clamp(255.0f * G, 0.0f, 255.0f))
                         << " ";
                    file << static_cast<unsigned int>(std::clamp(255.0f * B, 0.0f, 255.0f))
                         << std::endl;
                }
            }
            file.close();
        }

        void writePNG(const std::string &filename, bool check_gamma = true) const
        {
            unsigned char *upixel = new unsigned char[_width * _height * 3];
            for (int j = 0; j < _height; j++)
            {
                for (int i = 0; i < _width; i++)
                {
                    const unsigned int idx = 3 * (i + j * _width);

                    float R = _pixel[idx];
                    float G = _pixel[idx + 1];
                    float B = _pixel[idx + 2];

                    vec3 RGB(R, G, B);
                    RGB = toneMappting_Uchimura(RGB);

                    if (check_gamma)
                    {
                        RGB.x = std::pow(RGB.x, 1 / 2.2f);
                        RGB.y = std::pow(RGB.y, 1 / 2.2f);
                        RGB.z = std::pow(RGB.z, 1 / 2.2f);
                    }

                    R = std::min(RGB.x, 1.0f);
                    G = std::min(RGB.y, 1.0f);
                    B = std::min(RGB.z, 1.0f);

                    upixel[idx] = std::clamp(static_cast<unsigned char>(R * 255), (unsigned char)0, (unsigned char)255);
                    upixel[idx + 1] = std::clamp(static_cast<unsigned char>(G * 255), (unsigned char)0, (unsigned char)255);
                    upixel[idx + 2] = std::clamp(static_cast<unsigned char>(B * 255), (unsigned char)0, (unsigned char)255);
                }
            }

            stbi_write_png(filename.c_str(), _width, _height, 3, upixel, _width * sizeof(unsigned char) * 3);
        }
    };
}