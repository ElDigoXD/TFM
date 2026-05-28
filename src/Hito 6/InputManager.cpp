#include "InputManager.h"
#include "imgui.h"

InputManager *InputManager::instance = nullptr;

InputManager *InputManager::get_instance() {
    // Si la instancia es nula, crea una nueva instancia
    return !instance ? instance = new InputManager() : instance;
}

void InputManager::init(GLFWwindow *l_window) {
    window = l_window;

    // Configura el callback de teclado para actualizar el estado de las teclas
    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        InputManager *input = InputManager::get_instance();
        assert(key >= 0 && key <= GLFW_KEY_LAST);
        // Los estados pueden ser GLFW_PRESS, GLFW_RELEASE o GLFW_REPEAT.
        input->key_states[key] = action == GLFW_PRESS || action == GLFW_REPEAT;
    });

    // Configura el callback de botones del ratón para actualizar el estado de los botones
    glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
        InputManager *input = InputManager::get_instance();
        assert(button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST);
        input->mouse_button_states[button] = action == GLFW_PRESS || action == GLFW_REPEAT;
    });

    // Configura el callback de movimiento del ratón para actualizar la posición actual del ratón
    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y) {
        InputManager *input = InputManager::get_instance();
        input->mouse_position = vec2{x, y};
        input->mouse_delta = input->mouse_position - input->old_mouse_position;
    });

    glfwSetScrollCallback(window, [](GLFWwindow *window, double x_delta, double y_delta) {
        InputManager *input = InputManager::get_instance();
        input->scroll_delta = vec2{x_delta, y_delta};
    });
}

void InputManager::prepare_next_frame() {
    // Actualiza el estado anterior de las teclas y botones del ratón
    old_key_states = key_states;
    old_mouse_button_states = mouse_button_states;

    // Actualiza la posición del ratón
    mouse_delta = vec2{0, 0};
    old_mouse_position = mouse_position;

    scroll_delta = vec2{0, 0};
}

bool InputManager::is_key_down(Key key) {
    if (ImGui::GetIO().WantCaptureKeyboard) return false;
    return key_states[(int) key];
}

bool InputManager::is_key_pressed(Key key) {
    if (ImGui::GetIO().WantCaptureKeyboard) return false;
    return key_states[(int) key] && !old_key_states[(int) key];
}

bool InputManager::is_key_released(Key key) {
    if (ImGui::GetIO().WantCaptureKeyboard) return false;
    return !key_states[(int) key] && old_key_states[(int) key];
}

bool InputManager::is_mouse_button_down(MouseButton button) {
    if (ImGui::GetIO().WantCaptureMouse) return false;
    return mouse_button_states[(int) button];
}

bool InputManager::is_mouse_button_pressed(MouseButton button) {
    if (ImGui::GetIO().WantCaptureMouse) return false;
    return mouse_button_states[(int) button] && !old_mouse_button_states[(int) button];
}

bool InputManager::is_mouse_button_released(MouseButton button) {
    if (ImGui::GetIO().WantCaptureMouse) return false;
    return !mouse_button_states[(int) button] && old_mouse_button_states[(int) button];
}

vec2 InputManager::get_mouse_position() {
    return mouse_position;
}

vec2 InputManager::get_mouse_delta() {
    return mouse_delta;
}

void InputManager::hide_and_stop_cursor(bool hide_and_stop) {
    glfwSetInputMode(window, GLFW_CURSOR, hide_and_stop ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    // Si el sistema soporta obtener el movimiento del ratón sin aceleración lo activa o desactiva
    // Esto es útil para cámaras
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, hide_and_stop ? GLFW_TRUE : GLFW_FALSE);
    }
}

vec2 InputManager::get_scroll_delta() {
    return scroll_delta;
}
