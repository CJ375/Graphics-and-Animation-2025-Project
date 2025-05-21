#include "MasterRenderScene.h"
#include "rendering/renders/ParticleRenderer.h"
#include "scene/editor_scene/ParticleEmitterElement.h"

void MasterRenderScene::use_camera(const CameraInterface& camera_interface) {
    entity_scene.global_data.use_camera(camera_interface);
    animated_entity_scene.global_data.use_camera(camera_interface);
    emissive_entity_scene.global_data.use_camera(camera_interface);
}

void MasterRenderScene::insert_entity(std::shared_ptr<EntityRenderer::Entity> entity) {
    entity_scene.entities.insert(std::move(entity));
}

void MasterRenderScene::insert_entity(std::shared_ptr<AnimatedEntityRenderer::Entity> entity) {
    animated_entity_scene.entities.insert(std::move(entity));
}

void MasterRenderScene::insert_entity(std::shared_ptr<EmissiveEntityRenderer::Entity> entity) {
    emissive_entity_scene.entities.insert(std::move(entity));
}

bool MasterRenderScene::remove_entity(const std::shared_ptr<EntityRenderer::Entity>& entity) {
    return entity_scene.entities.erase(entity) != 0;
}

bool MasterRenderScene::remove_entity(const std::shared_ptr<AnimatedEntityRenderer::Entity>& entity) {
    return animated_entity_scene.entities.erase(entity) != 0;
}

bool MasterRenderScene::remove_entity(const std::shared_ptr<EmissiveEntityRenderer::Entity>& entity) {
    return emissive_entity_scene.entities.erase(entity) != 0;
}

void MasterRenderScene::insert_light(std::shared_ptr<PointLight> point_light) {
    light_scene.point_lights.insert(std::move(point_light));
}

// Task H - Directional Light Element
void MasterRenderScene::insert_light(std::shared_ptr<DirectionalLight> directional_light) {
    light_scene.directional_lights.insert(std::move(directional_light));
}

bool MasterRenderScene::remove_light(const std::shared_ptr<PointLight>& point_light) {
    return light_scene.point_lights.erase(point_light) != 0;
}

MasterRenderScene::MasterRenderScene() {
    particle_renderer = std::make_unique<ParticleRenderer::ParticleRenderer>();
}

MasterRenderScene::~MasterRenderScene() {
    // Unique_ptr will handle cleanup automatically
}

void MasterRenderScene::add_particle_system(EditorScene::ParticleEmitterElement* system) {
    if (system) {
        // Create a shared_ptr with a no-op deleter to avoid double deletion
        auto shared_system = std::shared_ptr<EditorScene::ParticleEmitterElement>(system, [](EditorScene::ParticleEmitterElement*){});
        particle_systems.push_back(shared_system);
        particle_system_references[system] = shared_system;
    }
}

void MasterRenderScene::remove_particle_system(EditorScene::ParticleEmitterElement* system) {
    if (system) {
        auto it = particle_system_references.find(system);
        if (it != particle_system_references.end()) {
            auto shared_system = it->second;
            particle_systems.erase(
                std::remove_if(particle_systems.begin(), particle_systems.end(),
                              [&](const std::shared_ptr<EditorScene::ParticleEmitterElement>& s) {
                                  return s == shared_system;
                              }),
                particle_systems.end()
            );
            particle_system_references.erase(it);
        }
    }
}

const std::vector<std::shared_ptr<EditorScene::ParticleEmitterElement>>& MasterRenderScene::get_particle_systems() const {
    return particle_systems;
}

bool MasterRenderScene::remove_light(const std::shared_ptr<DirectionalLight>& directional_light) {
    return light_scene.directional_lights.erase(directional_light) != 0;
}
