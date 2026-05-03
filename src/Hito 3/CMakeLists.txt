# CMakeLists.txt para el Hito 3: Mostrar un triángulo en la web.

# Crea un ejecutable llamado TFM_03 a partir del archivo fuente "03_main_web_triangle.cpp"
add_executable(TFM_03
        "03_main_web_triangle.cpp"
)


if (EMSCRIPTEN) # Si se compila con Emscripten
    # Cambia la extensión del ejecutable a ".html"
    set_target_properties(TFM_03 PROPERTIES SUFFIX ".html")

    # Establece las librerías contra las que tiene que linkar el ejecutable
    # La librería emdawnwebgpu_cpp permite el uso de WebGPU utilizando Emscripten
    target_link_libraries(TFM_03 PRIVATE emdawnwebgpu_cpp webgpu_glfw)

    # ASYNCIFY=1 permite al código síncrono C++ interactuar con código asíncrono JS.
    # USE_GLFW=3 indica a Emscripten que utilize su implementación en JS de la API de GLFW3.
    target_link_options(TFM_03 PRIVATE
            "-sASYNCIFY=1"
            "-sUSE_GLFW=3"
    )

else () # Si no (nativo)
    # Establece las librerías contra las que tiene que linkar el ejecutable
    # Dawn incluye la librería glfw, que sirve como una capa de abstracción para crear ventanas multiplataforma
    # Las tres librerias las aporta automáticamente Dawn
    target_link_libraries(TFM_03 PRIVATE webgpu_dawn webgpu_glfw glfw)
endif ()