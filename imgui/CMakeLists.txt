# CMakeLists.txt para la librería ImGui

# Crea una librería estática llamada imgui
add_library(imgui STATIC
        # Implementación de los backends de ImGui para WebGPU y GLFW
        # Estos backends son los que se utilizan en el proyecto
        backends/imgui_impl_wgpu.cpp
        backends/imgui_impl_wgpu.h
        backends/imgui_impl_glfw.cpp
        backends/imgui_impl_glfw.h

        # Archivos principales de ImGui
        imconfig.h
        imgui.cpp
        imgui.h
        imgui_draw.cpp
        imgui_internal.h
        imgui_tables.cpp
        imgui_widgets.cpp
        imstb_rectpack.h
        imstb_textedit.h
        imstb_truetype.h

        # Archivos que añaden el uso de std::string a ImGui
        misc/cpp/imgui_stdlib.cpp
        misc/cpp/imgui_stdlib.h
)
target_compile_definitions(imgui PUBLIC "IMGUI_IMPL_WEBGPU_BACKEND_DAWN")

target_include_directories(imgui PUBLIC .)

# Linkea contra las librerías de WebGPU y GLFW
if (EMSCRIPTEN) # Si se compila con Emscripten
    target_link_libraries(imgui PUBLIC emdawnwebgpu_cpp webgpu_glfw glfw)
else () # Si no (nativo)
    target_link_libraries(imgui PUBLIC webgpu_dawn webgpu_glfw glfw)
endif ()