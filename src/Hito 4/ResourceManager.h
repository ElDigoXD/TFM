#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <array>
#include <map>

#include "glm/glm.hpp"

/// Clase estática para gestionar recursos (shaders y modelos 3d).
class ResourceManager {
public:

    static std::map<const std::filesystem::path, wgpu::Buffer> vertex_buffers;

    /// Carga un shader a partir de un archivo.
    static wgpu::ShaderModule load_shader_module(const std::filesystem::path &path, const wgpu::Device &device) {
        std::ifstream shader_file = std::ifstream(path);
        if (!shader_file.is_open()) {
            std::cerr << "ERROR: Could not open shader file " << path << std::endl;
            return {};
        }
        std::string shader_code = std::string(std::istreambuf_iterator<char>(shader_file),
                                              std::istreambuf_iterator<char>());
        wgpu::ShaderSourceWGSL wgsl{{.code = shader_code.c_str()}};

        wgpu::ShaderModuleDescriptor shader_module_descriptor{
                .nextInChain = &wgsl,
                .label = path.filename().string().c_str(),
        };

        return device.CreateShaderModule(&shader_module_descriptor);
    }

    /// Carga un modelo 3D a partir de un archivo .obj, y devuelve un vertex buffer. Utiliza caché de los modelos.
    static wgpu::Buffer get_vertex_buffer(const std::filesystem::path &path, const wgpu::Device &device) {
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

    /// Carga un modelo 3D a partir de un archivo .obj utilizando la librería tinyobjloader.
    /// Devuelve un vector con los atributos de los vértices (posición, normal y UV).
    static bool load_obj(const std::filesystem::path &path, std::vector<VertexAttributes> &out_vertex_attributes) {
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
};