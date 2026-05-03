# CMakeLists.txt para el Hito 1: Mostrar una ventana.

# Crea un ejecutable llamado TFM_01 a partir del archivo fuente "01_main_hello_window.cpp"
add_executable(TFM_01
        "01_main_hello_window.cpp"
)

# Dawn incluye la librería glfw, que sirve como una capa de abstracción para crear ventanas multiplataforma
# Linkea con la librería
target_link_libraries(TFM_01 PRIVATE glfw)