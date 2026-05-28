#include "GameObject.h"

void GameObject::update(float delta_time) {
    // Rota el objeto alrededor del eje Y
    transform.rotation.y -= delta_time;
}

void GameObject::create_bind_group(const Renderer &renderer) {
    // Crea un buffer uniforme para la matriz de transformación
    wgpu::BufferDescriptor uniform_buffer_desc{
            // El buffer se usará como uniforme y se podrá actualizar
            .usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
            .size = sizeof(mat4),
    };
    transform_buffer = renderer.get_device().CreateBuffer(&uniform_buffer_desc);

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
            .layout = renderer.get_pipeline().GetBindGroupLayout(1),
            .entryCount = 1,
            .entries = &bind_group_entry,
    };

    bind_group = renderer.get_device().CreateBindGroup(&bind_group_desc);
}
