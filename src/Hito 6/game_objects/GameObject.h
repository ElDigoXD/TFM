#pragma once

#include "webgpu/webgpu_cpp.h" // Para wgpu::Buffer, wgpu::BindGroup

#include "../Renderer.h"
#include "../Transform.h"

/// Representa un objeto a renderizar, con su vertex buffer y su update
class GameObject {
public:
    /// Buffer que contiene los vértices del objeto con el formato VertexAttributes (posición, normal, uv).
    wgpu::Buffer vertex_buffer;
    /// Bind group que contiene el uniform buffer con la matriz de transformación del objeto.
    wgpu::BindGroup bind_group;
    /// Buffer que contiene la matriz de transformación del objeto.
    wgpu::Buffer transform_buffer;

    wgpu::RenderPipeline pipeline;

    /// Transformación del objeto (posición, rotación y escala).
    Transform transform;

    /// Función que se ejecuta cada frame.
    /// Se encarga de actualizar la transformación del objeto.
    /// @param delta_time Tiempo transcurrido desde el último frame, en segundos.
    virtual void update(float delta_time) = 0;

    virtual void render(const wgpu::RenderPassEncoder &encoder, const wgpu::Queue &queue) = 0;

    void create_transform_buffer_and_bind_group(const wgpu::Device &device) {

        // Crea un buffer uniforme para la matriz de transformación
        wgpu::BufferDescriptor uniform_buffer_desc{
                // El buffer se usará como uniforme y se podrá actualizar
                .usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
                .size = sizeof(mat4),
        };
        transform_buffer = device.CreateBuffer(&uniform_buffer_desc);

        // Crea el bind group para el buffer anterior.
        wgpu::BindGroupEntry bind_group_entry{
                // @binding(0) en el shader
                .binding = 0,
                .buffer = transform_buffer,
                .offset = 0,
                .size = sizeof(mat4),
        };

        wgpu::BindGroupDescriptor bind_group_desc{
                // @group(1) en el shader
                .layout = pipeline.GetBindGroupLayout(1),
                .entryCount = 1,
                .entries = &bind_group_entry,
        };

        bind_group = device.CreateBindGroup(&bind_group_desc);
    }
};