#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <Image/image.hpp>
#include <stb_image.h>
#include <string>

namespace MR_Image
{
    inline void imageLoderPNG(const std::string &filepath, Image &image)
    {
        int width, height, channels;
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
}