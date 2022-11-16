#pragma once

#include <Sampler/sampler.hpp>
#include <Core/ray.hpp>
#include <Image/image.hpp>
#include <spdlog/spdlog.h>
#include <Core/logger.hpp>
#include <vector>
#include <Scene/sphere.hpp>
#include <Material/material.hpp>
#include <Core/objloader.hpp>
#include <embree3/rtcore.h>

namespace MR
{
    float plane_vert[9] = {
        1, -1, 0,
        1, 1, 0,
        -1, 1, 0};
    unsigned int plane_ind[3] = {
        0, 1, 2};

    float plane_norm[9] = {
        1, 0, 0,
        1, 0, 0,
        1, 0, 0};

    float plane_tex[6] = {
        0, 0,
        1, 0,
        1, 1};

    class Scene
    {
    private:
        // Geometry
        std::vector<Sphere> _sphere;

        struct ModelInfo
        {
            std::vector<float> vertices;
            std::vector<unsigned int> vert_indices;
            std::vector<float> normals;
            std::vector<float> texcoord;

            std::vector<unsigned int> mat_indices;
            std::vector<std::shared_ptr<Material>> materials;

            std::vector<unsigned int> light_indices;
        };

        ModelInfo _modelinfo;

        // Embree
        RTCDevice device;
        RTCScene scene;
        RTCGeometry geom;

        struct VertexIndex
        {
            unsigned int idx1;
            unsigned int idx2;
            unsigned int idx3;
        };

        void geomAttach()
        {
            unsigned int nvert = _modelinfo.vertices.size() / 3;
            float *vb = (float *)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float), nvert);

            for (int i = 0; i < _modelinfo.vertices.size(); i++)
            {
                vb[i] = _modelinfo.vertices[i];
                std::cout << vb[i] << std::endl;
            }

