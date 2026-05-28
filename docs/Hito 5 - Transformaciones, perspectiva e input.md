# Hito 5: Transformaciones, perspectiva e input

## Paso 1

Crear un sistema de transformaciones para los objetos 3D, utilizando GLM para las operaciones de matrices. Cada `GameObject` tendrá una matriz de transformación, formada por translación, rotación y escala.

## Paso 2

Crear una clase que represente una cámara. Esta clase tendrá una matriz de vista y una matriz de proyección. La matriz de vista se calculará a partir de su posición y orientación. La matriz de proyección se calculará a partir de su campo de visión vertical (vFOV), su relación de aspecto y los planos de clip (cortan todo lo que se encuentra fuera de ellos).

> Con estas tres matrices se puede cambiar entre distintos espacios de coordenadas: locales (o de modelo), globales (o de mundo), de vista (o de cámara), y de clip.

## Paso 3

Modificar los shaders para que utilicen las matrices de transformación de los objetos y las de la cámara para renderizar los objetos 3D con perspectiva.

> Hay que tener en cuenta que el orden de multiplicación de las matrices es importante.

## Paso 4

Crear un sistema de input. La funcionalidad que no ofrece GLFW de manera sencilla, como mouse delta (diferencia de ratón), scroll delta (diferencia de rueda), o detectar si una tecla ha sido pulsada o soltada durante el frame actual; se encapsula en la clase `InputManager`. Esta clase se actualiza cada frame, y se encarga de almacenar el estado del teclado y del ratón.

## Paso 5

Utilizar el nuevo sistema de input para mover la cámara en su método `update()`. El sistema de control implementado es esférico (rotando alrededor de un punto), con zoom.