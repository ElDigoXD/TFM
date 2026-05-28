#pragma once

#include "glm_defines.h"

/// Representa una transformación en el espacio 3D, con posición, rotación y escala.
struct Transform {
    /// Posición del objeto en el espacio 3D.
    vec3 position{0.0f, 0.0f, 0.0f};
    /// Rotación del objeto en el espacio 3D, en radianes (ángulos de Euler).
    vec3 rotation{0.0f, 0.0f, 0.0f};
    /// Escala del objeto.
    vec3 scale{1.0f, 1.0f, 1.0f};

    /// Calcula la matriz de transformación a partir de la posición, rotación y escala.
    /// @note Como optimización, la matriz de transformación se puede pre-calcular y solamente actualizarla
    /// cuando se modifique la posición, rotación o escala.
    /// @return La matriz de transformación de coordenadas locales a coordenadas de mundo.
    [[nodiscard]]mat4 get_matrix() const {
        mat4 scale_matrix = glm::scale(mat4(1.0f), scale);
        mat4 translation_matrix = glm::translate(mat4(1.0f), position);
        mat4 rotation_matrix = glm::yawPitchRoll(rotation.y, rotation.x, rotation.z);

        /// El orden de multiplicación es relevante.
        return rotation_matrix * translation_matrix * scale_matrix;
    }
};