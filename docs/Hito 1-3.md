## Paso 1

Crear la carpeta del proyecto y un repositorio git.

## Paso 2

Instalar las dependencias: 
- CMake: https://cmake.org/
- Compilador C++ (en este caso MSVC): https://visualstudio.microsoft.com/vs/features/cplusplus/
- Dawn (mediante `git submodule add https://github.com/google/dawn.git`): https://github.com/google/dawn
- Emscripten SDK: https://emscripten.org/docs/getting_started/downloads.html

## Paso 3

Crear las instrucciones de build (`CMakeLists.txt`) y el punto de entrada del proyecto (`main.cpp`).

> Comprobar que el proyecto compila.

## Paso 4 (Hito 1 - Mostrar una ventana)

Añadir Dawn a `CMakeLists.txt` y crear un programa minimo que muestre una ventana en main.cpp (`01_main_hello_window.cpp`).

## Paso 5 (Hito 2 - Mostrar un triángulo)

Añadir el código para inicializar WebGPU y mostrar un triángulo (`02_main_first_triangle.cpp`).

> En el caso de que salte un error al ejecutar el programa de que falta `d3dcompiler_47.dll` o `vulkan-1.dll`,
> copiar los archivos necesarios de `C:\Windows\System32` a la carpeta del ejecutable.

## Paso 6 (Hito 3 - Mostrar un triángulo en la web)

Añadir Emscripten a `CMakeLists.txt` y modificar el código para que se pueda compilar a web (`03_main_web_triangle.cpp`).

> Hay que cambiar el toolchain de CMake para que utilice el de Emscripten. (Ver [como instalar y utilizar Emscripten SDK](Como%20instalar%20y%20utilizar%20Emscripten%20SDK.md)).
> 
> Puede ser necesario modificar el archivo `utils_emscripten.cpp` sustituyendo `#if !defined(EMSCRIPTEN)` por `#if !defined(__EMSCRIPTEN__)`.
