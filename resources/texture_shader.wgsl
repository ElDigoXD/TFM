// Mismo formato de vertices que en C++
struct VertexInput {
    @location(0) vertex_pos : vec3f,
    @location(1) normal: vec3f,
    @location(2) uv: vec2f,
};

// Output del vertex shader e input del fragment shader
struct VertexOutput {
    @builtin(position) position : vec4f,
    @location(0) uv : vec2f,
    @location(1) normal: vec3f,
};

struct CameraUniforms {
    projection: mat4x4<f32>,
    view: mat4x4<f32>,
};

@group(0) @binding(0) var<uniform> camera : CameraUniforms;
@group(1) @binding(0) var<uniform> model : mat4x4<f32>;
@group(2) @binding(0) var albedo_texture: texture_2d<f32>;
@group(2) @binding(1) var texture_sampler: sampler;

// Vertex shader:
@vertex fn vertex_main(in: VertexInput) -> VertexOutput {
    return VertexOutput(camera.projection * camera.view * model * vec4f(in.vertex_pos, 1.0) , in.uv, in.normal);
}

// Fragment shader:
@fragment fn fragment_main(in: VertexOutput) -> @location(0) vec4f {
    return textureSample(albedo_texture, texture_sampler, in.uv);
}