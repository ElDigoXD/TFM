#include "Camera.h"

#include "InputManager.h"

void Camera::update(float delta_time) {
    // Utiliza el controlador esférico para actualizar la posición de la cámara
    update_spherical_controller(delta_time);
}

mat4 Camera::get_view_matrix() const {
    // Calcula la matriz de vista utilizando la función lookAt de GLM
    return glm::lookAt(look_from, look_at, vec3{0, 1, 0});
}

mat4 Camera::get_projection_matrix() const {
    // Calcula la matriz de proyección utilizando la función perspective de GLM
    // El campo de visión vertical se calcula a partir de la distancia focal
    return glm::perspective(2 * glm::atan(1 / focal_length), aspect_ratio, near_plane, far_plane);
}

void Camera::update_spherical_controller(float delta_time) {
    InputManager *input = InputManager::get_instance();

    // Si se pulsa el botón izquierdo del ratón, oculta el cursor
    if (input->is_mouse_button_pressed(MouseButton::LEFT)) {
        input->hide_and_stop_cursor(true);
    }

    // Si el botón izquierdo del ratón está pulsado, rota la cámara en función del movimiento del ratón
    if (input->is_mouse_button_down(MouseButton::LEFT)) {
        vec2 delta = input->get_mouse_delta();
        if (delta != vec2{0}) {
            float sensitivity = .01f;
            // Rotación respondiendo al movimiento en el eje x del ratón
            auto new_look_from = rodrigues_rotation(look_at - look_from,
                                                    vec3{0, 1, 0},
                                                    delta.x * sensitivity);
            // Rotación respondiendo al movimiento en el eje y del ratón
            new_look_from = rodrigues_rotation(new_look_from,
                                               glm::normalize(glm::cross(look_at - look_from, vec3{0, 1, 0})),
                                               -delta.y * sensitivity);
            look_from = look_at - new_look_from;
        }
    }

    // Si se suelta el botón izquierdo del ratón, vuelve a mostrar el cursor
    if (input->is_mouse_button_released(MouseButton::LEFT)) {
        input->hide_and_stop_cursor(false);
    }

    // Si se mueve la rueda del ratón, ajusta la distancia focal de la cámara
    auto scroll_delta = input->get_scroll_delta();
    if (scroll_delta != vec2{0}) {
        float sensitivity = 1;
        // Ajusta la distancia focal, linearizando el efecto de zoom.
        focal_length += scroll_delta.y * focal_length * 20 / 100 * sensitivity;
        focal_length = glm::max(focal_length, 1.f);
    }
}

vec3 Camera::rodrigues_rotation(vec3 v, vec3 k, float theta) const {
    float cos_theta = glm::cos(theta);
    float sin_theta = glm::sin(theta);
    return v * cos_theta + glm::cross(k, v) * sin_theta + k * glm::dot(k, v) * (1 - cos_theta);
}
