#pragma once

#include "imgui.h"

#include "Renderer.h"
#include "game_objects/BunnyObject.h"
#include "game_objects/StaticGenericMeshObject.h"
#include "game_objects/SponzaObject.h"
#include "game_objects/BoatObject.h"

/// Representa una escena, con su cámara, sus objetos y su UI.
class Scene {
public:
    /// Cámara de la escena.
    Camera camera;
    /// Lista de objetos a renderizar de la escena.
    std::vector<GameObject *> game_objects;

    /// Función con los comandos inmediatos de ImGui para crear la interfaz de usuario de la escena.
    virtual void ui() = 0;

    /// Función que se ejecuta cada frame.
    virtual void update(float delta_time) = 0;

    /// Función que se encarga de renderizar la escena
    virtual void render(Renderer &renderer) = 0;
};

class BunnyScene : public Scene {
    // Variables para la telemetría
    constexpr static int rb_size = 1000;
    float ring_buffer_cpu[rb_size];
    float ring_buffer_total[rb_size];
    float ring_buffer_fps[rb_size];
    size_t ring_buffer_index = 0;

    float cpu_frame_time, delta_time;
    double cpu_time_start;

public:
    BunnyScene(const Renderer &renderer) {
        // Crea la cámara
        camera = Camera{
                .focal_length = 10.0f,
                .aspect_ratio = (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
                .near_plane = 0.1f,
                .far_plane = 1000.0f,
                .look_from = vec3{0, 0, -5},
                .look_at = vec3{0, 0, 0},
        };

        // Obtiene la render pipeline principal del renderer
        wgpu::RenderPipeline default_pipeline = renderer.get_pipelines()["default"];

        // Crea un objeto con el modelo del conejo de Stanford
        auto game_object_1 = new BunnyObject();
        game_object_1->init(default_pipeline, renderer.get_device());

        // Crea otro objeto con el modelo del conejo de Stanford, pero rotado 180 grados
        auto game_object_2 = new BunnyObject();
        game_object_2->init(default_pipeline, renderer.get_device());
        game_object_2->transform.rotation = vec3{0, glm::pi<float>(), 0};

        // Crea un objeto con el modelo de una pirámide, escalado en Y para que sea más plano y situado debajo de los conejos
        auto game_object_3 = new StaticGenericMeshObject();
        game_object_3->init(RESOURCE_DIR "pyramid.obj", default_pipeline, renderer.get_device());
        game_object_3->transform.position = vec3{0, -0.4, 0};
        game_object_3->transform.scale = vec3{1, 0.5f, 1};

        // Añade los objetos a la lista de objetos a renderizar
        game_objects = std::vector<GameObject *>{
                game_object_1,
                game_object_2,
                game_object_3
        };
    }

    void ui() override {
        using namespace ImGui;

        SetNextWindowPos({0, 0});
        SetNextWindowSizeConstraints({300, 300}, {FLT_MAX, FLT_MAX});
        Begin("##Scene");
        // Sección con los parámetros de la cámara
        if (CollapsingHeader("Camera")) {
            PushItemWidth(-1);
            Text("Position");
            DragFloat3("##Position", &camera.look_from.x, 0.1f);
            Text("Look At");
            DragFloat3("##LookAt", &camera.look_at.x, 0.1f);
            Text("Focal Length (%.0fº vfov):", glm::degrees(2 * atan(1 / camera.focal_length)));
            DragFloat("##FocalLength", &camera.focal_length, 0.1f, 0.1f, 100.0f);
            Text("Near/Far Plane:");
            DragFloat2("##NearFar", &camera.near_plane, 0.1f, 0.1f);
            PopItemWidth();
        }
        // Sección con los parámetros de cada objeto
        if (CollapsingHeader("GameObjects")) {
            auto view = camera.get_view_matrix();
            auto projection = camera.get_projection_matrix();

            for (size_t i = 0; i < game_objects.size(); i++) {
                auto &game_object = game_objects[i];
                if (TreeNode(("GameObject " + std::to_string(i)).c_str())) {
                    PushItemWidth(-1);
                    Unindent(20);

                    auto transform = game_object->transform;
                    Text("Position");
                    DragFloat3("##Position", glm::value_ptr(transform.position), 0.1f);
                    Text("Rotation");
                    transform.rotation = glm::degrees(transform.rotation);
                    DragFloat3("##Rotation", glm::value_ptr(transform.rotation), 0.1f);
                    Text("Scale");
                    DragFloat3("##Scale", glm::value_ptr(transform.scale), 0.1f);

                    transform.rotation = glm::radians(transform.rotation);
                    game_object->transform = transform;

                    Indent(20);
                    PopItemWidth();
                    TreePop();
                }
            }
        }
        // Sección con la telemetría
        ring_buffer_cpu[ring_buffer_index] = cpu_frame_time*1000;
        ring_buffer_total[ring_buffer_index] = delta_time*1000;
        ring_buffer_fps[ring_buffer_index] = 1/delta_time;
        ring_buffer_index = (ring_buffer_index + 1) % rb_size;
        double avg_cpu_frame_time = 0, avg_total_frame_time = 0, avg_fps = 0;
        for (size_t i = 0; i < rb_size; i++) {
            avg_cpu_frame_time += ring_buffer_cpu[i];
            avg_total_frame_time += ring_buffer_total[i];
            avg_fps += ring_buffer_fps[i];
        }
        avg_cpu_frame_time /= rb_size;
        avg_total_frame_time /= rb_size;
        avg_fps /= rb_size;

        Text("CPU Frame Time (ms)");
        ImGui::SameLine();
        Text("%.3f / %.3f", cpu_frame_time * 1000, avg_cpu_frame_time);
        PlotLines("##CPU Frame Time (ms)", ring_buffer_cpu, rb_size, 0);

        Text("Total Frame Time (ms)");
        ImGui::SameLine();
        Text("%.3f / %.3f", delta_time * 1000, avg_total_frame_time);
        PlotLines("##Total Frame Time (ms)", ring_buffer_total, rb_size, 0);

        Text("FPS");
        ImGui::SameLine();
        Text("%.1f / %.1f", 1 / delta_time, avg_fps);
        PlotLines("##FPS", ring_buffer_fps, rb_size, 0);
        End();
    }

    void update(float delta_time) override {
        // Telemetría
        cpu_time_start = glfwGetTime();
        this->delta_time = delta_time;

        // Actualiza la cámara
        camera.update(delta_time);

        // Actualiza cada objeto
        for (auto &game_object: game_objects) {
            game_object->update(delta_time);
        }
    }

    void render(Renderer &renderer) override {
        // Renderiza los objetos desde la perspectiva de la cámara y dibuja la UI
        renderer.render(camera, game_objects, [this]() { ui(); });

        // Telemetría
        cpu_frame_time = glfwGetTime() - cpu_time_start;
    }
};

class SponzaScene : public Scene {
public:
    // Variables para la telemetría
    constexpr static int rb_size = 1000;
    float ring_buffer_cpu[rb_size];
    float ring_buffer_total[rb_size];
    float ring_buffer_fps[rb_size];
    size_t ring_buffer_index = 0;

