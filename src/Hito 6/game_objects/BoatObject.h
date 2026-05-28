#pragma once

#include "../global_defines.h"
#include "../ResourceManager.h"
#include "GameObject.h"

class BoatObject : public GameObject {
    const char *model_path = RESOURCE_DIR "fourareen/fourareen.obj";
    const char *texture_path = RESOURCE_DIR "fourareen/fourareen2K_albedo.jpg";

    wgpu::Texture texture;
    wgpu::BindGroup texture_bind_group;

public:

    BoatObject() {
    }

    void init(wgpu::RenderPipeline pipeline, const wgpu::Device &device) {
        this->pipeline = pipeline;
        vertex_buffer = ResourceManager::get_vertex_buffer(model_path, device);
        texture = ResourceManager::get_texture(texture_path, device);

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

        // Crea el bind group la textura y el sampler.
        wgpu::TextureView texture_view = texture.CreateView();
        wgpu::BindGroupEntry texture_bind_group_entry{
                // @binding(0) en el shader
                .binding = 0,
                .textureView = texture_view,
        };

        wgpu::SamplerDescriptor sampler_desc{
                .addressModeU = wgpu::AddressMode::MirrorRepeat,
                .addressModeV = wgpu::AddressMode::MirrorRepeat,
                .addressModeW = wgpu::AddressMode::MirrorRepeat,
                .magFilter = wgpu::FilterMode::Linear,
                .minFilter = wgpu::FilterMode::Linear,
                .mipmapFilter = wgpu::MipmapFilterMode::Linear,
                .lodMinClamp = 0.0f,
                .lodMaxClamp = 8.0f,
                .compare = wgpu::CompareFunction::Undefined,
                .maxAnisotropy = 1,
        };
        wgpu::Sampler sampler = device.CreateSampler(&sampler_desc);
        wgpu::BindGroupEntry sampler_bind_group_entry{
                // @binding(1) en el shader
                .binding = 1,
                .sampler = sampler,
        };

        std::vector<wgpu::BindGroupEntry> entries{
                texture_bind_group_entry,
                sampler_bind_group_entry
        };

        wgpu::BindGroupDescriptor texture_bind_group_desc{
                // @group(2) en el shader
                .layout = pipeline.GetBindGroupLayout(2),
                .entryCount = entries.size(),
                .entries = entries.data(),
        };

        texture_bind_group = device.CreateBindGroup(&texture_bind_group_desc);
    }

    void update(float delta_time) {
    }

    void render(const wgpu::RenderPassEncoder &encoder, const wgpu::Queue &queue) {
        auto transform_matrix = transform.get_matrix();
        queue.WriteBuffer(transform_buffer, 0, &transform_matrix, sizeof(mat4));

        encoder.SetPipeline(pipeline);
        encoder.SetBindGroup(1, bind_group);
        encoder.SetBindGroup(2, texture_bind_group);

        encoder.SetVertexBuffer(0, vertex_buffer);
        encoder.Draw(vertex_buffer.GetSize() / sizeof(VertexAttributes));
    }
};