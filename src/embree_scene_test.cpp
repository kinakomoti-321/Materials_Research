#include <Core/renderer.hpp>
#include <Camera/camera.hpp>
#include <Integrator/integrator.hpp>
#include <Integrator/classical.hpp>
#include <Camera/pinhole.hpp>
#include <Scene/scene.hpp>
#include <Sampler/sampler.hpp>
#include <Sampler/rng.hpp>
#include <glm/glm.hpp>
#include <memory>

int main()
{

    vec3 cp = vec3(0, 0, 1);
    vec3 atlook = normalize(vec3(0) - cp);
    std::shared_ptr<MR::Camera> camera = std::make_shared<MR::Pinhole>(cp, atlook, 45);

    MR::Renderer renderer;
    MR::RenderInformation render_info;
    render_info.width = 512;
    render_info.height = 512;
    render_info.filename = "Render_test.png";

    render_info.camera = camera;
    render_info.integrator = std::make_shared<MR::ClassicalRaytrace>();
    render_info.sampler = std::make_shared<MR::RNGrandom>();
    render_info.scene.testScene();
    render_info.scene.sceneBuild();
    render_info.sample = 10;

    renderer.setRenderInfomation(render_info);
    renderer.render();

    return 0;
}