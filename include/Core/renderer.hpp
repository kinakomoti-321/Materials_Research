#pragma once

#include <Scene/scene.hpp>
#include <Sampler/sampler.hpp>
#include <Integrator/integrator.hpp>
#include <Camera/camera.hpp>
#include <omp.h>
#include <memory>
#include <Core/logger.hpp>

namespace MR
{
    struct RenderInformation
    {
        std::string name;
        unsigned int width;
        unsigned int height;
        unsigned int sample;

        std::shared_ptr<Integrator> integrator;
        std::shared_ptr<Sampler> sampler;
        std::shared_ptr<Camera> camera;
        Scene scene;

        std::string filepath;
        std::string filename;
    };
    class Renderer
    {
    private:
        RenderInformation _render_info;

    public:
        Renderer() {}
        void setRenderInfomation(const RenderInformation &renderinfo)
        {
            _render_info = renderinfo;
        }
        void render() const
        {
            unsigned int width = _render_info.width;
            unsigned int height = _render_info.height;
            Image image(width, height);

            startLog("Rendering");
            std::cout << "name : " << _render_info.name << std::endl;
            std::cout << "width : " << _render_info.width << std::endl;
            std::cout << "height : " << _render_info.height << std::endl;
            std::cout << "sample : " << _render_info.sample << std::endl;
            std::cout << "camera : " << _render_info.camera->getCameraType() << std::endl;
            std::cout << "integrator : " << _render_info.integrator->getType() << std::endl;
            std::cout << "sampler : " << _render_info.sampler->getSamplerName() << std::endl;
            std::cout << "filename : " << _render_info.filename << std::endl;

            // #pragma omp parallel for schedule(dynamic, 1)
            for (int j = 0; j < height; j++)
            {
                for (int i = 0; i < width; i++)
                {
                    auto sampler = _render_info.sampler->clone();
                    sampler->setSeed(i + j * width);
                    vec3 s_color = vec3(0);

                    for (int k = 0; k < _render_info.sample; k++)
                    {
                        const float u = (2.0f * (i + sampler->getSample() - 0.5f) - width) / height;
                        const float v = (2.0f * (j + sampler->getSample() - 0.5f) - height) / height;
                        float cam_weight;
                        Ray cam_ray = _render_info.camera->getCameraRay(u, v, cam_weight);
                        s_color += _render_info.integrator->integrate(cam_ray, _render_info.scene, sampler);
                        // s_color += cam_ray.direction;
                    }

                    image.setPixel(i, j, s_color / static_cast<float>(_render_info.sample));
                }
            }

            endLog("Rendering");
            image.writePNG(_render_info.name);
        }
    };
}