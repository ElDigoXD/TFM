#pragma once

#include "webgpu/webgpu_cpp.h" // Para wgpu::Buffer, wgpu::BindGroup

#include "Renderer.h"

/// Representa un objeto a renderizar, con su vertex buffer y su update
class GameObject {
public:
    /// Buffer que contiene los vértices del objeto con el formato VertexAttributes (posición, normal, uv).
    wgpu::Buffer vertex_buffer;
    /// Bind group que contiene el uniform buffer con la matriz de transformación del objeto.
    wgpu::BindGroup bind_group;
    /// Buffer que contiene la matriz de transformación del objeto.
    wgpu::Buffer transform_buffer;

    /// Transformación del objeto (posición, rotación y escala).
    Transform transform;

    /// Función que se ejecuta cada frame.
    /// Se encarga de actualizar la transformación del objeto.
    /// @param delta_time Tiempo transcurrido desde el último frame, en segundos.
    void update(float delta_time);

    /// Crea el uniform buffer y el bind group del objeto. Necesarios para poder renderizarlo con la tranformación.
    /// @param renderer Referencia al renderer, necesario para crear el bind group con el layout de la pipeline.
    void create_bind_group(const Renderer &renderer);
};