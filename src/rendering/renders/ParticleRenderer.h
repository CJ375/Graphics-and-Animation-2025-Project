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
    static constexpr size_t MAX_PARTICLES_PER_DRAW = 10000;

    struct ParticleVertexData {
        glm::vec3 position;
        glm::vec2 tex_coord;
        glm::vec4 color;

        static void setup_attrib_pointers();
    };

    class ParticleShader : public ShaderInterface {
    public:
        ParticleShader();
        
        // Uniform locations
        int view_matrix_location{};
        int projection_matrix_location{};
        int particle_texture_sampler_location{};
        
    private:
        bool init_shader();
        void get_uniforms_set_bindings();
    };

    class ParticleRenderer {
        ParticleShader shader;
        unsigned int vao{};
        unsigned int vbo{};
        std::vector<ParticleVertexData> particle_vertex_buffer_data;
        std::vector<float> particle_sizes;
        std::shared_ptr<TextureHandle> currentTextureHandle;

    public:
        ParticleRenderer();
        ~ParticleRenderer();

        void prepare_frame(const std::vector<std::shared_ptr<EditorScene::ParticleEmitterElement>>& particle_systems, const Window& window, const BaseEntityGlobalData& global_data);
        void render(const BaseEntityGlobalData& global_data);
        
        bool refresh_shaders();
    };
}

#endif