#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stb.h>
#include <glm/glm.hpp>

class Image
{
private:
    float *_pixel;
    unsigned int _width;
    unsigned int _height;

public:
    Image(unsigned int width, unsigned int height) : _width(width), _height(height) { _pixel = new float[width * height * 3]; };
    ~Image() { delete[] _pixel; }

    void setPixel(unsigned int i, unsigned int j, const glm::vec3 &color)
    {

        const unsigned int index = i + j * _width;
        _pixel[index] = color.x;
        _pixel[index + 1] = color.y;
        _pixel[index + 2] = color.z;
    }

    glm::vec3 getPixel(unsigned int i, unsigned int j) const
    {
        if (i >= _width || j >= _height)
        {
        }
        const unsigned int index = i + j * _width;
        return glm::vec3(_pixel[index], _pixel[index + 1], _pixel[index + 2]);
    }

    glm::vec3 getPixel(float i, float j) const
    {
        const unsigned int index = i + j * _width;
        return glm::vec3(_pixel[index], _pixel[index + 1], _pixel[index + 2]);
    }

    void writePPM(const std::string &filename) const
    {
    }
};