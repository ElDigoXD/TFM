#include <vector>

// Incluye la libreria para la creación de ventanas
#include <GLFW/glfw3.h> // Para GLFWwindow, glfwInit, glfwCreateWindow, glfwPollEvents, glfwWindowShouldClose

// Incluye emscripten si se compila para web
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "global_defines.h"

#include "Camera.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Renderer.h"
#include "ResourceManager.h"


// Referencia a la ventana
GLFWwindow *window;

// Clase con el comportamiento de renderizado/WebGPU
Renderer renderer{};

// Clase con el comportamiento de input
InputManager *input;

// Clase que representa la cámara
Camera camera;

// Vector de objetos a renderizar
std::vector<GameObject> game_objects;

// Para el cálculo del delta time
double prev_time;


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


/// Función que se llama cada frame. No depende de la plataforma (nativo o web).
void loop() {
    // Calcula el tiempo transcurrido desde el último frame (delta time)
    float delta_time = glfwGetTime() - prev_time;
    prev_time = glfwGetTime();

    // Actualiza la cámara
    camera.update(delta_time);

    // Actualiza cada objeto
    for (auto &game_object: game_objects) {
        game_object.update(delta_time);
    }

    // Renderiza los objetos desde la perspectiva de la cámara
    renderer.render(camera, game_objects);

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

    // Crea la cámara
    camera = Camera{
            .focal_length = 10.0f,
            .aspect_ratio = (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
            .near_plane = 0.1f,
            .far_plane = 100.0f,
            .look_from = vec3{0, 0, -5},
            .look_at = vec3{0, 0, 0},
    };

    // Crea un objeto con el modelo del conejo de Stanford
    auto game_object_1 = GameObject();
    game_object_1.vertex_buffer =
            ResourceManager::get_vertex_buffer(RESOURCE_DIR "stanford-bunny.obj", renderer.get_device());
    game_object_1.create_bind_group(renderer);

    // Crea otro objeto con el modelo del conejo de Stanford, pero rotado 180 grados
    auto game_object_2 = GameObject();
    game_object_2.vertex_buffer =
            ResourceManager::get_vertex_buffer(RESOURCE_DIR "stanford-bunny.obj", renderer.get_device());
    game_object_2.transform.rotation = vec3{0, glm::pi<float>(), 0};
    game_object_2.create_bind_group(renderer);

    // Crea un objeto con el modelo de una pirámide, escalado en Y para que sea más plano y situado debajo de los conejos
    auto game_object_3 = GameObject();
    game_object_3.vertex_buffer =
            ResourceManager::get_vertex_buffer(RESOURCE_DIR "pyramid.obj", renderer.get_device());
    game_object_3.transform.position = vec3{0, -0.4, 0};
    game_object_3.transform.scale = vec3{1, 0.5f, 1};
    game_object_3.create_bind_group(renderer);

    // Añade los objetos a la lista de objetos a renderizar
    game_objects.emplace_back(game_object_1);
    game_objects.emplace_back(game_object_2);
    game_objects.emplace_back(game_object_3);

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