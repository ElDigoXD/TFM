# CMakeLists.txt para el Hito 2: Mostrar un triángulo.

# Crea un ejecutable llamado TFM_02 a partir del archivo fuente "02_main_first_triangle.cpp"
add_executable(TFM_02
        "02_main_first_triangle.cpp"
)

# Dawn incluye la librería glfw, que sirve como una capa de abstracción para crear ventanas multiplataforma
# Linkea con la librería
target_link_libraries(TFM_02 PRIVATE webgpu_dawn webgpu_glfw glfw)