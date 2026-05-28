#pragma once

#include <vector>
#include <iostream>
#include <map>

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
    wgpu::Texture depth_texture;
    /// Vista a la textura de profundidad.
    wgpu::TextureView depth_texture_view;
    /// Describe la configuración del depth buffer
    wgpu::DepthStencilState depth_stencil_state;

    /// Representa las distintas pipelines gráficas.
    std::unordered_map<std::string, wgpu::RenderPipeline> pipelines;

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

    /// Crea la superficie con el dispositivo, formato de textura y tamaño.
    /// @param window Ventana de GLFW con la que crear la superficie.
    /// @return Si la ejecución ha sido exitosa.
    bool init_surface(GLFWwindow *window);

    /// Crea y configura el depth buffer (sin stencil).
    /// @return Si la ejecución ha sido exitosa.
    bool init_depth_buffer();

    /// Crea la render pipeline por defecto (muestra las normales como colores).
    /// Se almacena en `pipelines["default"]`.
    /// @return Si la ejecución ha sido exitosa.
    bool init_default_render_pipeline();

    /// Crea una render pipeline capáz de renderizar objetos con textura de color.
    /// Se almacena en `pipelines["texture"]`.
    /// @return Si la ejecución ha sido exitosa.
    bool init_texture_render_pipeline();

    /// Inicializa las render pipelines.
    /// @return Si la ejecución ha sido exitosa.
    bool init_render_pipelines();

    /// Crea una render pipeline a partir de un shader y sus bind groups layouts necesarios.
    /// @param shader_path Ruta al shader en formato WGSL.
    /// @param bind_group_layouts Vector con los bind group layouts necesarios para el shader.
    /// @param label Nombre de la pipeline, para identificarla en depuración.
    /// @returns La render pipeline creada.
    wgpu::RenderPipeline create_render_pipeline(const char *shader_path,
                                                const std::vector<wgpu::BindGroupLayout> &bind_group_layouts,
                                                const char *label);

    /// Crea un bind group layout para un uniform buffer con la estructura de datos indicada.
    /// @tparam DataStruct Estructura de datos.
    /// @param visibility Etapa del shader en la que se va a utilizar el bind group (vertex, fragment o ambos).
    /// @param label Nombre del bind group layout, para identificarlo en depuración.
    /// @returns El bind group layout creado.
    template<typename DataStruct>
    wgpu::BindGroupLayout create_buffer_bind_group_layout(const wgpu::ShaderStage visibility, const char *label);

    /// Crea un bind group layout para una textura y un sampler.
    /// @param visibility Etapa del shader en la que se va a utilizar el bind group (vertex, fragment o ambos).
    /// @param label Nombre del bind group layout, para identificarlo en depuración.
    /// @returns El bind group layout creado.
    wgpu::BindGroupLayout create_texture_bind_group_layout(const wgpu::ShaderStage visibility, const char *label);

    /// Crea los uniform buffers necesarios para enviar los datos de la cámara al shader.
    /// También crea el bind group para utilizar el buffer enn el shader.
    /// \@group(0) \@binding(0) en el shader.
    /// @return Si la ejecución ha sido exitosa.
    bool init_uniform_buffers();

    /// Envía los comandos para renderizar la escena en la superficie a partir de una cámara y una lista de GameObjects.
    /// También renderiza la interfaz de usuario utilizando ImGui.
    /// @param camera Cámara a través de la que renderizar la escena (matrices de vista y proyección).
    /// @param game_objects Lista de objetos a renderizar en la escena.
    /// @param ui Función con los comandos inmediatos de ImGui para crear la interfaz de usuario.
    void render(const Camera &camera, const std::vector<GameObject *> &game_objects, const std::function<void()> &ui);

    /// Redimensiona la superficie al nuevo tamaño de la ventana. También se redimensiona el depth buffer.
    /// @param width Nuevo ancho de la ventana.
    /// @param height Nuevo alto de la ventana.
    void resize(unsigned int width, unsigned int height);

    /// @return La superficie.
    [[nodiscard]] wgpu::Surface get_surface() const;

    /// @returns La instancia de WebGPU.
    [[nodiscard]] wgpu::Instance get_instance() const;

    /// @return El dispositivo lógico.
    [[nodiscard]] wgpu::Device get_device() const;

    /// @return El mapa de render pipelines.
    [[nodiscard]] std::unordered_map<std::string, wgpu::RenderPipeline> get_pipelines() const;

    /// @returns El formato de la textura de la superficie.
    [[nodiscard]] wgpu::TextureFormat get_texture_format() const;
};