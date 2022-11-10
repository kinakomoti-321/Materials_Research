#pragma once
#include <iostream>
#include <string>
#include <glm/glm.hpp>

namespace MR
{
    void startLog(const std::string &str)
    {
        std::cout << "-------------------" << std::endl;
        std::cout << str << " : "
                  << "start" << std::endl;
        std::cout << "-------------------" << std::endl;
    }
    void endLog(const std::string &str)
    {
        std::cout << "-------------------" << std::endl;
        std::cout << str << " : "
                  << "end" << std::endl;
        std::cout << "-------------------" << std::endl;
    }
    inline std::ostream &operator<<(std::ostream &stream, const glm::vec3 &v)
    {
        stream << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
        return stream;
    }

}