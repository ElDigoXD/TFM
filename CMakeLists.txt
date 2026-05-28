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
add_subdirectory("src/Hito 6")



#### Apartado de Dawn ####
# Marca una opción de compilación de Dawn para que descargue todas sus dependencias
set(DAWN_FETCH_DEPENDENCIES ON)

# Añade la carpeta con las fuentes de Dawn y solamente lo compila si lo pide otro target
add_subdirectory("dawn" EXCLUDE_FROM_ALL)


#### Apartado de GLM ####
# Añade la carpeta con las fuentes de GLM y solamente lo compila si lo pide otro target
add_subdirectory("glm" EXCLUDE_FROM_ALL)

#### Apartado de ImGui ####
# Añade la librería ImGui, que se utiliza para crear interfaces gráficas inmediatas
add_subdirectory(imgui EXCLUDE_FROM_ALL)