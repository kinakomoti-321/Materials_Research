#include <Core/renderer.hpp>
#include <Camera/camera.hpp>
#include <Integrator/integrator.hpp>
#include <Integrator/classical.hpp>
#include <Integrator/pathtrace.hpp>
#include <Camera/pinhole.hpp>
#include <Scene/scene.hpp>
#include <Sampler/sampler.hpp>
#include <Sampler/rng.hpp>
#include <Core/sceneloader.hpp>
#include <glm/glm.hpp>
#include <memory>

int main()
{
    vec3 cp = vec3(2, 0, 0);
    vec3 atlook = normalize(vec3(0) - cp);
    std::shared_ptr<MR::Camera> camera = std::make_shared<MR::Pinhole>(cp, atlook, 45);

    MR::Renderer renderer;
    MR::RenderInformation render_info;

    MR::sceneLoader("../scene/scene.json", render_info);
    render_info.camera = camera;

    renderer.setRenderInfomation(render_info);
    renderer.render();

    return 0;
}