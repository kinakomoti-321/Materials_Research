#pragma once
#include <iostream>
#include <string>

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
}