#pragma once

#include <vector>
#include <iostream>

#include <GLFW/glfw3.h> // Para GLFWwindow
#include <webgpu/webgpu_cpp.h>

#include "Camera.h"

class GameObject;

/// El formato de los vértices, con posición, normal y coordenadas UV.
struct VertexAttributes {
    /// Posición del vértice en el espacio 3D.
    glm::vec3 position;
    /// Normal del vértice.
    glm::vec3 normal;
    /// Coordenadas UV del vértice para texturas.
    glm::vec2 uv;
};

/// Representa una transformación en el espacio 3D, con posición, rotación y escala.
struct Transform {
    /// Posición del objeto en el espacio 3D.
    vec3 position{0.0f, 0.0f, 0.0f};
    /// Rotación del objeto en el espacio 3D, en radianes (ángulos de Euler).
    vec3 rotation{0.0f, 0.0f, 0.0f};
    /// Escala del objeto.
    vec3 scale{1.0f, 1.0f, 1.0f};

    /// Calcula la matriz de transformación a partir de la posición, rotación y escala.
    /// @return La matriz de transformación de coordenadas locales a coordenadas de mundo.
    [[nodiscard]]mat4 get_matrix() const {
        mat4 scale_matrix = glm::scale(mat4(1.0f), scale);
        mat4 translation_matrix = glm::translate(mat4(1.0f), position);
        mat4 rotation_matrix = glm::yawPitchRoll(rotation.y, rotation.x, rotation.z);

        /// El orden de multiplicación es relevante.
        return rotation_matrix * translation_matrix * scale_matrix;
    }
};

/// Clase con el comportamiento de renderizado.
class Renderer {
    /// Representa una instancia de WebGPU.
    wgpu::Instance instance;
    /// Representa un dispositivo físico.
    /// El adapter se utiliza para crear el device con las características (capabilities) deseadas.
    wgpu::Adapter adapter;
    /// Representa un dispositivo lógico.
    wgpu::Device device;

    /// Representa la superficie donde dibujar los gráficos.
    wgpu::Surface surface;

    /// Formato de la textura (número de canales, tamaño de cada canal y tipo de los canales).
    wgpu::TextureFormat texture_format;

    /// Representa el buffer de profundidad (depth buffer).
    /// Esto es una vista a la textura subyacente.
    wgpu::TextureView depth_texture_view;

    /// Representa la pipeline gráfica.
    wgpu::RenderPipeline pipeline;

    /// Estructura con los datos de la cámara que se van a enviar al shader.
    /// @see Camera
    struct CameraUniforms {
        /// Matriz de proyección de la cámara, que transforma las coordenadas de cámara a coordenadas de clip.
        mat4 projection;
        /// Matriz de vista de la cámara, que transforma las coordenadas de mundo a coordenadas de cámara.
        mat4 view;
    };

    /// Buffer para los datos de la cámara.
    wgpu::Buffer camera_uniform_buffer;
    /// Bind group de los datos de la cámara.
    wgpu::BindGroup camera_bind_group;

public:
    /// Inicializa el renderer y todos los elementos necesarios.
    /// @param window Ventana de GLFW para la que se va a renderizar (mediante la Surface).
    /// @return Si la ejecución ha sido exitosa.
    bool init(GLFWwindow *window);

    /// Inicializa el Device de WebGPU.
    /// Para ello primero se crea la instancia, luego se obtiene el Adapter y finalmente el Device.
    /// @return Si la ejecución ha sido exitosa.
    bool init_webgpu();

    /// Crea y configura la superficie con el dispositivo, formato de textura y tamaño.
    /// @param window Ventana de GLFW con la que crear la superficie.
    /// @return Si la ejecución ha sido exitosa.
    bool init_surface(GLFWwindow *window);

    /// Crea una render pipeline (vertex shader -> fragment shader).
    /// También configura el formato de los vértices, el depth buffer y los layouts de los bind groups necesarios para el shader.
    /// @return Si la ejecución ha sido exitosa.
    bool init_default_render_pipeline();

    /// Crea los uniform buffers necesarios para enviar los datos de la cámara al shader.
    /// También crea el bind group para utilizar el buffer enn el shader.
    /// \@group(1) \@binding(0) en el shader.
    /// @return Si la ejecución ha sido exitosa.
    bool init_uniform_buffers();

    /// Envía los comandos para renderizar la escena en la superficie a partir de una cámara y una lista de GameObjects.
    /// @param camera Cámara a través de la que renderizar la escena (matrices de vista y proyección).
    /// @param game_objects Lista de objetos a renderizar en la escena (malla de triángulos y matriz de transformación).
    void render(const Camera &camera, const std::vector<GameObject> &game_objects);

    /// @return La superficie.
    [[nodiscard]] wgpu::Surface get_surface() const;

    /// @returns La instancia de WebGPU.
    [[nodiscard]] wgpu::Instance get_instance() const;

    /// @return El dispositivo lógico.
    [[nodiscard]] wgpu::Device get_device() const;

    /// @return La pipeline gráfica.
    [[nodiscard]] wgpu::RenderPipeline get_pipeline() const;
};