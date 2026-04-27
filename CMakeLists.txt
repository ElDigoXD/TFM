#### Apartado general ####
# Version mínima de cmake con la que se puede ejecutar
cmake_minimum_required(VERSION 3.22)

# Nombre del proyecto
project(TFM)

# Versión del estándar de C++ a utilizar
set(CMAKE_CXX_STANDARD 26)

# Añade un ejecutable al proyecto
add_executable(TFM "src/main.cpp")


#### Apartado de Dawn ####
# Marca una opción de compilación de Dawn para que descargue todas sus dependencias
set(DAWN_FETCH_DEPENDENCIES ON)


# Añade la carpeta con las fuentes de Dawn y solamente lo compila si lo pide otro target
add_subdirectory("dawn" EXCLUDE_FROM_ALL)



#### Apartado de Emscripten ####
if (EMSCRIPTEN)
    # Cambia la extensión del ejecutable a ".html"
    set_target_properties(TFM PROPERTIES SUFFIX ".html")
    # La librería emdawnwebgpu_cpp permite el uso de WebGPU utilizando Emscripten
    target_link_libraries(TFM PRIVATE emdawnwebgpu_cpp webgpu_glfw)
    # ASYNCIFY=1 permite al código síncrono C++ interactuar con código asíncrono JS.
    # USE_GLFW=3 indica a Emscripten que utilize su implementación en JS de la API de GLFW3.
    target_link_options(TFM PRIVATE "-sASYNCIFY=1" "-sUSE_GLFW=3")
else ()
    #### Apartado de Dawn ####
    # Establece las librerías contra las que tiene que linkar el ejecutable
    # Dawn incluye la librería glfw, que sirve como una capa de abstracción para crear ventanas multiplataforma
    # Las tres librerias las aporta automáticamente Dawn
    target_link_libraries(TFM PRIVATE webgpu_dawn webgpu_glfw glfw)
endif()