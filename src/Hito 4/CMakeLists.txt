# CMakeLists.txt para el Hito 4: Mostrar un modelo 3D

# Crea un ejecutable llamado TFM_04 a partir de los archivos fuente
add_executable(TFM_04
        04_main_3d_model.cpp
        GameObject.h
        ResourceManager.h
        Renderer.h
        tiny_obj_loader.h # Para cargar modelos 3D en formato OBJ. Librería bajo licencia MIT.
)

# Define la ruta en la que se encuentran los recursos (shaders, modelos, texturas, etc.)
if (1)
    # Ruta absoluta, para el proceso de desarrollo
    target_compile_definitions(TFM_04 PRIVATE RESOURCE_DIR="${CMAKE_SOURCE_DIR}/resources/")
else ()
    # Ruta relativa, para producción
    target_compile_definitions(TFM_04 PRIVATE RESOURCE_DIR="./resources/")
endif ()

if (EMSCRIPTEN) # Si se compila con Emscripten
    # Cambia la extensión del ejecutable a ".html"
    set_target_properties(TFM_04 PROPERTIES SUFFIX ".html")

    # Establece las librerías contra las que tiene que linkar el ejecutable
    # La librería emdawnwebgpu_cpp permite el uso de WebGPU utilizando Emscripten
    # glm es una librería de matemáticas para gráficos 3D
    target_link_libraries(TFM_04 PRIVATE emdawnwebgpu_cpp webgpu_glfw glm::glm)

    # ASYNCIFY=1 permite al código síncrono C++ interactuar con código asíncrono JS.
    # USE_GLFW=3 indica a Emscripten que utilize su implementación en JS de la API de GLFW3.
    # preload-file permite incluir archivos en el sistema de archivos virtual de Emscripten.
    # ALLOW_MEMORY_GROWTH permite que la memoria del programa crezca dinámicamente.
    target_link_options(TFM_04 PRIVATE
            "-sASYNCIFY=1"
            "-sUSE_GLFW=3"
            "--preload-file" "${CMAKE_SOURCE_DIR}/resources"
            "-sALLOW_MEMORY_GROWTH"
    )

else () # Si no (nativo)
    # Establece las librerías contra las que tiene que linkar el ejecutable
    # Dawn incluye la librería glfw, que sirve como una capa de abstracción para crear ventanas multiplataforma
    # Las tres primeras librerias las aporta automáticamente Dawn
    # glm es una librería de matemáticas para gráficos 3D
    target_link_libraries(TFM_04 PRIVATE webgpu_dawn webgpu_glfw glfw glm::glm)
endif ()