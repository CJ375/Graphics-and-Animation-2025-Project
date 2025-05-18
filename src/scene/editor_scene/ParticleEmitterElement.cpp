#include "ParticleEmitterElement.h"
#include "rendering/imgui/ImGuiManager.h" // For ImGui widgets
#include "utility/Random.h"
#include <glm/gtc/type_ptr.hpp> // For ImGui::ColorEdit4
#include <iostream>

namespace EditorScene {


ParticleEmitterElement::ParticleEmitterElement(const ElementRef& parent, std::string name)
    : SceneElement(parent, std::move(name)), 
      LocalTransformComponent(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f)) {
    // Particles vector is initialized empty by default
    // textureHandle will be loaded by new_default or from_json
}


std::unique_ptr<ParticleEmitterElement> ParticleEmitterElement::new_default(const SceneContext& scene_context, ElementRef parent) {
    auto element = std::make_unique<ParticleEmitterElement>(parent, "New Particle Emitter");
    element->load_particle_texture(scene_context);
    element->update_instance_data(); // Update initial transform
    return element;
}

std::unique_ptr<ParticleEmitterElement> ParticleEmitterElement::from_json(const SceneContext& scene_context, ElementRef parent, const json& j) {
    auto element = new_default(scene_context, parent); // Start with defaults

    element->update_local_transform_from_json(j);

    element->emissionRate = j.value("emissionRate", element->emissionRate);
    element->maxParticles = j.value("maxParticles", element->maxParticles);
    element->particleLifespanMin = j.value("particleLifespanMin", element->particleLifespanMin);
    element->particleLifespanMax = j.value("particleLifespanMax", element->particleLifespanMax);
    element->initialVelocityMin = j.value("initialVelocityMin", element->initialVelocityMin);
    element->initialVelocityMax = j.value("initialVelocityMax", element->initialVelocityMax);
    element->initialSizeMin = j.value("initialSizeMin", element->initialSizeMin);
    element->initialSizeMax = j.value("initialSizeMax", element->initialSizeMax);
    element->endSizeFactor = j.value("endSizeFactor", element->endSizeFactor);
    element->initialColorStart = j.value("initialColorStart", element->initialColorStart);
    element->initialColorEnd = j.value("initialColorEnd", element->initialColorEnd);
    element->endColor = j.value("endColor", element->endColor);
    element->gravity = j.value("gravity", element->gravity);
    element->particleTexturePath = j.value("particleTexturePath", element->particleTexturePath);
    element->blendMode = j.value("blendMode", element->blendMode);
    element->worldSpaceParticles = j.value("worldSpaceParticles", element->worldSpaceParticles);

    element->load_particle_texture(scene_context);
    element->update_instance_data();
    return element;
}
        
json ParticleEmitterElement::into_json() const {
    json j = local_transform_into_json();
    j["emissionRate"] = emissionRate;
    j["maxParticles"] = maxParticles;
    j["particleLifespanMin"] = particleLifespanMin;
    j["particleLifespanMax"] = particleLifespanMax;
    j["initialVelocityMin"] = initialVelocityMin;
    j["initialVelocityMax"] = initialVelocityMax;
    j["initialSizeMin"] = initialSizeMin;
    j["initialSizeMax"] = initialSizeMax;
    j["endSizeFactor"] = endSizeFactor;
    j["initialColorStart"] = initialColorStart;
    j["initialColorEnd"] = initialColorEnd;
    j["endColor"] = endColor;
    j["gravity"] = gravity;
    j["particleTexturePath"] = particleTexturePath;
    j["blendMode"] = blendMode;
    j["worldSpaceParticles"] = worldSpaceParticles;
    return j;
}

void ParticleEmitterElement::add_imgui_edit_section(MasterRenderScene& render_scene, const SceneContext& scene_context) {
    SceneElement::add_imgui_edit_section(render_scene, scene_context);
    add_local_transform_imgui_edit_section(render_scene, scene_context);

    ImGui::Separator();
    ImGui::Text("Particle Emitter Properties");

    ImGui::DragFloat("Emission Rate##PE", &emissionRate, 0.1f, 0.0f, 1000.0f);
    ImGui::DragInt("Max Particles##PE", &maxParticles, 1, 0, 10000);
    ImGui::DragFloatRange2("Lifespan (s)##PE", &particleLifespanMin, &particleLifespanMax, 0.01f, 0.0f, 60.0f);
    
    ImGui::Text("Initial Velocity Min##PE"); ImGui::DragFloat3("##IVelMinPE", &initialVelocityMin[0], 0.01f);
    ImGui::Text("Initial Velocity Max##PE"); ImGui::DragFloat3("##IVelMaxPE", &initialVelocityMax[0], 0.01f);
    
    ImGui::DragFloatRange2("Initial Size##PE", &initialSizeMin, &initialSizeMax, 0.001f, 0.001f, 10.0f);
    ImGui::DragFloat("End Size Factor##PE", &endSizeFactor, 0.01f, 0.0f, 10.0f);

    ImGui::Text("Initial Color Start##PE"); ImGui::ColorEdit4("##IColStartPE", glm::value_ptr(initialColorStart));
    ImGui::Text("Initial Color End##PE");   ImGui::ColorEdit4("##IColEndPE", glm::value_ptr(initialColorEnd));
    ImGui::Text("End Color##PE");           ImGui::ColorEdit4("##EndColPE", glm::value_ptr(endColor));

    ImGui::Text("Gravity##PE"); ImGui::DragFloat3("##GravityPE", &gravity[0], 0.01f);

    scene_context.texture_loader.add_imgui_texture_selector("Particle Texture##PE", textureHandle);

    if (textureHandle && textureHandle->get_filename().has_value()) {
        particleTexturePath = textureHandle->get_filename().value();
    } else if (textureHandle) {
        if ( (textureHandle == scene_context.texture_loader.default_white_texture() || 
              textureHandle == scene_context.texture_loader.default_black_texture()) && 
             !particleTexturePath.empty() && 
             (particleTexturePath != "default_white" && particleTexturePath != "default_black")
           ) {
        }
    }

    ImGui::Text("Blend Mode##PE");
    if (ImGui::RadioButton("Alpha Blend##PE", blendMode == ParticleBlendMode::AlphaBlend)) { blendMode = ParticleBlendMode::AlphaBlend; }
    ImGui::SameLine();
    if (ImGui::RadioButton("Additive##PE", blendMode == ParticleBlendMode::Additive)) { blendMode = ParticleBlendMode::Additive; }
    
    ImGui::Checkbox("World Space Particles##PE", &worldSpaceParticles);
    ImGui::DragDisableCursor(scene_context.window);
}

