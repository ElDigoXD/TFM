# Hito 4: Mostrar un modelo 3D

## Paso 1

Añadir las dependencias necesarias para cargar modelos 3D:
- tiny obj loader (librería en un solo .h): https://github.com/tinyobjloader/tinyobjloader/blob/release/tiny_obj_loader.h
- GLM (OpenGL Mathematics, mediante `git submodule add https://github.com/g-truc/glm`): https://github.com/g-truc/glm

> No es estrictamente necesario el uso de estas librerías para cargar modelos 3D, pero facilitan mucho el proceso. 
> GLM se utilizará para las transformaciones de los modelos, y tiny obj loader para cargar modelos en formato OBJ.

## Paso 2

Refactorizar el código para encapsular el código relacionado con el render (devices, pipelines, buffers, etc.) en una clase (`Renderer`), y crear una clase que represente un objeto a renderizar (`GameObject`), y otra clase, estática (o singleton), como gestor de recursos (`ResourceManager`) para cargar los modelos 3D. Además de cargar los modelos 3D a buffers, también podrá cargar shaders WGSL.

Modificar el código de los shaders para que muestren los modelos 3D con las normales como color.

## Paso 3

En `main.cpp` crear un `GameObject`, asignarle el buffer de un objeto 3D cargado mediante `ResourceManager` y, en el bucle del programa, renderizar el objeto utilizando el `Renderer`. Para que el objeto se renderize correctamente en 3D, es necesario añadir un buffer de profundidad a la pipeline.

> Si el modelo 3D no tiene normales, se calculan a partir de las posiciones de los vértices utilizando la normal del triángulo.