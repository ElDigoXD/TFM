#pragma once

#include <bitset>

#include "GLFW/glfw3.h"

#include "glm_defines.h"

/// Enumeración de teclas, con los mismos valores que GLFW.
enum class Key {
    SPACE = GLFW_KEY_SPACE,
    APOSTROPHE = GLFW_KEY_APOSTROPHE,
    COMMA = GLFW_KEY_COMMA,
    MINUS = GLFW_KEY_MINUS,
    PERIOD = GLFW_KEY_PERIOD,
    SLASH = GLFW_KEY_SLASH,
    _0 = GLFW_KEY_0,
    _1 = GLFW_KEY_1,
    _2 = GLFW_KEY_2,
    _3 = GLFW_KEY_3,
    _4 = GLFW_KEY_4,
    _5 = GLFW_KEY_5,
    _6 = GLFW_KEY_6,
    _7 = GLFW_KEY_7,
    _8 = GLFW_KEY_8,
    _9 = GLFW_KEY_9,
    SEMICOLON = GLFW_KEY_SEMICOLON,
    EQUAL = GLFW_KEY_EQUAL,
    A = GLFW_KEY_A,
    B = GLFW_KEY_B,
    C = GLFW_KEY_C,
    D = GLFW_KEY_D,
    E = GLFW_KEY_E,
    F = GLFW_KEY_F,
    G = GLFW_KEY_G,
    H = GLFW_KEY_H,
    I = GLFW_KEY_I,
    J = GLFW_KEY_J,
    K = GLFW_KEY_K,
    L = GLFW_KEY_L,
    M = GLFW_KEY_M,
    N = GLFW_KEY_N,
    O = GLFW_KEY_O,
    P = GLFW_KEY_P,
    Q = GLFW_KEY_Q,
    R = GLFW_KEY_R,
    S = GLFW_KEY_S,
    T = GLFW_KEY_T,
    U = GLFW_KEY_U,
    V = GLFW_KEY_V,
    W = GLFW_KEY_W,
    X = GLFW_KEY_X,
    Y = GLFW_KEY_Y,
    Z = GLFW_KEY_Z,
    LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET,
    BACKSLASH = GLFW_KEY_BACKSLASH,
    RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,
    GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,
    WORLD_1 = GLFW_KEY_WORLD_1,
    WORLD_2 = GLFW_KEY_WORLD_2,
    ESCAPE = GLFW_KEY_ESCAPE,
    ENTER = GLFW_KEY_ENTER,
    TAB = GLFW_KEY_TAB,
    BACKSPACE = GLFW_KEY_BACKSPACE,
    INSERT = GLFW_KEY_INSERT,
    DELETE_ = GLFW_KEY_DELETE,
    RIGHT = GLFW_KEY_RIGHT,
    LEFT = GLFW_KEY_LEFT,
    DOWN = GLFW_KEY_DOWN,
    UP = GLFW_KEY_UP,
    PAGE_UP = GLFW_KEY_PAGE_UP,
    PAGE_DOWN = GLFW_KEY_PAGE_DOWN,
    HOME = GLFW_KEY_HOME,
    END = GLFW_KEY_END,
    CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
    SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK,
    NUM_LOCK = GLFW_KEY_NUM_LOCK,
    PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,
    PAUSE = GLFW_KEY_PAUSE,
    F1 = GLFW_KEY_F1,
    F2 = GLFW_KEY_F2,
    F3 = GLFW_KEY_F3,
    F4 = GLFW_KEY_F4,
    F5 = GLFW_KEY_F5,
    F6 = GLFW_KEY_F6,
    F7 = GLFW_KEY_F7,
    F8 = GLFW_KEY_F8,
    F9 = GLFW_KEY_F9,
    F10 = GLFW_KEY_F10,
    F11 = GLFW_KEY_F11,
    F12 = GLFW_KEY_F12,
    F13 = GLFW_KEY_F13,
    F14 = GLFW_KEY_F14,
    F15 = GLFW_KEY_F15,
    F16 = GLFW_KEY_F16,
    F17 = GLFW_KEY_F17,
    F18 = GLFW_KEY_F18,
    F19 = GLFW_KEY_F19,
    F20 = GLFW_KEY_F20,
    F21 = GLFW_KEY_F21,
    F22 = GLFW_KEY_F22,
    F23 = GLFW_KEY_F23,
    F24 = GLFW_KEY_F24,
    F25 = GLFW_KEY_F25,
    KP_0 = GLFW_KEY_KP_0,
    KP_1 = GLFW_KEY_KP_1,
    KP_2 = GLFW_KEY_KP_2,
    KP_3 = GLFW_KEY_KP_3,
    KP_4 = GLFW_KEY_KP_4,
    KP_5 = GLFW_KEY_KP_5,
    KP_6 = GLFW_KEY_KP_6,
    KP_7 = GLFW_KEY_KP_7,
    KP_8 = GLFW_KEY_KP_8,
    KP_9 = GLFW_KEY_KP_9,
    KP_DECIMAL = GLFW_KEY_KP_DECIMAL,
    KP_DIVIDE = GLFW_KEY_KP_DIVIDE,
    KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY,
    KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT,
    KP_ADD = GLFW_KEY_KP_ADD,
    KP_ENTER = GLFW_KEY_KP_ENTER,
    KP_EQUAL = GLFW_KEY_KP_EQUAL,
    LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
    LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL,
    LEFT_ALT = GLFW_KEY_LEFT_ALT,
    LEFT_SUPER = GLFW_KEY_LEFT_SUPER,
    RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,
    RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,
    RIGHT_ALT = GLFW_KEY_RIGHT_ALT,
    RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER,
    MENU = GLFW_KEY_MENU,
};

