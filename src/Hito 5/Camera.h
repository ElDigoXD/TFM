#pragma once

#include "glm_defines.h"

/// Representa una cámara con su posición, orientación y parámetros de proyección.
class Camera {
private:
public:
    /// Distancia desde la posición de la cámara (look_from) hasta el plano de proyección.
    /// Afecta el campo de visión de la cámara.
    float focal_length;
    /// Relación de aspecto de la cámara.
    float aspect_ratio;
    /// Distancia al plano cercano de la cámara.
    /// Las partes de la escena que estén más cerca que esta distancia no serán visibles.
    float near_plane;
    /// Distancia al plano lejano de la cámara.
    /// Las partes de la escena que estén más lejos que esta distancia no serán visibles.
    float far_plane;
    /// Posición de la cámara en el espacio 3D.
    vec3 look_from;
    /// Punto al que la cámara está apuntando en el espacio 3D.
    vec3 look_at;

    /// Función que se ejecuta cada frame.
    /// Se encarga de actualizar la posición y orientación según el input del usuario.
    /// @param delta_time Tiempo transcurrido desde el último frame, en segundos.
    void update(float delta_time);

    /// Controla la rotación esférica de la cámara alrededor del punto look_at, utilizando el input del usuario.
    /// @param delta_time Tiempo transcurrido desde el último frame, en segundos.
    void update_spherical_controller(float delta_time);

    /// Devuelve la matriz de vista de la cámara, calculada a partir de su posición (look_from) y su objetivo (look_at).
    /// Se utiliza para transformar las coordenadas de mundo a coordenadas de cámara.
    /// @return La matriz de vista de la cámara.
    [[nodiscard]] mat4 get_view_matrix() const;

    /// Devuelve la matriz de proyección de la cámara, calculada a partir de sus parámetros de proyección
    /// (focal_length, aspect_ratio, near_plane, far_plane).
    /// Se utiliza para transformar las coordenadas de cámara a coordenadas de clip (aplicando la perspectiva).
    /// En el caso de WebGPU, el volumen de clip es x: (-1, 1), y: (-1, 1), z: (0, 1).
    /// @return La matriz de proyección de la cámara.
    [[nodiscard]] mat4 get_projection_matrix() const;

private:
    /// Rotación de un vector v alrededor de un eje k por un ángulo theta, utilizando la fórmula de Rodrigues.
    /// @param v El vector a rotar.
    /// @param k El eje de rotación (normalizado).
    /// @param theta El ángulo de rotación en radianes.
    /// @return El vector resultante de rotar v alrededor de k theta radianes.
    /// @see https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
    vec3 rodrigues_rotation(vec3 v, vec3 k, float theta) const;
};