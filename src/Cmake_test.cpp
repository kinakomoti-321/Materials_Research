#include <iostream>
#include <glm/glm.hpp>

int main()
{
    auto a = glm::vec2(3, 5);
    std::cout << "a:" << a.x << " " << a.y << std::endl
              << "length(a)" << glm::length(a) << std::endl;
    return 0;
}
