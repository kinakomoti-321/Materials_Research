#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <Image/image.hpp>
#include <stb_image.h>
#include <string>

namespace MR
{
    inline void imageLoderPNG(const std::string &filepath, Image &image)
    {
        int width, height, channels;
        size_t pos = filepath.rfind('.');
        std::cout << "Texture Loading: " << filepath << std::endl;
        if (pos != std::string::npos)
        {
            std::string ext = filepath.substr(pos + 1, filepath.size() - pos);
            if (ext != "hdr")
            {
                std::cout << "PNG" << std::endl;
                unsigned char *img = stbi_load(filepath.c_str(), &width, &height, &channels, 3);
                if (!img)
                {
                    spdlog::error("[ImageLoader] failed to load " + filepath);
                    std::exit(EXIT_FAILURE);
                }

                std::cout << width << " , " << height << std::endl;
                image.resize(width, height);
                for (int j = 0; j < height; j++)
                {
                    for (int i = 0; i < width; i++)
                    {
                        constexpr float divider = 1.0f / 255.0f;
                        const float R = std::pow(img[3 * i + 3 * width * j] * divider, 2.2f);
                        const float G = std::pow(img[3 * i + 3 * width * j + 1] * divider, 2.2f);
                        const float B = std::pow(img[3 * i + 3 * width * j + 2] * divider, 2.2f);
                        image.setPixel(i, j, glm::vec3(R, G, B));
                    }
                }

                stbi_image_free(img);
            }
            else
            {
                int resx, resy;
                int comp;
                float *pixel = stbi_loadf(filepath.c_str(), &resx, &resy, &comp, 0);

                std::cout << "HDR" << std::endl;

                if (!pixel)
                {
                    spdlog::error("[ImageLoader] failed to load " + filepath);
                    std::exit(EXIT_FAILURE);
                }

                unsigned int width = resx;
                unsigned int height = resy;
                image.resize(width, height);
                for (int j = 0; j < height; j++)
                {
                    for (int i = 0; i < width; i++)
                    {
                        unsigned int idx = i * 3 + j * 3 * width;
                        image.setPixel(i, j, glm::vec3(pixel[idx], pixel[idx + 1], pixel[idx + 2]));
                    }
                }
                stbi_image_free(pixel);
            }
            std::cout << "Texture Finished: " << filepath << std::endl;
        }
    }
}