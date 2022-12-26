#include <iostream>
#include <Core/gltfloader.hpp>
#include <Scene/model.hpp>
#include <Camera/camera.hpp>
#include <Camera/pinhole.hpp>
#include <glm/glm.hpp>

int main()
{
    MR::ModelInfo info;
    std::shared_ptr<MR::Camera> camera = std::make_shared<MR::Pinhole>(glm::vec3(0), glm::vec3(0), 45);
    gltfloader("test", "test", info, camera);
    return 0;
}