#include "ResourceManager.h"

#include <string>
#include <fstream>
#include <iostream>

#include "glm_defines.h"

#define TINYOBJLOADER_IMPLEMENTATION

#include "tiny_obj_loader.h"

using std::filesystem::path;

std::map<const path, wgpu::Buffer> ResourceManager::vertex_buffers{};

wgpu::ShaderModule ResourceManager::load_shader_module(const path &path, const wgpu::Device &device) {
    // Lee el archivo
    std::ifstream shader_file = std::ifstream(path);
    if (!shader_file.is_open()) {
        std::cerr << "ERROR: Could not open shader file " << path << std::endl;
        return {};
    }
    std::string shader_code = std::string(std::istreambuf_iterator<char>(shader_file),
                                          std::istreambuf_iterator<char>());
    // Crea el ShaderModule
    wgpu::ShaderSourceWGSL wgsl{{.code = shader_code.c_str()}};

    wgpu::ShaderModuleDescriptor shader_module_descriptor{
            .nextInChain = &wgsl,
            .label = path.filename().string().c_str(),
    };

    return device.CreateShaderModule(&shader_module_descriptor);
}

wgpu::Buffer ResourceManager::get_vertex_buffer(const path &path, const wgpu::Device &device) {
    // Si el modelo existe en el caché, lo devuelve
    auto it = vertex_buffers.find(path);
    if (it != vertex_buffers.end())
        return it->second;

    // Si no existe, lo carga
    std::vector<VertexAttributes> vertex_attributes;
    load_obj(path, vertex_attributes);

    // Crea el vertex buffer
    wgpu::BufferDescriptor buffer_desc{
            .label = path.filename().string().c_str(),
            .usage = wgpu::BufferUsage::Vertex,
            .size = vertex_attributes.size() * sizeof(VertexAttributes),
            .mappedAtCreation = true
    };
    wgpu::Buffer vertex_buffer = device.CreateBuffer(&buffer_desc);
    std::memcpy(vertex_buffer.GetMappedRange(), vertex_attributes.data(),
                vertex_attributes.size() * sizeof(VertexAttributes));
    vertex_buffer.Unmap();

    // Guarda el vertex buffer en el caché y lo devuelve
    vertex_buffers[path] = vertex_buffer;
    return vertex_buffer;
}

bool ResourceManager::load_obj(const path &path, std::vector<VertexAttributes> &out_vertex_attributes) {
    // Similar al código de ejemplo de tinyobjloader, adaptado al formato de vertices VertexAttributes
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.string().c_str());

    if (!warn.empty()) std::cerr << "WARN:  tinyobj " << warn << std::endl;
    if (!err.empty()) std::cerr << "ERROR: tinyobj " << err << std::endl;
    if (!success) return false;

    out_vertex_attributes.clear();

    // Recorre todos los vertices y los guarda en el formato de vértice utilizado (VertexAttributes)
    for (const auto &shape: shapes) {
        const auto offset = out_vertex_attributes.size();
        out_vertex_attributes.resize(offset + shape.mesh.indices.size());

        for (auto i = 0; i < shape.mesh.indices.size(); i++) {
            const auto &idx = shape.mesh.indices[i];

            out_vertex_attributes[offset + i].position = {
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
            };
            out_vertex_attributes[offset + i].normal = (idx.normal_index >= 0)
                                                       ? glm::vec3{
                            attrib.normals[3 * idx.normal_index + 0],
                            attrib.normals[3 * idx.normal_index + 1],
                            attrib.normals[3 * idx.normal_index + 2]}
                                                       : glm::vec3{0.0f, 0.0f, 0.0f};

            if (idx.texcoord_index >= 0)
                out_vertex_attributes[offset + i].uv = {
                        attrib.texcoords[3 * idx.texcoord_index + 0],
                        attrib.texcoords[3 * idx.texcoord_index + 1]
                };
        }
    }

    // Crea normales si no existen
    if (out_vertex_attributes[0].normal == glm::vec3{0.0f, 0.0f, 0.0f}) {
        for (auto i = 0; i < out_vertex_attributes.size(); i += 3) {
            const auto &v0 = out_vertex_attributes[i + 0].position;
            const auto &v1 = out_vertex_attributes[i + 1].position;
            const auto &v2 = out_vertex_attributes[i + 2].position;

            const auto normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

            out_vertex_attributes[i + 0].normal = normal;
            out_vertex_attributes[i + 1].normal = normal;
            out_vertex_attributes[i + 2].normal = normal;
        }
    }

    return true;
}
