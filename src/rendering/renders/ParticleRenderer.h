#ifndef PARTICLE_RENDERER_H
#define PARTICLE_RENDERER_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "rendering/renders/shaders/ShaderInterface.h"
#include "rendering/scene/GlobalData.h"
#include "rendering/cameras/CameraInterface.h"
#include "rendering/renders/shaders/BaseEntityShader.h"
#include "rendering/resources/TextureHandle.h"
#include "rendering/renders/shaders/BaseLitEntityShader.h"
#include "scene/editor_scene/ParticleEmitterElement.h"

// Forward declaration
namespace EditorScene { class ParticleEmitterElement; }

namespace ParticleRenderer {

    // Maximum number of particles to render in a single draw call
    static constexpr size_t MAX_PARTICLES_PER_DRAW = 100000;

    struct ParticleInstance {
        glm::vec3 position;
        float size;
        glm::vec4 color;
        float rotation;
    };

    class ParticleShader : public ShaderInterface {
    public:
        ParticleShader();
        
        // Uniform locations
        int view_matrix_location{};
        int projection_matrix_location{};
        int projection_view_matrix_location{};
        
    private:
        bool init_shader();
        void get_uniforms_set_bindings();
    };

    class ParticleRenderer {
        ParticleShader shader;
        unsigned int vao{0}, vbo{0};
        size_t num_particles_to_render{0};

    public:
        ParticleRenderer();
        ~ParticleRenderer();

        void prepare_frame(const std::vector<std::shared_ptr<EditorScene::ParticleEmitterElement>>& particle_systems, const Window& window, const BaseEntityGlobalData& global_data);
        void render(const BaseEntityGlobalData& global_data);
        
        bool refresh_shaders();
    };
}

#endif