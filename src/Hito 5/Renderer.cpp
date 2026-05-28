#include "Renderer.h"

#include "dawn/webgpu_cpp_print.h"
#include "webgpu/webgpu_glfw.h"

#include "global_defines.h"
#include "ResourceManager.h"
#include "GameObject.h"

bool Renderer::init(GLFWwindow *window) {
    if (!init_webgpu()) return false;
    if (!init_surface(window)) return false;
    if (!init_default_render_pipeline()) return false;
    if (!init_uniform_buffers()) return false;

    printf("Init Renderer done\n");
    return true;
}

bool Renderer::init_webgpu() {
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
    // Configura un callback para mostrar los mensajes de log del dispositivo (solo en nativo)
    device.SetLoggingCallback([](wgpu::LoggingType type, wgpu::StringView message) {
        std::cerr << type << " - " << message << std::endl;
    });
#endif

    return true;
}

bool Renderer::init_surface(GLFWwindow *window) {
    // Crea la superficie para la ventana de GLFW
    surface = wgpu::glfw::CreateSurfaceForWindow(instance, window);
    if (!surface) return false;
    printf("Init Surface\n");

    // Extrae el formato de textura a partir de las capabilities
    wgpu::SurfaceCapabilities surface_capabilities;
    surface.GetCapabilities(adapter, &surface_capabilities);
    texture_format = surface_capabilities.formats[0];

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

bool Renderer::init_default_render_pipeline() {
    // El código fuente del shader, escrito en WGSL
    wgpu::ShaderModule shader_module =
            ResourceManager::load_shader_module(RESOURCE_DIR "perspective_shader.wgsl", device);
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

    // Configura el layout del bind group para los uniforms de la cámara. @group(0) @binding(0) en el shader
    wgpu::BindGroupLayoutEntry binding_layout_entry = {
            .binding = 0,
            .visibility = wgpu::ShaderStage::Vertex,
            .buffer = {
                    .type = wgpu::BufferBindingType::Uniform,
                    .minBindingSize = sizeof(CameraUniforms),
            },
    };

    wgpu::BindGroupLayoutDescriptor bind_group_layout_desc{
            .label = "Camera Bind Group Layout",
            .entryCount = 1,
            .entries = &binding_layout_entry,
    };
    wgpu::BindGroupLayout bind_group_layout = device.CreateBindGroupLayout(&bind_group_layout_desc);

    // Configura el layout del bind group para los uniforms de cada objeto (transformación) @group(1) @binding(0) en el shader
    wgpu::BindGroupLayoutEntry game_object_binding_layout_entry = {
            .binding = 0,
            .visibility = wgpu::ShaderStage::Vertex,
            .buffer = {
                    .type = wgpu::BufferBindingType::Uniform,
                    .minBindingSize = sizeof(mat4),
            },
    };

    wgpu::BindGroupLayoutDescriptor game_object_bind_group_layout_desc{
            .label = "GameObject Bind Group Layout",
            .entryCount = 1,
            .entries = &game_object_binding_layout_entry,
    };

    wgpu::BindGroupLayout game_object_bind_group_layout =
            device.CreateBindGroupLayout(&game_object_bind_group_layout_desc);

    // Configura el layout de la pipeline con los bind groups anteriores
    std::vector<wgpu::BindGroupLayout> bind_group_layouts{
            bind_group_layout,
            game_object_bind_group_layout
    };

    wgpu::PipelineLayoutDescriptor pipeline_layout_desc{
            .bindGroupLayoutCount = bind_group_layouts.size(),
            .bindGroupLayouts = bind_group_layouts.data(),
    };
    wgpu::PipelineLayout pipeline_layout = device.CreatePipelineLayout(&pipeline_layout_desc);

    // Configura la render pipeline con el vertex shader, fragment shader, formato de las primitivas y depth buffer
    wgpu::RenderPipelineDescriptor render_pipeline_descriptor{
            .layout = pipeline_layout,
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

bool Renderer::init_uniform_buffers() {
    // Crea el uniform buffer para la cámara
    wgpu::BufferDescriptor camera_uniform_buffer_desc{
            .usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
            .size = sizeof(CameraUniforms),
    };
    camera_uniform_buffer = device.CreateBuffer(&camera_uniform_buffer_desc);
    if (!camera_uniform_buffer) return false;

    // Crea el bind group para la cámara, enlazando el uniform buffer al binding 0 del grupo 0 del shader
    wgpu::BindGroupEntry camera_bind_group_entry{
            .binding = 0,
            .buffer = camera_uniform_buffer,
            .offset = 0,
            .size = sizeof(CameraUniforms),
    };
    wgpu::BindGroupDescriptor camera_bind_group_desc{
            .layout = pipeline.GetBindGroupLayout(0),
            .entryCount = 1,
            .entries = &camera_bind_group_entry,
    };

    camera_bind_group = device.CreateBindGroup(&camera_bind_group_desc);

    return true;
}

void Renderer::render(const Camera &camera, const std::vector<GameObject> &game_objects) {
    // Obtiene la textura actual de la superficie a pintar
    wgpu::SurfaceTexture surface_texture;
    surface.GetCurrentTexture(&surface_texture);

    // Indica al Render Pass las texturas objetivo (color y profundidad)
    wgpu::RenderPassColorAttachment attachment{
            .view = surface_texture.texture.CreateView(),
            // Limpia la textura antes de pintar
            .loadOp = wgpu::LoadOp::Clear,
            .storeOp = wgpu::StoreOp::Store,
            // Color con el que limpiar
            .clearValue = {.1, .1, .1, 1}
    };

    // Configura la textura de profundidad para el Render Pass
    wgpu::RenderPassDepthStencilAttachment depth_stencil_attachment{
            .view = depth_texture_view,
            .depthLoadOp = wgpu::LoadOp::Clear,
            .depthStoreOp = wgpu::StoreOp::Store,
            .depthClearValue = 1.0f,
            .depthReadOnly = false,
            // Ignora el stencil
            .stencilLoadOp = wgpu::LoadOp::Undefined,
            .stencilStoreOp = wgpu::StoreOp::Undefined,
            .stencilClearValue = 0,
            .stencilReadOnly = true,
    };

    // Indica al Render Pass el número de texturas objetivo y las texturas objetivo.
    wgpu::RenderPassDescriptor render_pass_descriptor{
            .colorAttachmentCount = 1,
            .colorAttachments = &attachment,
            .depthStencilAttachment = &depth_stencil_attachment
    };

    // Crea un codificador de comandos.
    // Es necesario porque cada dispositivo físico tiene su propia codificación
    wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&render_pass_descriptor);

    // Comandos:
    // Se selecciona la pipeline a utilizar
    pass.SetPipeline(pipeline);

    // Se actualizan los datos de la cámara en el uniform buffer
    CameraUniforms camera_uniforms{
            .projection = camera.get_projection_matrix(),
            .view = camera.get_view_matrix(),
    };
    device.GetQueue().WriteBuffer(camera_uniform_buffer, 0, &camera_uniforms, sizeof(CameraUniforms));

    // Se establece el bind group con el uniform buffer de la cámara
    pass.SetBindGroup(0, camera_bind_group);

    // Para cada objeto:
    for (const auto &game_object: game_objects) {
        // Se actualizan los datos de la transformación del objeto en su uniform buffer
        auto transform = game_object.transform.get_matrix();
        device.GetQueue().WriteBuffer(game_object.transform_buffer, 0, &transform, sizeof(mat4));

        // Se establece el bind group del objeto
        pass.SetBindGroup(1, game_object.bind_group);

        // Se establece el vertex buffer del objeto y se dibujan sus vértices
        pass.SetVertexBuffer(0, game_object.vertex_buffer);
        pass.Draw(game_object.vertex_buffer.GetSize() / sizeof(VertexAttributes));
    }

    // Finaliza el Render Pass, termina la codificación de comandos y envia los comandos.
    pass.End();
    wgpu::CommandBuffer commands = encoder.Finish();
    device.GetQueue().Submit(1, &commands);
}

wgpu::Surface Renderer::get_surface() const { return surface; }

wgpu::Instance Renderer::get_instance() const { return instance; }

wgpu::Device Renderer::get_device() const { return device; }

wgpu::RenderPipeline Renderer::get_pipeline() const { return pipeline; }
