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


// Vertex shader:
// Simplemente pasa los valores del del vertice al fragment shader
@vertex fn vertex_main(in: VertexInput) -> VertexOutput {
    return VertexOutput(vec4f(in.vertex_pos, 1), in.uv, in.normal);
}


// Fragment shader:
// Muestra el valor de la normal como color (x = r, y = g, z = b), cambiando el rango de la normal de [-1, 1] a [0, 1]
@fragment fn fragment_main(in: VertexOutput) -> @location(0) vec4f {
    return vec4f(in.normal / 2 + 0.5, 1);
}