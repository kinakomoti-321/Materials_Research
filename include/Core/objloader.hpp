#pragma once

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <Scene/scene.hpp>
#include <Material/material.hpp>
#include <Scene/model.hpp>

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

using namespace glm;

// TODO
// sample test

std::shared_ptr<MR::Texture> loadTexture(std::map<std::string, std::shared_ptr<MR::Texture>> &known_tex, const std::string &filename, const std::string &modelpath, const std::string &tex_type)
{
    if (filename == "")
        return nullptr;

    if (known_tex.find(filename) != known_tex.end())
    {
        return known_tex[filename];
    }

    known_tex[filename] = std::make_shared<MR::Texture>(filename, modelpath + "/" + filename);
    return known_tex[filename];
}

bool loadObj(const std::string &filepath, const std::string &filename, MR::ModelInfo &model)
{
    tinyobj::ObjReader reader;
    std::cout << std::endl
              << "-------------------" << std::endl;
    std::cout << filename << " load start" << std::endl;
    std::cout << "-------------------" << std::endl;

    if (!reader.ParseFromFile(filepath + "/" + filename))
    {
        if (!reader.Error().empty())
        {
            std::cerr << reader.Error();
        }
        return false;
    }

    if (!reader.Warning().empty())
    {
        std::cout << std::endl
                  << "...loading error" << std::endl;
        std::cout << reader.Warning();
    }

    const auto &attrib = reader.GetAttrib();
    const auto &shapes = reader.GetShapes();
    const auto &load_materials = reader.GetMaterials();
    size_t index_offset_ = 0;

    bool has_material = load_materials.size() > 0;

    std::map<std::string, std::shared_ptr<MR::Texture>> known_tex;

    if (has_material)
    {
        for (int i = 0; i < load_materials.size(); i++)
        {
            // Matrial setting
            MR::MaterialInfo mat;
            load_materials[i].name;

            // Diffuse Color
            mat.basecolor = {load_materials[i].diffuse[0], load_materials[i].diffuse[1], load_materials[i].diffuse[2]};
            mat.basecolor_tex = loadTexture(known_tex, load_materials[i].diffuse_texname, filepath, "Diffuse");

            // Metallic
            mat.metallic = load_materials[i].metallic;
            mat.metallic_tex = loadTexture(known_tex, load_materials[i].metallic_texname, filepath, "Metallic");

            // Roghness
            mat.roughness = load_materials[i].roughness;
            // mat.roughness = 0.5f;
            mat.roughness_tex = loadTexture(known_tex, load_materials[i].roughness_texname, filepath, "Roughness");

            // Sheen
            mat.sheen = load_materials[i].sheen;
            mat.sheen_tex = loadTexture(known_tex, load_materials[i].sheen_texname, filepath, "Sheen");

            // Subsurface
            // ClearCoutRoughnessをsubsurfaceとして扱う
            mat.subsurface = load_materials[i].clearcoat_roughness;

            // Clearcoat
            mat.clearcoat = load_materials[i].clearcoat_thickness;

            // IOR
            mat.ior = load_materials[i].ior;

            // Specular
            mat.specular = load_materials[i].specular[0];

            // Normal map
            mat.normal_tex = loadTexture(known_tex, load_materials[i].bump_texname, filepath, "Normalmap");

            // Emmision
            mat.emmision = vec3(load_materials[i].emission[0], load_materials[i].emission[1], load_materials[i].emission[2]);

            model.materials.push_back(std::make_shared<MR::Material>(load_materials[i].name, mat));
        }
    }
    else
    {
        MR::MaterialInfo mat;
        mat.basecolor = vec3(1, 1, 1);
        mat.emmision = vec3(0);
        model.materials.push_back(std::make_shared<MR::Material>("Sample", mat));
    }

    // シェイプ数分のループ
    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_t index_offset = 0;
        // シェイプのフェイス分のループ
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {

            // シェイプsの面fに含まれる頂点数
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            vec3 nv[3];
            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                // シェイプのv番目の頂点座標
                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                model.vertices.push_back(vx);
                model.vertices.push_back(vy);
                model.vertices.push_back(vz);

                if (idx.normal_index >= 0)
                {
                    // シェイプのv番目の法線ベクトル
                    tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

                    vec3 n_s = normalize(vec3(nx, ny, nz));

                    model.normals.push_back(n_s.x);
                    model.normals.push_back(n_s.y);
                    model.normals.push_back(n_s.z);
                }
                else
                {
                    nv[v] = vec3(vx, vy, vz);
                }

                if (idx.texcoord_index >= 0)
                {
                    // シェイプのv番目のUV
                    tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

                    // std::cout << f << std::endl;
                    model.texcoord.push_back(tx);
                    model.texcoord.push_back(ty);
                }
                else
                {
                    model.texcoord.push_back(0);
                    model.texcoord.push_back(0);
                }
                // v番目のindex
                model.vert_indices.push_back(static_cast<unsigned int>(index_offset_ + index_offset + v));
            }

            if (attrib.normals.size() == 0)
            {
                const vec3 nv1 = normalize(nv[1] - nv[0]);
                const vec3 nv2 = normalize(nv[2] - nv[0]);
                vec3 geoNormal = normalize(cross(nv1, nv2));
                model.normals.push_back(geoNormal[0]);
                model.normals.push_back(geoNormal[1]);
                model.normals.push_back(geoNormal[2]);

                model.normals.push_back(geoNormal[0]);
                model.normals.push_back(geoNormal[1]);
                model.normals.push_back(geoNormal[2]);

                model.normals.push_back(geoNormal[0]);
                model.normals.push_back(geoNormal[1]);
                model.normals.push_back(geoNormal[2]);
            }

            if (has_material)
            {
                model.mat_indices.push_back(shapes[s].mesh.material_ids[f]);
            }
            else
            {
                model.mat_indices.push_back(shapes[s].mesh.material_ids[f]);
            }
            index_offset += fv;
        }
        index_offset_ += index_offset;
    }

    std::cout << std::endl
              << "...model information" << std::endl;
    std::cout << "the number of vertex : " << model.vertices.size() / 3 << std::endl;
    std::cout << "the number of normal : " << model.normals.size() / 3 << std::endl;
    std::cout << "the number of texcoord : " << model.texcoord.size() / 2 << std::endl;
    std::cout << "the number of face : " << model.vert_indices.size() << std::endl;
    std::cout << "the number of material : " << model.materials.size() << std::endl;
    return true;
}