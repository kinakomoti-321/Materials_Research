#include <iostream>
#include <Image/image.hpp>
#include <glm/glm.hpp>

int main()
{
    const unsigned int width = 512;
    const unsigned int height = 512;
    MR_Image::Image image(width, height);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            image.setPixel(i, j, glm::vec3(static_cast<float>(i) / width, static_cast<float>(j) / height, 0));
        }
    }
    image.writePPM("Image_test");
    image.writePNG("Image_test.png");

    // Error check
    image.getPixel((unsigned int)513, (unsigned int)513);
    return 0;
}