#include "Renderer.h"

#include "dawn/webgpu_cpp_print.h"
#include "webgpu/webgpu_glfw.h"
#include "imgui.h"
#include "backends/imgui_impl_wgpu.h"
#include "backends/imgui_impl_glfw.h"


#include "global_defines.h"
#include "ResourceManager.h"
#include "game_objects/GameObject.h"

bool Renderer::init(GLFWwindow *window) {
    if (!init_webgpu()) return false;
    if (!init_surface(window)) return false;
    if (!init_depth_buffer()) return false;
    if (!init_render_pipelines()) return false;
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

    // Obtiene el tamaño de la ventana
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

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
            .width = (u32) width,
            .height = (u32) height,
            // Indica donde se pinta en la superficie.
            // ::Immediate pinta directamente en la superficie.
            // ::Fifo pinta en un buffer intermedio varios frames, y los muestra en orden.
            // ::Mailbox pinta en un buffer intermedio y descarta los frames anteriores si no se han mostrado.
#ifdef __EMSCRIPTEN__
            .presentMode = wgpu::PresentMode::Fifo,
#else
            .presentMode = wgpu::PresentMode::Mailbox,
#endif
    };
    surface.Configure(&config);
    printf("Configure Surface\n");

    return true;
}

bool Renderer::init_depth_buffer() {
    // Configura y crea el depth buffer (sin stencil)
    depth_stencil_state = {
            // Formato con 24 bits para profundidad
            .format = wgpu::TextureFormat::Depth24Plus,
            .depthWriteEnabled = true,
            .depthCompare = wgpu::CompareFunction::Less,
            // Ignora el stencil
            .stencilReadMask = 0,
            .stencilWriteMask = 0,
    };
    wgpu::TextureDescriptor depth_texture_desc{
            .label = "Depth Texture",
            .usage = wgpu::TextureUsage::RenderAttachment,
            .dimension =  wgpu::TextureDimension::e2D,
            .size = {WINDOW_WIDTH, WINDOW_HEIGHT, 1},
            .format =  depth_stencil_state.format,
            .mipLevelCount = 1,
            .sampleCount = 1,
            .viewFormatCount = 1,
            .viewFormats = &depth_stencil_state.format,
    };
    depth_texture = device.CreateTexture(&depth_texture_desc);
    wgpu::TextureViewDescriptor depth_texture_view_desc{
            .label = "Depth Texture View",
            .format = depth_stencil_state.format,
            .dimension = wgpu::TextureViewDimension::e2D,
            .baseMipLevel = 0,
            .mipLevelCount = 1,
            .baseArrayLayer = 0,
            .arrayLayerCount = 1,
            .aspect = wgpu::TextureAspect::DepthOnly,
    };
    depth_texture_view = depth_texture.CreateView(&depth_texture_view_desc);

    if (!depth_texture || !depth_texture_view) return false;
    return true;
}

bool Renderer::init_render_pipelines() {
    if (!init_default_render_pipeline()) return false;
    if (!init_texture_render_pipeline()) return false;

    return true;
}


bool Renderer::init_default_render_pipeline() {

    // Crea los bind group layouts necesarios para el shader.
    auto bind_group_layouts = std::vector<wgpu::BindGroupLayout>{
            create_buffer_bind_group_layout<CameraUniforms>(wgpu::ShaderStage::Vertex, "Camera BGL"),
            create_buffer_bind_group_layout<mat4>(wgpu::ShaderStage::Vertex, "Object Transform BGL")
    };
    // Crea la render pipeline a partir del shader y los bind group layouts.
    pipelines["default"] =
            create_render_pipeline(RESOURCE_DIR "perspective_shader.wgsl", bind_group_layouts, "Default RP");

    if (!pipelines["default"]) return false;
    printf("Init Default Render Pipeline\n");

    return true;
}

