// Incluye la librería para la creación de ventanas
#include <GLFW/glfw3.h>

typedef uint32_t u32;

// Tamaño de la ventana
const u32 WINDOW_WIDTH = 600;
const u32 WINDOW_HEIGHT = 400;

int main()
{
    // Si no se puede inicializar la librería, aborta el programa
    if (!glfwInit())
        return -1;

    // Indica a la librería que no inicialice ninguna API gráfica
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Crea la ventana con el tamaño y el título indicados
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Primera Ventana", nullptr, nullptr);

    // Bucle de eventos
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
    
    return 0;
}