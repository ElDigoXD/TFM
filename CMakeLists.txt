#### Apartado general ####
# Version mínima de cmake con la que se puede ejecutar
cmake_minimum_required(VERSION 3.22)

# Nombre del proyecto
project(TFM)

# Versión del estándar de C++ a utilizar
set(CMAKE_CXX_STANDARD 26)

# Selecciona el hito a compilar
add_subdirectory("src/Hito 1")
add_subdirectory("src/Hito 2")
add_subdirectory("src/Hito 3")
add_subdirectory("src/Hito 4")
add_subdirectory("src/Hito 5")


#if (1 AND NOT EMSCRIPTEN)
#    # Ruta absoluta, para el proceso de desarrollo
#    target_compile_definitions(TFM PRIVATE RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
#else ()
#    # Ruta relativa, para producción
#    target_compile_definitions(TFM PRIVATE RESOURCE_DIR="./resources/")
#endif ()

#### Apartado de Dawn ####
# Marca una opción de compilación de Dawn para que descargue todas sus dependencias
set(DAWN_FETCH_DEPENDENCIES ON)

# Añade la carpeta con las fuentes de Dawn y solamente lo compila si lo pide otro target
add_subdirectory("dawn" EXCLUDE_FROM_ALL)


#### Apartado de GLM ####
# Añade la carpeta con las fuentes de GLM y solamente lo compila si lo pide otro target
add_subdirectory("glm" EXCLUDE_FROM_ALL)


#### Apartado de Emscripten ####
#if (EMSCRIPTEN)
#    # Cambia la extensión del ejecutable a ".html"
#    set_target_properties(TFM PROPERTIES SUFFIX ".html")
#    # La librería emdawnwebgpu_cpp permite el uso de WebGPU utilizando Emscripten
#    target_link_libraries(TFM PRIVATE emdawnwebgpu_cpp webgpu_glfw glm::glm)
#    # ASYNCIFY=1 permite al código síncrono C++ interactuar con código asíncrono JS.
#    # USE_GLFW=3 indica a Emscripten que utilize su implementación en JS de la API de GLFW3.
#    # preload-file permite incluir archivos en el sistema de archivos virtual de Emscripten.
#    target_link_options(TFM PRIVATE
#            "-sASYNCIFY=1"
#            "-sUSE_GLFW=3"
#            "--preload-file \"${CMAKE_CURRENT_SOURCE_DIR}/resources@resources\""
#    )
#
#else ()
#    #### Apartado de Dawn ####
#    # Establece las librerías contra las que tiene que linkar el ejecutable
#    # Dawn incluye la librería glfw, que sirve como una capa de abstracción para crear ventanas multiplataforma
#    # Las tres librerias las aporta automáticamente Dawn
#    target_link_libraries(TFM PRIVATE webgpu_dawn webgpu_glfw glfw glm::glm)
#endif ()
