#pragma once

#include <filesystem>
#include <vector>
#include <map>

#include "webgpu/webgpu_cpp.h"

#include "Renderer.h"


/// Clase estática para gestionar recursos (shaders y modelos 3d). (Podría ser un singleton).
class ResourceManager {
public:

    /// Caché de buffers de vértices, para evitar cargar el mismo modelo 3D varias veces.
    static std::map<const std::filesystem::path, wgpu::Buffer> vertex_buffers;

    /// Carga un shader (wgsl) a partir de un archivo.
    /// @param path Ruta al archivo del shader.
    /// @param device Dispositivo lógico donde crear el ShaderModule.
    /// @returns Un ShaderModule a partir del código del shader del archivo.
    static wgpu::ShaderModule load_shader_module(const std::filesystem::path &path, const wgpu::Device &device);

    /// Carga un modelo 3D a partir de un archivo .obj, y devuelve un vertex buffer. Utiliza caché de los modelos.
    /// @param path Ruta al archivo del modelo 3D.
    /// @param device Dispositivo lógico donde crear el Buffer.
    /// @returns Un Buffer con los atributos de los vértices (posición, normal y UV) del modelo 3D.
    static wgpu::Buffer get_vertex_buffer(const std::filesystem::path &path, const wgpu::Device &device);

    /// Carga un modelo 3D a partir de un archivo .obj utilizando la librería tinyobjloader.
    /// @param path Ruta al archivo del modelo 3D.
    /// @param out_vertex_attributes Vector de salida con los atributos de los vértices (posición, normal y UV) del modelo 3D.
    /// @returns Si la carga del modelo 3D ha sido exitosa.
    static bool load_obj(const std::filesystem::path &path, std::vector<VertexAttributes> &out_vertex_attributes);
};