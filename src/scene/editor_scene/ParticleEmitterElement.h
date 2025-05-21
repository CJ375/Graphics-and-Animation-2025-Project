#ifndef PARTICLE_EMITTER_ELEMENT_H
#define PARTICLE_EMITTER_ELEMENT_H

#include "SceneElement.h"
#include "scene/SceneContext.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace EditorScene {

    // Represents a single particle
    struct Particle {
        glm::vec3 position{0.0f};
        glm::vec3 velocity{0.0f};
        glm::vec4 color{1.0f};
        float size = 1.0f;
        float lifeRemaining = 0.0f;
        float totalLife = 1.0f;
        float rotation = 0.0f;
        float angularVelocity = 0.0f;
    };

    class ParticleEmitterElement : virtual public SceneElement, public LocalTransformComponent {
    public:
        static constexpr const char* ELEMENT_TYPE_NAME = "ParticleEmitter";

        // Emitter Properties
        float emissionRate = 10.0f; // Particles per second
        int maxParticles = 100;
        float particleLifespanMin = 1.0f;
        float particleLifespanMax = 3.0f;
        glm::vec3 initialVelocityMin{ -0.5f, 1.0f, -0.5f };
        glm::vec3 initialVelocityMax{ 0.5f, 2.0f,  0.5f };
        float initialSizeMin = 250.0f;
        float initialSizeMax = 400.0f;
        float endSizeFactor = 0.0f; // Multiplier for initial size at end of life
        float initialRotationMin = 0.0f;
        float initialRotationMax = 360.0f;
        float angularVelocityMin = -180.0f;
        float angularVelocityMax = 180.0f;
        glm::vec4 initialColorStart{ 1.0f, 1.0f, 1.0f, 1.0f };
        glm::vec4 initialColorEnd{ 1.0f, 1.0f, 1.0f, 1.0f };
        glm::vec4 endColor{ 1.0f, 1.0f, 1.0f, 0.0f }; // Fades out by default
        glm::vec3 gravity{ 0.0f, -0.98f, 0.0f };
        bool worldSpaceParticles = false; // If true, particles are not affected by emitter's transform after emission

        std::vector<Particle> particles;
        
    private:
        float emissionTimer = 0.0f;

    public:
        ParticleEmitterElement(const ElementRef& parent, std::string name);

        static std::unique_ptr<ParticleEmitterElement> new_default(const SceneContext& scene_context, ElementRef parent);
        static std::unique_ptr<ParticleEmitterElement> from_json(const SceneContext& scene_context, ElementRef parent, const json& j);

        [[nodiscard]] json into_json() const override;
        void add_imgui_edit_section(MasterRenderScene& render_scene, const SceneContext& scene_context) override;
        void update_instance_data() override;
        void add_to_render_scene(MasterRenderScene& target_render_scene) override;
        void remove_from_render_scene(MasterRenderScene& target_render_scene) override;
        [[nodiscard]] const char* element_type_name() const override;
        
        // Particle simulation logic
        void tick_particles(float deltaTime, const SceneContext& scene_context);
    };

} // namespace EditorScene

#endif