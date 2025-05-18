#include "ParticleRenderer.h"
#include "rendering/cameras/CameraInterface.h"
#include <glm/gtc/matrix_transform.hpp> // For billboard math
#include <glm/gtc/type_ptr.hpp> // For glm::value_ptr
#include <iostream> // For error messages
#include <glad/gl.h>
#include "scene/editor_scene/ParticleEmitterElement.h"

namespace ParticleRenderer {

// --- ParticleVertexData --- 
void ParticleVertexData::setup_attrib_pointers() {
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleVertexData), (void*)offsetof(ParticleVertexData, position));
    // Texture Coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleVertexData), (void*)offsetof(ParticleVertexData, tex_coord));
    // Color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertexData), (void*)offsetof(ParticleVertexData, color));
}


ParticleShader::ParticleShader() : 
    ShaderInterface("Particle", "particle/vert.glsl", "particle/frag.glsl", 
                   [&]() { get_uniforms_set_bindings(); }) {
}

bool ParticleShader::init_shader() {
    return true;
}

void ParticleShader::get_uniforms_set_bindings() {
    view_matrix_location = get_uniform_location("view_matrix");
    projection_matrix_location = get_uniform_location("projection_matrix");
    particle_texture_sampler_location = get_uniform_location("particle_texture_sampler");
    
    if (particle_texture_sampler_location != -1) {
        set_binding("particle_texture_sampler", 0); // Texture unit 0
    }
}


ParticleRenderer::ParticleRenderer() {
    particle_vertex_buffer_data.reserve(MAX_PARTICLES_PER_DRAW * 6); // 6 vertices per particle (quad)

    // Create VAO and VBO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Allocate buffer data
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES_PER_DRAW * 6 * sizeof(ParticleVertexData), nullptr, GL_DYNAMIC_DRAW);

    ParticleVertexData::setup_attrib_pointers();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ParticleRenderer::~ParticleRenderer() {
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    
}

void ParticleRenderer::prepare_frame(const std::vector<std::shared_ptr<EditorScene::ParticleEmitterElement>>& particle_systems, const Window& window, const BaseEntityGlobalData& global_data) {
    particle_vertex_buffer_data.clear();
    
    currentBlendMode = EditorScene::ParticleBlendMode::AlphaBlend; // Default
    
    // Use actual camera orientation vectors for billboarding from global_data
    glm::vec3 camera_right_ws = global_data.camera_right;
    glm::vec3 camera_up_ws    = global_data.camera_up;
    // We rely on CameraInterface providing orthonormal vectors for front, up, right.
    
    for (const auto& system : particle_systems) {
        if (!system || !system->enabled || !system->textureHandle) continue;

        currentTextureHandle = system->textureHandle;
        currentBlendMode = system->blendMode;

        const auto& particles = system->particles;

        for (const auto& p : particles) {
            if (particle_vertex_buffer_data.size() + 6 > MAX_PARTICLES_PER_DRAW * 6) {
                break; 
            }

            glm::vec3 particle_center_ws = p.position;
            
            bool hasParent = system->parent != EditorScene::NullElementRef;
            bool isWorldSpace = system->worldSpaceParticles;
            
            if (!isWorldSpace && hasParent) {
                 const auto& element = *(system->parent);
                 glm::mat4 parent_transform_matrix = element->transform; 
                 if (!isWorldSpace) {
                    particle_center_ws = glm::vec3(system->transform * glm::vec4(p.position, 1.0f));
                 }
            }
            
            float current_size = p.size;
            
            glm::vec3 half_right = camera_right_ws * (current_size * 0.5f);
            glm::vec3 half_up = camera_up_ws * (current_size * 0.5f);

            glm::vec3 v0_pos = particle_center_ws - half_right - half_up;
            glm::vec3 v1_pos = particle_center_ws + half_right - half_up;
            glm::vec3 v2_pos = particle_center_ws - half_right + half_up;
            glm::vec3 v3_pos = particle_center_ws + half_right + half_up;

            particle_vertex_buffer_data.push_back({v0_pos, glm::vec2(0,0), p.color});
            particle_vertex_buffer_data.push_back({v1_pos, glm::vec2(1,0), p.color});
            particle_vertex_buffer_data.push_back({v2_pos, glm::vec2(0,1), p.color});

            particle_vertex_buffer_data.push_back({v1_pos, glm::vec2(1,0), p.color});
            particle_vertex_buffer_data.push_back({v3_pos, glm::vec2(1,1), p.color});
            particle_vertex_buffer_data.push_back({v2_pos, glm::vec2(0,1), p.color});
        }
        
        if (particle_vertex_buffer_data.size() + 6 > MAX_PARTICLES_PER_DRAW * 6) break;
    }

    if (!particle_vertex_buffer_data.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, particle_vertex_buffer_data.size() * sizeof(ParticleVertexData), particle_vertex_buffer_data.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void ParticleRenderer::render(const BaseEntityGlobalData& global_data) {
    if (particle_vertex_buffer_data.empty()) {
        return;
    }

    shader.use();

    // For view matrix, use identity matrix
    glm::mat4 identity_matrix = glm::mat4(1.0f);
    glUniformMatrix4fv(shader.view_matrix_location, 1, GL_FALSE, glm::value_ptr(identity_matrix));
    glUniformMatrix4fv(shader.projection_matrix_location, 1, GL_FALSE, glm::value_ptr(global_data.projection_view_matrix));
    
    // Disable depth testing completely for particles
    glDisable(GL_DEPTH_TEST);
    
    // Set up blending
    glEnable(GL_BLEND);
    // Apply the correct blend mode
    if (currentBlendMode == EditorScene::ParticleBlendMode::Additive) {
        // Additive blending (bright, glowy effect)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    } else {
        // Alpha blending (standard transparent effect)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Bind the particle texture
    if (currentTextureHandle) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTextureHandle->get_texture_id());
    }

    // Update VBO with billboarded vertices
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particle_vertex_buffer_data.size() * sizeof(ParticleVertexData), particle_vertex_buffer_data.data());
    
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(particle_vertex_buffer_data.size()));
    glBindVertexArray(0);

    // Restore OpenGL state
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glUseProgram(0); // Unbind shader
}

bool ParticleRenderer::refresh_shaders() {
    shader.cleanup();
    bool success = shader.reload_files();
    if (!success) {
        std::cerr << "Error: ParticleRenderer failed to refresh shader." << std::endl;
    }
    return success;
}

}