# Hito 6: Interfaz, texturas y escenas

## Paso 1

Añadir imgui (https://github.com/ocornut/imgui) al proyecto. Es necesario escribir un archivo `CMakeLists.txt` en la carpeta de las fuentes `imgui` para compilar la librería, ya que no incluye cmake como sistema de compilación.

## Paso 2

Añadir el código necesario para integrar imgui en el proyecto. Inicialización en el main y renderizado en el renderer.

## Paso 3

Modificar el `InputManager` para que ignore los eventos de teclado y ratón cuando se esté usando la interfaz.

## Paso 4

Añadir soporte para texturas. Modificar el `ResourceManager` para que pueda cargar texturas a partir de imágenes. Añadir una nueva pipeline para renderizar objetos con texturas, y modificar los shaders para que muestren las texturas.

Se ha utilizado la librería stb_image (librería en un solo .h) para leer las imágenes (https://github.com/nothings/stb/blob/master/stb_image.h).

## Paso 5

Modificar el sistema de `GameObject`s para que puedan utilizar diferentes pipelines.

## Paso 6

Crear una clase `Scene` que contenga una lista de `GameObject`s y una cámara. Esta clase se encargará de gestionar las actualizaciones y el renderizado de los objetos, la cámara y la interfaz. La clase se comporta como una interfaz (clase virtual pura), la cual se implementa para crear diferentes escenas.

# Paso 7

Añadir un controlador en primera persona para la cámara que permita navegar por las escenas.