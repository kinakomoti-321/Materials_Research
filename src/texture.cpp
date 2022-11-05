#include <iostream>
#include <Image/image.hpp>
#include <glm/glm.hpp>
#include <Image/texture.hpp>
int main()
{
    const unsigned int width = 512;
    const unsigned int height = 512;

    MR_Image::Texture tex1("tex", "./Test_texture.png");
    std::cout << tex1.getPixel(0, 0).x << std::endl;
    tex1.writePPM("Texture");
    return 0;
}