            unsigned int npoly = _modelinfo.vert_indices.size() / 3;
            unsigned int *ib = (unsigned int *)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(unsigned int), npoly);

            for (int i = 0; i < _modelinfo.vert_indices.size(); i++)
            {
                ib[i] = _modelinfo.vert_indices[i];
                std::cout << ib[i] << std::endl;
            }
        }

    public:
        Scene()
        {
            device = rtcNewDevice(NULL);
            scene = rtcNewScene(device);
            geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
        }
        ~Scene()
        {
            rtcReleaseScene(scene);
            rtcReleaseDevice(device);
        }

        void sceneLoadGLTF()
        {
        }

        void testScene()
        {
            for (int i = 0; i < 9; i++)
            {
                _modelinfo.vertices.push_back(plane_vert[i]);
                _modelinfo.normals.push_back(plane_norm[i]);
            }
            for (int i = 0; i < 3; i++)
            {
                _modelinfo.vert_indices.push_back(plane_ind[i]);
            }
            for (int i = 0; i < 6; i++)
            {
                _modelinfo.texcoord.push_back(plane_tex[i]);
            }
            for (int i = 0; i < 1; i++)
            {
                _modelinfo.mat_indices.push_back(0);
            }
            MaterialInfo mat_info;
            mat_info.basecolor = vec3(1);
            mat_info.roughness = 1;
            mat_info.specular = 1;
            mat_info.metallic = 1;
            mat_info.sheen = 1;

            auto mat = std::make_shared<Diffuse_Lambert>("testMaterial", mat_info);
            _modelinfo.materials.push_back(mat);
        }
        void addSphere(glm::vec3 origin, float radius)
        {
            _sphere.push_back(Sphere(origin, radius));
        }

        void sceneBuild()
        {
            startLog("Scene Build");

            geomAttach();

            rtcCommitGeometry(geom);
            std::cout << "Create Geometry success" << std::endl;

            rtcAttachGeometry(scene, geom);
            std::cout << "Attach Geometry success" << std::endl;

            rtcReleaseGeometry(geom);
            rtcCommitScene(scene);

            std::cout << "Scene Commit success" << std::endl;

            endLog("Scene Build");
        }

        VertexIndex getTriIndex(unsigned int faceID) const
        {
            unsigned int id = faceID * 3;
            VertexIndex index;
            index.idx1 = _modelinfo.vert_indices[id + 0];
            index.idx2 = _modelinfo.vert_indices[id + 1];
            index.idx3 = _modelinfo.vert_indices[id + 2];
            return index;
        }

        vec3 getVertexNormal(const unsigned int index) const
        {
            vec3 normal;
            unsigned int id = index * 3;

            normal[0] = _modelinfo.vertices[id + 0];
            normal[1] = _modelinfo.vertices[id + 1];
            normal[2] = _modelinfo.vertices[id + 2];

            return normal;
        }

        vec2 getVertexTexcoord(const unsigned int index) const
        {
            vec2 texcoord;
            unsigned int id = index * 2;

            texcoord[0] = _modelinfo.texcoord[id + 0];
            texcoord[1] = _modelinfo.texcoord[id + 1];

            return texcoord;
        }
        vec3 getPolyNormal(const unsigned int geomID, const vec2 bary) const
        {
            VertexIndex id = getTriIndex(geomID);

            vec3 n1 = getVertexNormal(id.idx1);
            vec3 n2 = getVertexNormal(id.idx2);
            vec3 n3 = getVertexNormal(id.idx3);

            return n1 * (1.0f - bary.x - bary.y) + n2 * bary.x + n3 * bary.y;
        }

        vec2 getPolyTexcoord(const unsigned int geomID, const vec2 bary) const
        {
            VertexIndex id = getTriIndex(geomID);
            vec2 t1 = getVertexTexcoord(id.idx1);
            vec2 t2 = getVertexTexcoord(id.idx2);
            vec2 t3 = getVertexTexcoord(id.idx3);
            return t1 * (1.0f - bary.x - bary.y) + t2 * bary.x + t3 * bary.y;
        }

        bool intersect(const Ray &ray, IntersectionInfo &info) const
        {
            /*
            IntersectionInfo mininfo;
            mininfo.distance = ray.tmax;
            bool intersection = false;
            for (int i = 0; i < _sphere.size(); i++)
            {
                IntersectionInfo ninfo;
                if (_sphere[i].intersect(ray, ninfo))
                {
                    if (mininfo.distance > ninfo.distance)
                    {
                        mininfo = ninfo;
                        intersection = true;
                    }
                }
            }
            info = mininfo;
            MaterialInfo mat_info;
            mat_info.basecolor = vec3(1);

            info.mat_info = mat_info;
            return intersection;
            */
            RTCRayHit rtc_ray = rayConvertRTCRayHit(ray);
            RTCIntersectContext context;
            rtcInitIntersectContext(&context);

            // std::cout << "intersect_" << std::endl;
            rtcIntersect1(scene, &context, &rtc_ray);

            if (rtc_ray.hit.geomID != RTC_INVALID_GEOMETRY_ID)
            {
                std::cout << "hit" << std::endl;
                info.distance = rtc_ray.ray.tfar;
                info.position = ray(info.distance);
                info.geometryID = rtc_ray.hit.primID;
                vec2 barycoord = vec2(rtc_ray.hit.u, rtc_ray.hit.v);
                info.normal = getPolyNormal(info.geometryID, barycoord);
                info.texcoord = getPolyTexcoord(info.geometryID, barycoord);

                unsigned int mat_index = _modelinfo.mat_indices[info.geometryID];
                auto &material = _modelinfo.materials[mat_index];
                info.bsdf = material->getMaterial(info.texcoord);
                info.mat_info = material->getMaterialInfomation();

                return true;
            }
            // std::cout << "koko?" << std::endl;

            return false;
        }

        vec3 getSkyLTE(const vec3 &direction) const
        {
            return vec3(0);
        }
    };
}