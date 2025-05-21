#include "ParticleRenderer.h"
#include "rendering/cameras/CameraInterface.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <glad/gl.h>
#include "scene/editor_scene/ParticleEmitterElement.h"

namespace ParticleRenderer {

ParticleShader::ParticleShader() : 
    ShaderInterface("Particle", "particle/vert.glsl", "particle/frag.glsl", 
                   [&]() { get_uniforms_set_bindings(); }) {
}

bool ParticleShader::init_shader() {
    return true;
}

void ParticleShader::get_uniforms_set_bindings() {
    projection_matrix_location = get_uniform_location("projection_matrix");
}

ParticleRenderer::ParticleRenderer() {
    // Create VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBO for particle data
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Allocate buffer for maximum number of particles
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES_PER_DRAW * sizeof(ParticleInstance), nullptr, GL_DYNAMIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleInstance), (void*)offsetof(ParticleInstance, position));
    
    // Size
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleInstance), (void*)offsetof(ParticleInstance, size));
    
    // Colour
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleInstance), (void*)offsetof(ParticleInstance, color));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
    std::vector<ParticleInstance> instances;
    instances.reserve(MAX_PARTICLES_PER_DRAW);

    std::cout << "[ParticleRenderer] prepare_frame called with " << particle_systems.size() << " particle systems." << std::endl;
    
    for (const auto& system : particle_systems) {
        if (!system || !system) {
            std::cout << "[ParticleRenderer] System " << system->name << " encountered a null or disabled system." << std::endl;
            continue;
        };

        if (!system->enabled) {
            std::cout << "[ParticleRenderer] System " << system->name << " is disabled." << std::endl;
            continue;
        }

        std::cout << "[Renderer] Processing system: " << system->name << "with" << system->particles.size() << " particles." << std::endl;

        if (system->particles.empty() && system->enabled) {
            std::cout << "[Renderer] System " << system->name << " is enabled but has no particles." << std::endl;
        }

        glm::mat4 emitter_transform = glm::mat4(1.0f);
        /*
        if (!system->worldSpaceParticles) {
            // If particles are in local space, their positions in ParticleInstance should be relative to the emitter.
            // The emitter's transform will be applied in the shader *if* we decide to pass it.
            // For now, assuming a_position IS world space OR the shader handles local->world.
            // The current vert shader expects a_position to be world.
            // So if particles are local, their positions need to be transformed to world here.
        }
        */


        for (const auto& p : system->particles) {
            if (instances.size() >= MAX_PARTICLES_PER_DRAW) {
                std::cout << "[Renderer] MAX_PARTICLES_PER_DRAW reached." << std::endl;
                break; 
            }

            glm::vec3 final_position = p.position;
            if (!system->worldSpaceParticles) {
                 final_position = glm::vec3(system->transform * glm::vec4(p.position, 1.0f));
            }


            // Create particle instance
            instances.push_back({
                final_position, // Use potentially transformed position
                p.size,
                p.color
            });
        }
        if (instances.size() >= MAX_PARTICLES_PER_DRAW) {
            break;
        }
    }

    // Upload particle data to GPU
    if (!instances.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, instances.size() * sizeof(ParticleInstance), instances.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    num_particles_to_render = instances.size();
}

void ParticleRenderer::render(const BaseEntityGlobalData& global_data) {
    if (num_particles_to_render == 0) {
        return;
    }

    shader.use();

    // Set view and projection matrices
    glUniformMatrix4fv(shader.projection_view_matrix_location, 1, GL_FALSE, glm::value_ptr(global_data.projection_view_matrix));
    
    // Set up blending for particles
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Enable point size
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    // Draw particles
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, num_particles_to_render);
    glGetError();
    glBindVertexArray(0);

    // Restore OpenGL state
    glDisable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_BLEND);
    glUseProgram(0);
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