    float cpu_frame_time, delta_time;
    double cpu_time_start;

    SponzaScene(const Renderer &renderer) {
        // Crea la cámara
        camera = Camera{
                .focal_length = 10.0f,
                .aspect_ratio = (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
                .near_plane = 0.1f,
                .far_plane = 1000.0f,
                .look_from = vec3{0, 0, -5},
                .look_at = vec3{0, 0, 0},
        };

        // Obtiene las render pipelines del renderer (para asignarlas a los objetos)
        wgpu::RenderPipeline default_pipeline = renderer.get_pipelines()["default"];
        wgpu::RenderPipeline texture_pipeline = renderer.get_pipelines()["texture"];

        // Crea un objeto con el modelo de Sponza, escalado
        auto game_object_1 = new SponzaObject();
        game_object_1->init(default_pipeline, renderer.get_device());
        game_object_1->transform.scale = {.25, .25, .25};

        // Crea un objeto con el modelo de un barco, texturizado
        auto game_object_2 = new BoatObject();
        game_object_2->init(texture_pipeline, renderer.get_device());

        // Añade los objetos a la lista de objetos a renderizar
        game_objects = std::vector<GameObject *>{
                game_object_1,
                game_object_2,
        };
    }

    void ui() override {
        using namespace ImGui;

        SetNextWindowPos({0, 0});
        SetNextWindowSizeConstraints({300, 300}, {FLT_MAX, FLT_MAX});
        Begin("##Scene");
        // Sección con los parámetros de la cámara
        if (CollapsingHeader("Camera")) {
            PushItemWidth(-1);
            Text("Position");
            DragFloat3("##Position", &camera.look_from.x, 0.1f);
            Text("Look At");
            DragFloat3("##LookAt", &camera.look_at.x, 0.1f);
            Text("Focal Length (%.0fº vfov):", glm::degrees(2 * atan(1 / camera.focal_length)));
            DragFloat("##FocalLength", &camera.focal_length, 0.1f, 0.1f, 100.0f);
            Text("Near/Far Plane:");
            DragFloat2("##NearFar", &camera.near_plane, 0.1f, 0.1f);
            PopItemWidth();
        }
        // Sección con los parámetros de cada objeto
        if (CollapsingHeader("GameObjects")) {
            auto view = camera.get_view_matrix();
            auto projection = camera.get_projection_matrix();

            for (size_t i = 0; i < game_objects.size(); i++) {
                auto &game_object = game_objects[i];
                if (TreeNode(("GameObject " + std::to_string(i)).c_str())) {
                    PushItemWidth(-1);
                    Unindent(20);

                    auto transform = game_object->transform;
                    Text("Position");
                    DragFloat3("##Position", glm::value_ptr(transform.position), 0.1f);
                    Text("Rotation");
                    transform.rotation = glm::degrees(transform.rotation);
                    DragFloat3("##Rotation", glm::value_ptr(transform.rotation), 0.1f);
                    Text("Scale");
                    DragFloat3("##Scale", glm::value_ptr(transform.scale), 0.1f);

                    transform.rotation = glm::radians(transform.rotation);
                    game_object->transform = transform;

                    Indent(20);
                    PopItemWidth();
                    TreePop();
                }
            }
        }
        // Sección con la telemetría
        ring_buffer_cpu[ring_buffer_index] = cpu_frame_time*1000;
        ring_buffer_total[ring_buffer_index] = delta_time*1000;
        ring_buffer_fps[ring_buffer_index] = 1/delta_time;
        ring_buffer_index = (ring_buffer_index + 1) % rb_size;
        double avg_cpu_frame_time = 0, avg_total_frame_time = 0, avg_fps = 0;
        for (size_t i = 0; i < rb_size; i++) {
            avg_cpu_frame_time += ring_buffer_cpu[i];
            avg_total_frame_time += ring_buffer_total[i];
            avg_fps += ring_buffer_fps[i];
        }
        avg_cpu_frame_time /= rb_size;
        avg_total_frame_time /= rb_size;
        avg_fps /= rb_size;

        Text("CPU Frame Time (ms)");
        ImGui::SameLine();
        Text("%.3f / %.3f", cpu_frame_time * 1000, avg_cpu_frame_time);
        PlotLines("##CPU Frame Time (ms)", ring_buffer_cpu, rb_size, 0);

        Text("Total Frame Time (ms)");
        ImGui::SameLine();
        Text("%.3f / %.3f", delta_time * 1000, avg_total_frame_time);
        PlotLines("##Total Frame Time (ms)", ring_buffer_total, rb_size, 0);

        Text("FPS");
        ImGui::SameLine();
        Text("%.1f / %.1f", 1 / delta_time, avg_fps);
        PlotLines("##FPS", ring_buffer_fps, rb_size, 0);
        End();
    }

    void update(float delta_time) override {
        // Telemetría
        cpu_time_start = glfwGetTime();
        this->delta_time = delta_time;

        // Actualiza la cámara
        camera.update(delta_time);

        // Actualiza cada objeto
        for (auto &game_object: game_objects) {
            game_object->update(delta_time);
        }
    }

    void render(Renderer &renderer)

    override {
        // Renderiza los objetos desde la perspectiva de la cámara y dibuja la UI
        renderer.render(camera, game_objects, [this]() { ui(); });

        // Telemetría
        cpu_frame_time = glfwGetTime() - cpu_time_start;
    }
};