void ParticleEmitterElement::update_instance_data() {
    transform = calc_model_matrix();
    if (!EditorScene::is_null(parent)) {
        transform = (*parent)->transform * transform;
    }
}

void ParticleEmitterElement::add_to_render_scene(MasterRenderScene& target_render_scene) {
    target_render_scene.add_particle_system(this);
}

void ParticleEmitterElement::remove_from_render_scene(MasterRenderScene& target_render_scene) {
    target_render_scene.remove_particle_system(this);
}

const char* ParticleEmitterElement::element_type_name() const {
    return ELEMENT_TYPE_NAME;
}

void ParticleEmitterElement::tick_particles(float deltaTime, const SceneContext&) {
    if (!enabled) return;

    emissionTimer += deltaTime;
    int particlesToEmit = static_cast<int>(emissionTimer * emissionRate);
    if (particlesToEmit > 0) {
        emissionTimer -= static_cast<float>(particlesToEmit) / emissionRate;
    }

    // Force emit at least 5 particles for testing
    particlesToEmit = std::max(particlesToEmit, 5);
    
    glm::vec3 emitter_position = glm::vec3(transform[3]);

    for (int i = 0; i < particlesToEmit && particles.size() < static_cast<size_t>(maxParticles); ++i) {
        Particle p;
        p.totalLife = Random::range(particleLifespanMin, particleLifespanMax);
        p.lifeRemaining = p.totalLife;

        float spread = 1.0f;
        glm::vec3 randomOffset(
            Random::range(-spread, spread),
            Random::range(-spread, spread),
            Random::range(-spread, spread)
        );
        
        if (worldSpaceParticles) {
            p.position = emitter_position + randomOffset;
        } else {
            p.position = randomOffset;
        }
        
        // Initial velocity
        glm::vec3 randomVelocity;
        randomVelocity.x = Random::range(initialVelocityMin.x, initialVelocityMax.x);
        randomVelocity.y = Random::range(initialVelocityMin.y, initialVelocityMax.y);
        randomVelocity.z = Random::range(initialVelocityMin.z, initialVelocityMax.z);
 
        p.velocity = randomVelocity;

        // Make particles larger for visibility
        p.size = Random::range(initialSizeMin, initialSizeMax) * 10.0f;
        
        // Make particles brighter
        float colorLerpFactor = Random::range(0.0f, 1.0f);
        
        // White particles at full opacity
        glm::vec4 brightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        p.color = brightColor;

        particles.push_back(p);
    }

    for (size_t i = 0; i < particles.size();) {
        Particle& p = particles[i];
        p.lifeRemaining -= deltaTime;

        if (p.lifeRemaining <= 0.0f) {
            particles.erase(particles.begin() + i);
        } else {
            p.velocity += gravity * deltaTime;
            p.position += p.velocity * deltaTime;
            
            float lifeRatio = 1.0f - (p.lifeRemaining / p.totalLife);
            glm::vec4 colorAtEmission = p.color;

            if (lifeRatio > 0.8f) {
                float fadeAlpha = (1.0f - lifeRatio) * 5.0f;
                p.color.a = fadeAlpha;
            }
            
            float sizeAtEmission = p.size;
            p.size = glm::mix(sizeAtEmission, sizeAtEmission * endSizeFactor, lifeRatio);
            
            i++;
        }
    }
}

void ParticleEmitterElement::load_particle_texture(const SceneContext& scene_context) {
    std::cout << "Loading particle texture: " << particleTexturePath << std::endl;
    
    if (!particleTexturePath.empty()) {
        textureHandle = scene_context.texture_loader.load_from_file(particleTexturePath, true);
    }
    
    if (!textureHandle) {
        std::cout << "Texture loading failed, using default white texture" << std::endl;
        // Fallback to default white if loading failed or path is empty
        textureHandle = scene_context.texture_loader.default_white_texture();
    } else {
        std::cout << "Texture loaded successfully, ID: " << textureHandle->get_texture_id() << std::endl;
    }
}

} // namespace EditorScene 