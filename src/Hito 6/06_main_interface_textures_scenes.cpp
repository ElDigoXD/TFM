#include <vector>

// Incluye la libreria para la creación de ventanas
#include <GLFW/glfw3.h> // Para GLFWwindow, glfwInit, glfwCreateWindow, glfwPollEvents, glfwWindowShouldClose

#include <imgui.h>
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>

// Incluye emscripten si se compila para web
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "global_defines.h"

#include "InputManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "game_objects/GameObject.h"
#include "Scene.h"


// Referencia a la ventana
GLFWwindow *window;

// Clase con el comportamiento de renderizado/WebGPU
Renderer renderer{};

// Clase con el comportamiento de input
InputManager *input;

// Vector de escenas
std::vector<Scene *> scenes;
int current_scene = 0;

// Para el cálculo del delta time
double prev_time, delta_time;

/// Inicializa GLFW y crea una ventana.
void init_glfw() {
    // Si no se puede inicializar la librería, aborta el programa
    if (!glfwInit())
        exit(-1);

    // Indica a la librería que no inicialice ninguna API gráfica
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // Crea la ventana con el tamaño y el título indicados
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Primera Ventana", nullptr, nullptr);
    printf("Init Window\n");
}

/// Inicializa ImGui.
void init_imgui() {
    // Crea el contexto de ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO();

    // Configura el backend de ImGui para GLFW y WebGPU
    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplWGPU_InitInfo init_info = {};
    init_info.Device = renderer.get_device().Get();
    init_info.NumFramesInFlight = 3;
    init_info.RenderTargetFormat = static_cast<WGPUTextureFormat>(renderer.get_texture_format());
    init_info.DepthStencilFormat = static_cast<WGPUTextureFormat>(wgpu::TextureFormat::Depth24Plus);
    ImGui_ImplWGPU_Init(&init_info);
}

/// Lista de comandos inmediatos de ImGui para crear la interfaz de usuario.
void ui() {

}

/// Función que se llama cada frame. No depende de la plataforma (nativo o web).
void loop() {
    // Calcula el tiempo transcurrido desde el último frame (delta time)
    delta_time = glfwGetTime() - prev_time;
    prev_time = glfwGetTime();

    // Cambia de escena al pulsar TAB
    if (input->is_key_pressed(Key::TAB)) {
        current_scene = (current_scene + 1) % scenes.size();
    }

    // Obtiene la escena actual
    Scene *scene = scenes[current_scene];

    // Actualiza la escena
    scene->update(delta_time);

    // Renderiza la escena
    scene->render(renderer);

    // Actualiza el estado del input para el siguiente frame.
    input->prepare_next_frame();
}


/// Punto de entrada del programa.
int main() {
    // Inicializa la ventana
    init_glfw();

    // Inicializa el sistema de entrada
    input = InputManager::get_instance();
    input->init(window);

    // Inicializa el renderer
    renderer.init(window);

    // Cuando la ventana cambia de tamaño, se redimensiona la superficie de renderizado
    // y se actualiza la relación de aspecto de la cámara
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        renderer.resize(width, height);
        for (auto &scene: scenes) {
            scene->camera.aspect_ratio = (float) width / (float) height;
        }
    });

    // Inicializa ImGui
    init_imgui();

    // Crea las escenas
    scenes = {
            new BunnyScene(renderer),
            new SponzaScene(renderer),
    };

    // Inicializa el tiempo del frame anterior
    prev_time = glfwGetTime();

    // En el caso de compilar para web, se hace uso del bucle de emscripten,
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(loop, 0, false);
#else
    // En el caso de nativo se hace uso del bucle de eventos de GLFW.
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        loop();
        // "Swap Chain". Muestra la superficie en la ventana
        renderer.get_surface().Present();
        renderer.get_instance().ProcessEvents();
    }
#endif
    return 0;
}