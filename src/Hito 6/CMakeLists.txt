# CMakeLists.txt para el Hito 6: Interfaz, texturas y escenas

# Crea un ejecutable llamado TFM_06 a partir de los archivos fuente
add_executable(TFM_06
        06_main_interface_textures_scenes.cpp
        Camera.cpp
        Camera.h
        InputManager.cpp
        InputManager.h
        Renderer.cpp
        Renderer.h
        ResourceManager.cpp
        ResourceManager.h
        Transform.h
        glm_defines.h
        global_defines.h
        game_objects/BoatObject.h
        game_objects/BunnyObject.h
        game_objects/GameObject.h
        game_objects/SponzaObject.h
        game_objects/StaticGenericMeshObject.h
        # Librerias "header-only"
        tiny_obj_loader.h # Para cargar modelos 3D en formato OBJ. Librería bajo licencia MIT.
        stb_image.h # Para cargar imágenes. Librería bajo licencia MIT o Dominio Público.
)

# Define la ruta en la que se encuentran los recursos (shaders, modelos, texturas, etc.)
if (1)
    # Ruta absoluta, para el proceso de desarrollo
    target_compile_definitions(TFM_06 PRIVATE RESOURCE_DIR="${CMAKE_SOURCE_DIR}/resources/")
else ()
    # Ruta relativa, para producción
    target_compile_definitions(TFM_06 PRIVATE RESOURCE_DIR="./resources/")
endif ()

if (EMSCRIPTEN) # Si se compila con Emscripten
    # Cambia la extensión del ejecutable a ".html"
    set_target_properties(TFM_06 PROPERTIES SUFFIX ".html")

    # Establece las librerías contra las que tiene que linkar el ejecutable
    # La librería emdawnwebgpu_cpp permite el uso de WebGPU utilizando Emscripten
    # glm es una librería de matemáticas para gráficos 3D
    # imgui es una librería para crear interfaces gráficas inmediatas
    target_link_libraries(TFM_06 PRIVATE emdawnwebgpu_cpp webgpu_glfw glm::glm imgui)

    # ASYNCIFY=1 permite al código síncrono C++ interactuar con código asíncrono JS.
    # USE_GLFW=3 indica a Emscripten que utilize su implementación en JS de la API de GLFW3.
    # preload-file permite incluir archivos en el sistema de archivos virtual de Emscripten.
    # ALLOW_MEMORY_GROWTH permite que la memoria del programa crezca dinámicamente.
    target_link_options(TFM_06 PRIVATE
            "-sASYNCIFY=1"
            "-sUSE_GLFW=3"
            "-sALLOW_MEMORY_GROWTH"
            "-sSINGLE_FILE=1"
            "-sSINGLE_FILE_BINARY_ENCODE=1"
            "--preload-file" "${CMAKE_SOURCE_DIR}/resources"
            #"--shell-file" "${CMAKE_SOURCE_DIR}/resources/shell_minimal.html"
    )

else () # Si no (nativo)
    # Establece las librerías contra las que tiene que linkar el ejecutable
    # Dawn incluye la librería glfw, que sirve como una capa de abstracción para crear ventanas multiplataforma
    # Las tres primeras librerias las aporta automáticamente Dawn
    # glm es una librería de matemáticas para gráficos 3D
    # imgui es una librería para crear interfaces gráficas inmediatas
    target_link_libraries(TFM_06 PRIVATE webgpu_dawn webgpu_glfw glfw glm::glm imgui)
endif ()