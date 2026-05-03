#pragma once

#include <array>

#include "webgpu/webgpu_cpp.h"

/// Representa un objeto a renderizar, con su vertex buffer y su update
class GameObject {
    wgpu::Buffer vertex_buffer;
public:

    void update(float delta_time) {};

    [[nodiscard]] wgpu::Buffer get_vertex_buffer() const { return vertex_buffer; }

    void set_vertex_buffer(const wgpu::Buffer &buffer) { vertex_buffer = buffer; }
};

