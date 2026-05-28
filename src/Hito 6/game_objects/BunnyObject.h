#pragma once

#include "../global_defines.h"
#include "../ResourceManager.h"
#include "GameObject.h"

class BunnyObject : public GameObject {
    const char * model_path = RESOURCE_DIR "stanford-bunny.obj";

public:
    BunnyObject() {
    }

    void init(wgpu::RenderPipeline pipeline, const wgpu::Device &device) {
        this->pipeline = pipeline;
        vertex_buffer = ResourceManager::get_vertex_buffer(model_path, device);
        this->create_transform_buffer_and_bind_group(device);
    }

    void update(float delta_time) {
        transform.rotation.y -= delta_time;
    }

    void render(const wgpu::RenderPassEncoder &encoder, const wgpu::Queue &queue) {
        auto transform_matrix = transform.get_matrix();
        queue.WriteBuffer(transform_buffer, 0, &transform_matrix, sizeof(mat4));

        encoder.SetPipeline(pipeline);
        encoder.SetBindGroup(1, bind_group);

        encoder.SetVertexBuffer(0, vertex_buffer);
        encoder.Draw(vertex_buffer.GetSize() / sizeof(VertexAttributes));
    }
};