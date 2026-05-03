#include <iostream>
#include <vector>

// Incluye la libreria para la creación de ventanas
#include <GLFW/glfw3.h>

// Incluye la librería WebGPU
#include <webgpu/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <webgpu/webgpu_glfw.h>

// Incluye emscripten si se compila para web
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

// Incluye la librería para cargar modelos 3D y su implementación
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// Incluye la librería OpenGL Mathematics
#include "glm/glm.hpp"

typedef uint32_t u32;

// Tamaño de la ventana
const u32 WINDOW_WIDTH = 600;
const u32 WINDOW_HEIGHT = 400;

/// El formato de los vértices, con posición, normal y coordenadas UV
struct VertexAttributes {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

#include "ResourceManager.h"
std::map<const std::filesystem::path, wgpu::Buffer> ResourceManager::vertex_buffers{};

#include "GameObject.h"
#include "Renderer.h"

// Vector de objetos a renderizar
std::vector<GameObject> game_objects;

// Referencia a la ventana
GLFWwindow *window;

// Clase con el comportamiento de renderizado/WebGPU
Renderer renderer{};

/// Inicializa GLFW y crea una ventana.
void init_glfw() {
    // Si no se puede inicializar la librería, aborta el programa
    if (!glfwInit())
        exit(-1);

    // Indica a la librería que no inicialice ninguna API gráfica
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Crea la ventana con el tamaño y el título indicados
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Primera Ventana", nullptr, nullptr);
    printf("Init Window\n");
}

int main() {
    // Inicializa la ventana
    init_glfw();
    // Inicializa el renderer
    renderer.init(window);

    // Crea un objeto con el modelo del conejo de Stanford
    auto game_object_1 = GameObject();
    game_object_1.set_vertex_buffer(ResourceManager::get_vertex_buffer(RESOURCE_DIR "stanford-bunny.obj",
                                                                    renderer.get_device()));
    game_objects.emplace_back(game_object_1);

    // En el caso de compilar para web, se hace uso del bucle de emscripten,
#ifdef __EMSCRIPTEN__
    auto render_loop = []() { renderer.render(game_objects); };
    emscripten_set_main_loop(render_loop, 0, false);
#else
    // Bucle de eventos
    double prev_time = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float delta_time = glfwGetTime() - prev_time;
        prev_time = glfwGetTime();

        glfwPollEvents();
        // Actualiza cada objeto
        for (auto &game_object : game_objects) {
            game_object.update(delta_time);
        }

        // Renderiza los objetos
        renderer.render(game_objects);
        // "Swap Chain". Muestra la superficie en la ventana
        renderer.get_surface().Present();
        renderer.get_instance().ProcessEvents();
    }
#endif
    return 0;
}