bool Renderer::init_texture_render_pipeline() {

    // Crea los bind group layouts necesarios para el shader.
    auto bind_group_layouts = std::vector<wgpu::BindGroupLayout>{
            create_buffer_bind_group_layout<CameraUniforms>(wgpu::ShaderStage::Vertex, "Camera BGL"),
            create_buffer_bind_group_layout<mat4>(wgpu::ShaderStage::Vertex, "Object Transform BGL"),
            create_texture_bind_group_layout(wgpu::ShaderStage::Fragment, "Texture BGL"),
    };
    // Crea la render pipeline a partir del shader y los bind group layouts.
    pipelines["texture"] =
            create_render_pipeline(RESOURCE_DIR "texture_shader.wgsl", bind_group_layouts, "Texture RP");

    if (!pipelines["texture"]) return false;
    printf("Init Texture Render Pipeline\n");

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
            .layout = pipelines["default"].GetBindGroupLayout(0),
            .entryCount = 1,
            .entries = &camera_bind_group_entry,
    };

    camera_bind_group = device.CreateBindGroup(&camera_bind_group_desc);

    return true;
}

void Renderer::render(const Camera &camera,
                      const std::vector<GameObject *> &game_objects,
                      const std::function<void()> &ui) {
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

    // Se actualizan los datos de la cámara en el uniform buffer
    CameraUniforms camera_uniforms{
            .projection = camera.get_projection_matrix(),
            .view = camera.get_view_matrix(),
    };
    device.GetQueue().WriteBuffer(camera_uniform_buffer, 0, &camera_uniforms, sizeof(CameraUniforms));

    // Comandos:

    // Se establece el bind group con el uniform buffer de la cámara
    // Todas las pipelines (default y texture) utilizan el mismo bind group layout para la cámara, en la misma posición
    for (auto &[name, pipeline]: pipelines) {
        pass.SetPipeline(pipeline);
        pass.SetBindGroup(0, camera_bind_group);
    }

    // Renderiza cada objeto
    for (const auto &game_object: game_objects) {
        game_object->render(pass, device.GetQueue());
    }

    // Dibuja la UI de ImGui
    if (ui) {
        // Inicializa un nuevo frame para ImGui
        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Llama a la función de UI pasada como parámetro
        ui();

        // Renderiza la UI
        ImGui::EndFrame();
        ImGui::Render();
        // Cuando se cambia el tamaño de la ventana, imgui se puede desincronizar con la superficie
        auto *draw_data = ImGui::GetDrawData();
        draw_data->DisplaySize = ImVec2((f32) surface_texture.texture.GetWidth(),
                                        (f32) surface_texture.texture.GetHeight());
        ImGui_ImplWGPU_RenderDrawData(draw_data, pass.Get());
    }

    // Finaliza el Render Pass, termina la codificación de comandos y los envía.
    pass.End();
    wgpu::CommandBuffer commands = encoder.Finish();
    device.GetQueue().Submit(1, &commands);
}

void Renderer::resize(u32 width, u32 height) {
    // Reconfigura la superficie con el nuevo tamaño
    surface.Unconfigure();
    wgpu::SurfaceConfiguration config{
            .device = device,
            .format = texture_format,
            .usage = wgpu::TextureUsage::RenderAttachment,
            .width = width,
            .height = height,
            .presentMode = wgpu::PresentMode::Fifo,
    };
    surface.Configure(&config);

    // Sustituye la textura de profundidad por una nueva con el nuevo tamaño
    depth_texture.Destroy();
    wgpu::TextureDescriptor depth_texture_desc{
            .usage = wgpu::TextureUsage::RenderAttachment,
            .dimension =  wgpu::TextureDimension::e2D,
            .size = {width, height, 1},
            .format =  depth_stencil_state.format,
            .mipLevelCount = 1,
            .sampleCount = 1,
            .viewFormatCount = 1,
            .viewFormats = &depth_stencil_state.format,
    };
    depth_texture = device.CreateTexture(&depth_texture_desc);
    depth_texture_view = depth_texture.CreateView();
}

