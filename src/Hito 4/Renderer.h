#pragma once

#include <vector>

#include "GameObject.h"
#include "ResourceManager.h"

/// Clase con el comportamiento de renderizado
class Renderer {
    // Representa una instancia de WebGPU
    wgpu::Instance instance;

    // Representa un dispositivo físico (adapter) y lógico (device)
    // El adapter se utiliza para crear el device con las características (capabilities) deseadas
    wgpu::Adapter adapter;
    wgpu::Device device;

    // Representa la superficie donde dibujar los gráficos
    wgpu::Surface surface;

    // Formato de la textura (número de canales, tamaño de cada canal y tipo de los canales)
    wgpu::TextureFormat texture_format;

    wgpu::TextureView depth_texture_view;

    // Representa la pipeline gráfica
    wgpu::RenderPipeline pipeline;

public:
    bool init(GLFWwindow *window) {
        if (!init_webgpu()) return false;
        if (!init_surface(window)) return false;
        if (!init_default_render_pipeline()) return false;

        printf("Init Renderer done\n");
        return true;
    }

    /// Inicializa el Device de WebGPU. Para ello primero se crea la instancia, luego se obtiene el Adapter y finalmente el Device.
    bool init_webgpu() {
        // Crea la instancia, asegurándose que disponga de la característica TimedWaitAny
        static const wgpu::InstanceFeatureName timed_wait_any = wgpu::InstanceFeatureName::TimedWaitAny;
        wgpu::InstanceDescriptor instance_descriptor{
                .requiredFeatureCount = 1,
                .requiredFeatures = &timed_wait_any};
        instance = wgpu::CreateInstance(&instance_descriptor);
        if (!instance) return false;
        printf("Init Instance\n");


        // Obtiene el Adapter de manera asíncrona
        wgpu::Future future_1 = instance.RequestAdapter(
                nullptr,                         // Sin opciones
                wgpu::CallbackMode::WaitAnyOnly, // Hace uso de la característica anterior
                [this](wgpu::RequestAdapterStatus status, wgpu::Adapter l_adapter, wgpu::StringView message) {
                    // Si no hay éxito, aborta el programa
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
        if (!adapter) return false;
        printf("Init Adapter\n");

        // Describe la petición de un dispositivo
        wgpu::DeviceDescriptor dev_descriptor{};
        // Muestra un mensaje de error al ocurrir un error no capturado
        dev_descriptor.SetUncapturedErrorCallback(
                [](const wgpu::Device &, wgpu::ErrorType error_type, wgpu::StringView message) {
                    std::cerr << "Error: " << error_type << " - message: " << message << std::endl;
                    exit(-1);
                } // Callback al saltar el error
        );

        // Similar al bloque asíncrono anterior, esta vez para obtener el Device
        wgpu::Future future_2 = adapter.RequestDevice(
                &dev_descriptor,                 // El descriptor del dispositivo creado anteriormente
                wgpu::CallbackMode::WaitAnyOnly, // Hace uso de la característica anterior
                [this](wgpu::RequestDeviceStatus status, wgpu::Device l_device, wgpu::StringView message) {
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
        if (!device) return false;
        printf("Init Device\n");

#ifndef __EMSCRIPTEN__
        device.SetLoggingCallback([](wgpu::LoggingType type, wgpu::StringView message) {
            std::cerr << type << " - " << message << std::endl;
        });
#endif

        return true;
    }

    /// Crea y configura la superficie con el dispositivo, formato de textura y tamaño.
    bool init_surface(GLFWwindow *window) {
        // Crea la superficie para la ventana de GLFW
        surface = wgpu::glfw::CreateSurfaceForWindow(instance, window);
        if (!surface) return false;
        printf("Init Surface\n");

        // Extrae el formato de textura a partir de las capabilities
        wgpu::SurfaceCapabilities capabilities;
        surface.GetCapabilities(adapter, &capabilities);
        texture_format = capabilities.formats[0];

        // Configura la superficie
        wgpu::SurfaceConfiguration config{
                .device = device,
                .format = texture_format,
                // Indica que la textura se va a usar como render target (se va a pintar en ella)
                .usage = wgpu::TextureUsage::RenderAttachment,
                .width = WINDOW_WIDTH,
                .height = WINDOW_HEIGHT,
                // Indica donde se pinta en la superficie.
                // ::Immediate pinta directamente en la superficie.
                // ::Fifo pinta en un buffer intermedio varios frames, y los muestra en orden.
                // ::Mailbox pinta en un buffer intermedio y descarta los frames anteriores si no se han mostrado.
                .presentMode = wgpu::PresentMode::Fifo,
        };
        surface.Configure(&config);
        printf("Configure Surface\n");

        return true;
    }

    /// Crea una render pipeline (vertex shader -> fragment shader).
    /// También configura el depth buffer.
    bool init_default_render_pipeline() {
        // El código fuente del shader, escrito en WGSL
        wgpu::ShaderModule shader_module = ResourceManager::load_shader_module(RESOURCE_DIR "normals_shader.wgsl",
                                                                               device);
        if (!shader_module) return false;

        // Indica el formato de la textura para el fragment shader
        wgpu::ColorTargetState color_target_state{.format = texture_format};

        // Configura el fragment shader
        wgpu::FragmentState fragment_state{
                .module = shader_module,
                .targetCount = 1,
                .targets = &color_target_state,
        };

        // Configura los atributos de los vértices (posición, normal y UV)
        std::vector<wgpu::VertexAttribute> attributes(0);

        auto position_attr = wgpu::VertexAttribute{
                .format = wgpu::VertexFormat::Float32x3,
                .offset = offsetof(VertexAttributes, position),
                .shaderLocation = 0,
        };

        auto normal_attr = wgpu::VertexAttribute{
                .format = wgpu::VertexFormat::Float32x3,
                .offset = offsetof(VertexAttributes, normal),
                .shaderLocation = 1,
        };

        auto uv_attr = wgpu::VertexAttribute{
                .format = wgpu::VertexFormat::Float32x2,
                .offset = offsetof(VertexAttributes, uv),
                .shaderLocation = 2,
        };

        attributes.emplace_back(position_attr);
        attributes.emplace_back(normal_attr);
        attributes.emplace_back(uv_attr);

        auto vertex_buff_layout = wgpu::VertexBufferLayout{
                .stepMode = wgpu::VertexStepMode::Vertex,
                .arrayStride = sizeof(VertexAttributes),
                .attributeCount = attributes.size(),
                .attributes = attributes.data(),
        };

        // Configura y crea el depth buffer (sin stencil)
        wgpu::DepthStencilState depth_stencil_state = {
                // Formato con 24 bits para profundidad
                .format = wgpu::TextureFormat::Depth24Plus,
                .depthWriteEnabled = true,
                .depthCompare = wgpu::CompareFunction::Less,
                // Ignora el stencil
                .stencilReadMask = 0,
                .stencilWriteMask = 0,
        };
        wgpu::TextureDescriptor depth_texture_desc{
                .usage = wgpu::TextureUsage::RenderAttachment,
                .dimension =  wgpu::TextureDimension::e2D,
                .size = {WINDOW_WIDTH, WINDOW_HEIGHT, 1},
                .format =  depth_stencil_state.format,
                .mipLevelCount = 1,
                .sampleCount = 1,
                .viewFormatCount = 1,
                .viewFormats = &depth_stencil_state.format,
        };
        wgpu::Texture depth_texture = device.CreateTexture(&depth_texture_desc);
        wgpu::TextureViewDescriptor depth_texture_view_desc{
                .format = depth_stencil_state.format,
                .dimension = wgpu::TextureViewDimension::e2D,
                .baseMipLevel = 0,
                .mipLevelCount = 1,
                .baseArrayLayer = 0,
                .arrayLayerCount = 1,
                .aspect = wgpu::TextureAspect::DepthOnly,
        };
        depth_texture_view = depth_texture.CreateView(&depth_texture_view_desc);

        // Configura la render pipeline con el vertex shader, fragment shader, formato de las primitivas y depth buffer
        wgpu::RenderPipelineDescriptor render_pipeline_descriptor{
                .vertex = {
                        .module = shader_module,
                        .bufferCount = 1,
                        .buffers = &vertex_buff_layout
                },
                .primitive = {
                        .topology = wgpu::PrimitiveTopology::TriangleList,
                        .frontFace = wgpu::FrontFace::CCW,
                        .cullMode = wgpu::CullMode::None,
                },
                .depthStencil = &depth_stencil_state,
                .fragment = &fragment_state,
        };
        pipeline = device.CreateRenderPipeline(&render_pipeline_descriptor);
        if (!pipeline) return false;
        printf("Init Render Pipeline\n");

        return true;
    }


    /// Envía los comandos para renderizar la escena en la superficie a partir de una lista de GameObjects.
    void render(const std::vector<GameObject> &game_objects) {
        // Obtiene la textura actual de la superficie a pintar
        wgpu::SurfaceTexture surface_texture;
        surface.GetCurrentTexture(&surface_texture);

        // Indica al Render Pass las texturas objetivo
        wgpu::RenderPassColorAttachment attachment{
                .view = surface_texture.texture.CreateView(),
                .loadOp = wgpu::LoadOp::Clear, // Limpia la textura antes de pintar
                .storeOp = wgpu::StoreOp::Store,
                .clearValue = {.1, .1, .1, 1} // Color con el que limpiar
        };

        // Configura la textura de profundidad para el Render Pass
        wgpu::RenderPassDepthStencilAttachment depth_stencil_attachment{
                .view = depth_texture_view,
                .depthLoadOp = wgpu::LoadOp::Clear,
                .depthStoreOp = wgpu::StoreOp::Store,
                .depthClearValue = 1.0f,
                .depthReadOnly = false,
                .stencilLoadOp = wgpu::LoadOp::Undefined,
                .stencilStoreOp = wgpu::StoreOp::Undefined,
                .stencilClearValue = 0,
                .stencilReadOnly = true,
        };

        // Indica al Render Pass el número de texturas objetivo y las texturas objetivo.
        wgpu::RenderPassDescriptor render_pass{
                .colorAttachmentCount = 1,
                .colorAttachments = &attachment,
                .depthStencilAttachment = &depth_stencil_attachment
        };

        // Crea un codificador de comandos.
        // Es necesario porque cada dispositivo físico tiene su propia codificación
        wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&render_pass);
        // Comandos
        // Se selecciona la pipeline a utilizar
        pass.SetPipeline(pipeline);

        for (auto &game_object: game_objects) {
            pass.SetVertexBuffer(0, game_object.get_vertex_buffer());
            pass.Draw(game_object.get_vertex_buffer().GetSize() / sizeof(VertexAttributes));
        }

        pass.End();
        wgpu::CommandBuffer commands = encoder.Finish();
        // Envía los comandos
        device.GetQueue().Submit(1, &commands);
    };

    [[nodiscard]] wgpu::Surface get_surface() const { return surface; }

    [[nodiscard]] wgpu::Instance get_instance() const { return instance; }

    [[nodiscard]] wgpu::Device get_device() const { return device; }
};