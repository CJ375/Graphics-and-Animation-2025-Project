#ifndef MASTER_RENDER_SCENE_H
#define MASTER_RENDER_SCENE_H

#include "utility/HelperTypes.h"
#include "Lights.h"
#include "GlobalData.h"
#include "RenderScene.h"
#include "RenderedEntity.h"
#include "Animator.h"
#include "rendering/renders/EntityRenderer.h"
#include "rendering/renders/AnimatedEntityRenderer.h"
#include "rendering/renders/EmissiveEntityRenderer.h"

namespace EditorScene { class ParticleEmitterElement; }
namespace ParticleRenderer { class ParticleRenderer; }

#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>

/// The master render scene, which holds a copy of each renderers RenderScene,
/// as well as the light scene, and offers an interface for adding/removing entities and lights.
/// Also holds the animator, which offers an API for controlling animation.
class MasterRenderScene {
    EntityRenderer::RenderScene entity_scene{};
    AnimatedEntityRenderer::RenderScene animated_entity_scene{};
    EmissiveEntityRenderer::RenderScene emissive_entity_scene{};
    std::unique_ptr<ParticleRenderer::ParticleRenderer> particle_renderer;

    LightScene light_scene{};
    std::vector<std::shared_ptr<EditorScene::ParticleEmitterElement>> particle_systems; 

    // A unique_ptr to shared_ptr mapping to help with lifetime management
    std::unordered_map<EditorScene::ParticleEmitterElement*, std::shared_ptr<EditorScene::ParticleEmitterElement>> particle_system_references;

public:
    MasterRenderScene();
    ~MasterRenderScene();

    Animator animator{};

    void insert_entity(std::shared_ptr<EntityRenderer::Entity> entity);
    void insert_entity(std::shared_ptr<AnimatedEntityRenderer::Entity> entity);
    void insert_entity(std::shared_ptr<EmissiveEntityRenderer::Entity> entity);

    bool remove_entity(const std::shared_ptr<EntityRenderer::Entity>& entity);
    bool remove_entity(const std::shared_ptr<AnimatedEntityRenderer::Entity>& entity);
    bool remove_entity(const std::shared_ptr<EmissiveEntityRenderer::Entity>& entity);

    void insert_light(std::shared_ptr<PointLight> point_light);
    void insert_light(std::shared_ptr<DirectionalLight> directional_light); // Task H - Directional Light Element

    bool remove_light(const std::shared_ptr<PointLight>& point_light);
    bool remove_light(const std::shared_ptr<DirectionalLight>& directional_light);

    // Particle System Management
    void add_particle_system(EditorScene::ParticleEmitterElement* system);
    void remove_particle_system(EditorScene::ParticleEmitterElement* system);
    const std::vector<std::shared_ptr<EditorScene::ParticleEmitterElement>>& get_particle_systems() const; // Getter for EditorScene to tick particles
    
    // Clear all entities, lights, and particles
    void clear() {
        entity_scene.entities.clear();
        animated_entity_scene.entities.clear();
        emissive_entity_scene.entities.clear();
        light_scene.point_lights.clear();
        particle_systems.clear();
    }

    /// Propagates a camera state to all the render scenes
    void use_camera(const CameraInterface& camera_interface);

    friend class MasterRenderer;
    // friend class ParticleRenderer;
};

#endif //MASTER_RENDER_SCENE_H