template<typename DataStruct>
wgpu::BindGroupLayout Renderer::create_buffer_bind_group_layout(const wgpu::ShaderStage visibility, const char *label) {

    // Configura el layout del bind group para un uniform buffer con la estructura de datos del template
    wgpu::BindGroupLayoutEntry binding_layout_entry = {
            .binding = 0,
            .visibility = visibility,
            .buffer = {
                    .type = wgpu::BufferBindingType::Uniform,
                    .minBindingSize = sizeof(DataStruct),
            },
    };

    wgpu::BindGroupLayoutDescriptor bind_group_layout_desc{
            .label = label,
            .entryCount = 1,
            .entries = &binding_layout_entry,
    };

    return device.CreateBindGroupLayout(&bind_group_layout_desc);
}

wgpu::BindGroupLayout Renderer::create_texture_bind_group_layout(const wgpu::ShaderStage visibility,
                                                                 const char *label) {

    // Configura el layout del bind group para una textura 2D y un sampler
    wgpu::BindGroupLayoutEntry binding_layout_entry = {
            .binding = 0,
            .visibility = visibility,
            .texture = {
                    .sampleType = wgpu::TextureSampleType::Float,
                    .viewDimension = wgpu::TextureViewDimension::e2D,
            },
    };
    wgpu::BindGroupLayoutEntry sampler_binding_layout_entry = {
            .binding = 1,
            .visibility = visibility,
            .sampler = {
                    .type = wgpu::SamplerBindingType::Filtering,
            },
    };

    std::vector<wgpu::BindGroupLayoutEntry> entries{
            binding_layout_entry,
            sampler_binding_layout_entry
    };

    wgpu::BindGroupLayoutDescriptor bind_group_layout_desc{
            .label = label,
            .entryCount = entries.size(),
            .entries = entries.data(),
    };

    return device.CreateBindGroupLayout(&bind_group_layout_desc);
}


wgpu::RenderPipeline Renderer::create_render_pipeline(
        const char *shader_path,
        const std::vector<wgpu::BindGroupLayout> &bind_group_layouts,
        const char *label) {

    // El código fuente del shader, escrito en WGSL
    wgpu::ShaderModule shader_module = ResourceManager::load_shader_module(shader_path, device);

    // ### Vertex Shader ###
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

    wgpu::VertexBufferLayout vertex_buff_layout = wgpu::VertexBufferLayout{
            .stepMode = wgpu::VertexStepMode::Vertex,
            .arrayStride = sizeof(VertexAttributes),
            .attributeCount = attributes.size(),
            .attributes = attributes.data(),
    };

    // ### Fragment Shader ###
    // Indica el formato de la textura para el fragment shader
    wgpu::ColorTargetState color_target_state{
            .format = texture_format
    };

    // Configura el fragment shader
    wgpu::FragmentState fragment_state{
            .module = shader_module,
            .targetCount = 1,
            .targets = &color_target_state,
    };

    // ### Pipeline Layout ###
    wgpu::PipelineLayoutDescriptor pipeline_layout_desc{
            .label = label,
            .bindGroupLayoutCount = bind_group_layouts.size(),
            .bindGroupLayouts = bind_group_layouts.data(),
    };
    wgpu::PipelineLayout pipeline_layout = device.CreatePipelineLayout(&pipeline_layout_desc);

    // Configura la render pipeline con el vertex shader, fragment shader, formato de las primitivas y depth buffer
    wgpu::RenderPipelineDescriptor render_pipeline_descriptor{
            .label = label,
            .layout = pipeline_layout,
            .vertex = {
                    .module = shader_module,
                    .bufferCount = 1,
                    .buffers = &vertex_buff_layout
            },
            .primitive = {
                    .topology = wgpu::PrimitiveTopology::TriangleList,
                    .frontFace = wgpu::FrontFace::CCW,
                    .cullMode = wgpu::CullMode::Front,
            },
            .depthStencil = &depth_stencil_state,
            .fragment = &fragment_state,
    };
    return device.CreateRenderPipeline(&render_pipeline_descriptor);
}

wgpu::Surface Renderer::get_surface() const { return surface; }

wgpu::Instance Renderer::get_instance() const { return instance; }

wgpu::Device Renderer::get_device() const { return device; }

std::unordered_map<std::string, wgpu::RenderPipeline> Renderer::get_pipelines() const { return pipelines; }

wgpu::TextureFormat Renderer::get_texture_format() const { return texture_format; }