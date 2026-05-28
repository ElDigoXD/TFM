#pragma once

#include "GameObject.h"
#include "../ResourceManager.h"

class StaticGenericMeshObject : public GameObject {
public:
    StaticGenericMeshObject() {}

    void init(const char * model_path, wgpu::RenderPipeline pipeline, const wgpu::Device &device) {
        this->pipeline = pipeline;
        vertex_buffer = ResourceManager::get_vertex_buffer(model_path, device);
        this->create_transform_buffer_and_bind_group(device);
    }

    void update(float delta_time) {};

    void render(const wgpu::RenderPassEncoder &encoder, const wgpu::Queue &queue) {
        auto transform_matrix = transform.get_matrix();
        queue.WriteBuffer(transform_buffer, 0, &transform_matrix, sizeof(mat4));

        encoder.SetPipeline(pipeline);
        encoder.SetBindGroup(1, bind_group);

        encoder.SetVertexBuffer(0, vertex_buffer);
        encoder.Draw(vertex_buffer.GetSize() / sizeof(VertexAttributes));
    };
};