/// Enumeración de los botones del ratón, con los mismos valores que GLFW.
enum class MouseButton {
    LEFT = GLFW_MOUSE_BUTTON_LEFT,
    RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
    MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
    _1 = GLFW_MOUSE_BUTTON_1,
    _2 = GLFW_MOUSE_BUTTON_2,
    _3 = GLFW_MOUSE_BUTTON_3,
    _4 = GLFW_MOUSE_BUTTON_4,
    _5 = GLFW_MOUSE_BUTTON_5,
    _6 = GLFW_MOUSE_BUTTON_6,
    _7 = GLFW_MOUSE_BUTTON_7,
    _8 = GLFW_MOUSE_BUTTON_8,
};

/// Clase encargada de gestionar la entrada de teclado y ratón, utilizando GLFW como backend.
class InputManager {
private:
    /// Instancia singleton de la clase.
    static InputManager *instance;
    /// Ventana de GLFW para la que se gestionará la entrada.
    GLFWwindow *window;

    /// Estado actual de las teclas.
    std::bitset<GLFW_KEY_LAST + 1> key_states;
    /// Estado del frame anterior de las teclas.
    std::bitset<GLFW_KEY_LAST + 1> old_key_states;

    /// Estado actual de los botones del ratón.
    std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> mouse_button_states;
    /// Estado del frame anterior de los botones del ratón.
    std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> old_mouse_button_states;

    /// Posición actual del ratón.
    vec2 mouse_position;
    /// Posición del ratón en el frame anterior.
    vec2 old_mouse_position;
    /// Diferencia de posición del ratón entre el frame actual y el anterior.
    vec2 mouse_delta;
    /// Diferencia de scroll del ratón entre el frame actual y el anterior.
    vec2 scroll_delta;

public:

    /// Devuelve el singleton de la clase.
    /// @return Puntero a la instancia del InputManager.
    static InputManager *get_instance();

    /// Inicializa el InputManager con la ventana de GLFW especificada.
    /// Establece los callbacks de GLFW para gestionar la entrada de teclado y ratón.
    void init(GLFWwindow *l_window);

    /// Prepara el InputManager para el siguiente frame, actualizando los estados de las teclas, botones del ratón, posición del ratón y scroll.
    void prepare_next_frame();

    /// Comprueba si la tecla especificada está actualmente presionada.
    /// @param key Tecla a comprobar.
    /// @return Si la tecla está presionada.
    bool is_key_down(Key key);

    /// Comprueba si la tecla especificada ha sido presionada en el frame actual.
    /// @param key Tecla a comprobar.
    /// @return Si la tecla ha sido presionada en el frame actual.
    bool is_key_pressed(Key key);

    /// Comprueba si la tecla especificada ha sido soltada en el frame actual.
    /// @param key Tecla a comprobar.
    /// @return Si la tecla ha sido soltada en el frame actual.
    bool is_key_released(Key key);

    /// Comprueba si el botón del ratón especificado está actualmente presionado.
    /// @param button Botón del ratón a comprobar.
    /// @return Si el botón del ratón está presionado.
    bool is_mouse_button_down(MouseButton button);

    /// Comprueba si el botón del ratón especificado ha sido presionado en el frame actual.
    /// @param button Botón del ratón a comprobar.
    /// @return Si el botón del ratón ha sido presionado en el frame actual.
    bool is_mouse_button_pressed(MouseButton button);

    /// Comprueba si el botón del ratón especificado ha sido soltado en el frame actual.
    /// @param button Botón del ratón a comprobar.
    /// @return Si el botón del ratón ha sido soltado en el frame actual.
    bool is_mouse_button_released(MouseButton button);

    /// Devuelve la posición actual del ratón.
    /// @return Posición actual del ratón.
    vec2 get_mouse_position();

    /// Devuelve la diferencia de posición del ratón entre el frame actual y el anterior.
    /// @return Diferencia de posición del ratón entre el frame actual y el anterior.
    vec2 get_mouse_delta();

    /// Devuelve la diferencia de scroll del ratón entre el frame actual y el anterior.
    /// @return Diferencia de scroll del ratón entre el frame actual y el anterior.
    vec2 get_scroll_delta();

    /// Establece si el cursor del ratón debe estar oculto y parado, o visible y libre para moverse.
    /// Elimina la aceleración del ratón cuando está oculto (y el sistema lo soporta).
    /// @param hide_and_stop Nuevo estado.
    void hide_and_stop_cursor(bool hide_and_stop);
};