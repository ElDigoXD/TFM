## Cómo instalar Emscripten SDK
Descargar Emscripten SDK 
```shell
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
```
Los archivos se descargarán en la carpeta `emsdk/upstream`.

## Cómo utilizar Emscripten SDK

Existen dos formas de utilizar el SDK:
- Activar el SDK para la sesión de la terminal actual y utilizar `emcmake`:

```shell
# Activa el SDK
./emsdk activate latest

# Desde el directorio del proyecto
# Crea la configuración del build a partir del archivo CMakeLists.txt:
emcmake cmake -B build-web
```

- O utilizar cmake indicando la ruta del toolchain de Emscripten:

```shell
# Crea la configuración del build a partir del archivo CMakeLists.txt:
# en mi caso: C:\Users\Diego\git\emsdk\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake
cmake -B build-web -DCMAKE_TOOLCHAIN_FILE=C:\Users\Diego\git\emsdk\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake
```