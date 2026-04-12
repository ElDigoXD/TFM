#include <iostream>

// Incluye la libreria para la creación de ventanas
#include <GLFW/glfw3.h>

// Incluye la librería WebGPU
#include <webgpu/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <webgpu/webgpu_glfw.h>

// Incluye emscripten si se compila para web
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

typedef uint32_t u32;

// Tamaño de la ventana
const u32 WINDOW_WIDTH = 600;
const u32 WINDOW_HEIGHT = 400;

// Referencia a la ventana
GLFWwindow *window;

// Representa una instancia de WebGPU
wgpu::Instance instance;

// TODO: documentar
wgpu::Adapter adapter;
wgpu::Device device;

// Representa la superficie donde dibujar los gráficos
wgpu::Surface surface;
// todo: documentar
wgpu::TextureFormat texture_format;

// Representa la pipeline gráfica
wgpu::RenderPipeline pipeline;

// Shader basico con un triángulo rojo
// TODO: comentar shader
const char shader_code[] = R"(
    @vertex fn vertex_main(@builtin(vertex_index) i : u32) ->
      @builtin(position) vec4f {
        const pos = array(vec2f(0, 1), vec2f(-1, -1), vec2f(1, -1));
        return vec4f(pos[i], 0, 1);
    }
    @fragment fn fragment_main() -> @location(0) vec4f {
        return vec4f(1, 0, 0, 1);
    }
)";

/// Configura la superficie con el dispositivo, formato de textura y tamaño.
void configure_surface() {
    // Extrae el formato de textura a partir de las capabilities
    wgpu::SurfaceCapabilities capabilities;
    surface.GetCapabilities(adapter, &capabilities);
    texture_format = capabilities.formats[0];

    // Configura la superficie
    wgpu::SurfaceConfiguration config{
            .device = device,
            .format = texture_format,
            .width = WINDOW_WIDTH,
            .height = WINDOW_HEIGHT,
    };
    surface.Configure(&config);
}

// TODO: documentar
void create_render_pipeline() {
    // The source code of the shader writen in WGSL
    wgpu::ShaderSourceWGSL wgsl{{.code = shader_code}};

    wgpu::ShaderModuleDescriptor shader_module_descriptor{.nextInChain = &wgsl};
    wgpu::ShaderModule shader_module = device.CreateShaderModule(&shader_module_descriptor);

    wgpu::ColorTargetState color_target_state{.format = texture_format};

    wgpu::FragmentState fragment_state{
            .module = shader_module,
            .targetCount = 1,
            .targets = &color_target_state,
    };

    wgpu::RenderPipelineDescriptor render_pipeline_descriptor{
            .vertex = {.module = shader_module},
            .fragment = &fragment_state,
    };

    pipeline = device.CreateRenderPipeline(&render_pipeline_descriptor);
}

void init_graphics() {
    configure_surface();
    create_render_pipeline();
}

// TODO: documentar
void render() {
    wgpu::SurfaceTexture surface_texture;
    surface.GetCurrentTexture(&surface_texture);
    wgpu::RenderPassColorAttachment attachment{
            .view = surface_texture.texture.CreateView(),
            .loadOp = wgpu::LoadOp::Clear,
            .storeOp = wgpu::StoreOp::Store,
    };

    wgpu::RenderPassDescriptor renderpass{
            .colorAttachmentCount = 1,
            .colorAttachments = &attachment,
    };

    wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass);
    pass.SetPipeline(pipeline);
    pass.Draw(3);
    pass.End();
    wgpu::CommandBuffer commands = encoder.Finish();
    device.GetQueue().Submit(1, &commands);
}

void init_webgpu() {
    // Crea la instancia, asegurandose que disponga de la caracteristica TimedWaitAny
    static const wgpu::InstanceFeatureName timed_wait_any = wgpu::InstanceFeatureName::TimedWaitAny;
    wgpu::InstanceDescriptor instance_descriptor{
            .requiredFeatureCount = 1,
            .requiredFeatures = &timed_wait_any};
    instance = wgpu::CreateInstance(&instance_descriptor);

    // Obtiene el Adapter de manera asíncrona
    wgpu::Future future_1 = instance.RequestAdapter(
            nullptr,                         // Sin opciones
            wgpu::CallbackMode::WaitAnyOnly, // Hace uso de la característica anterior
            [](wgpu::RequestAdapterStatus status, wgpu::Adapter l_adapter, wgpu::StringView message) {
                // Si no hay exito, aborta el programa
                if (status != wgpu::RequestAdapterStatus::Success) {
                    std::cerr << "RequestAdapter: " << message << std::endl;
                    exit(-1);
                }
                // Mueve valor del Adapter del callback al global
                adapter = std::move(l_adapter);
            } // Callback que se llama al terminar la operación
    );

    // Bloquea la ejecución hasta que termina la anterior función
    instance.WaitAny(future_1, /* Timeout: */ UINT64_MAX);

    // TODO: documentar
    wgpu::DeviceDescriptor dev_descriptor{};
    // Muestra un mensaje de error al ocurrir un error no capturado
    dev_descriptor.SetUncapturedErrorCallback(
            [](const wgpu::Device &, wgpu::ErrorType error_type, wgpu::StringView message) {
                std::cerr << "Error: " << error_type << " - message: " << message << std::endl;
            } // Callback al saltar el error
    );

    // Similar al bloque asíncrono anterior, esta vez para obtener el Device
    wgpu::Future future_2 = adapter.RequestDevice(
            &dev_descriptor,                 // El descriptor del dispositivo creado anteriormente
            wgpu::CallbackMode::WaitAnyOnly, // Hace uso de la característica anterior
            [](wgpu::RequestDeviceStatus status, wgpu::Device l_device, wgpu::StringView message) {
                if (status != wgpu::RequestDeviceStatus::Success) {
                    std::cout << "RequestDevice: " << message << std::endl;
                    exit(-1);
                }
                // Mueve valor del Device del callback al global
                device = std::move(l_device);
            } // Callback que se llama al terminar la operación
    );

    // Bloquea la ejecución hasta que termina la anterior función
    instance.WaitAny(future_2, /* Timeout: */ UINT64_MAX);
}

void init_glfw() {
    // Si no se puede inicializar la librería, aborta el programa
    if (!glfwInit())
        exit(-1);

    // Indica a la librería que no inicialice ninguna API gráfica
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Crea la ventana con el tamaño y el título indicados
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Primera Ventana", nullptr, nullptr);
}

int main() {
    init_webgpu();
    init_glfw();

    surface = wgpu::glfw::CreateSurfaceForWindow(instance, window);

    init_graphics();

    // En el caso de compilar para web, se hace uso del bucle de emscripten,
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(render, 0, false);
#else
    // Bucle de eventos
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        render();
        // "Swap Chain". Muestra la superficie en la ventana
        surface.Present();
        instance.ProcessEvents();
    }
#endif
    return 0;
}
