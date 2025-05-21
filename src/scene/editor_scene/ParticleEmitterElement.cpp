#include "ParticleEmitterElement.h"
#include "rendering/imgui/ImGuiManager.h"
#include "utility/Random.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace EditorScene {


ParticleEmitterElement::ParticleEmitterElement(const ElementRef& parent, std::string name)
    : SceneElement(parent, std::move(name)), 
      LocalTransformComponent(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f)) {
}


std::unique_ptr<ParticleEmitterElement> ParticleEmitterElement::new_default(const SceneContext& scene_context, ElementRef parent) {
    auto element = std::make_unique<ParticleEmitterElement>(parent, "New Particle Emitter");
    element->update_instance_data();
    return element;
}

std::unique_ptr<ParticleEmitterElement> ParticleEmitterElement::from_json(const SceneContext& scene_context, ElementRef parent, const json& j) {
    auto element = new_default(scene_context, parent);

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
    element->worldSpaceParticles = j.value("worldSpaceParticles", element->worldSpaceParticles);

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
    j["worldSpaceParticles"] = worldSpaceParticles;
    return j;
}

void ParticleEmitterElement::add_imgui_edit_section(MasterRenderScene& render_scene, const SceneContext& scene_context) {
    SceneElement::add_imgui_edit_section(render_scene, scene_context);
    add_local_transform_imgui_edit_section(render_scene, scene_context);

    ImGui::Separator();
    ImGui::Text("Particle Emitter Properties");

    ImGui::Checkbox("Enabled", &enabled);
    ImGui::Checkbox("World Space Particles", &worldSpaceParticles);
    ImGui::DragFloat("Emission Rate", &emissionRate, 0.1f, 0.0f, 1000.0f);
    ImGui::DragInt("Max Particles", &maxParticles, 1, 0, 10000);
    ImGui::DragFloatRange2("Lifespan (s)", &particleLifespanMin, &particleLifespanMax, 0.01f, 0.0f, 60.0f);
    
    ImGui::Text("Initial Velocity Min"); ImGui::DragFloat3("##IVelMin", &initialVelocityMin[0], 0.01f);
    ImGui::Text("Initial Velocity Max"); ImGui::DragFloat3("##IVelMax", &initialVelocityMax[0], 0.01f);
    
    ImGui::DragFloatRange2("Initial Size", &initialSizeMin, &initialSizeMax, 0.001f, 0.001f, 10.0f);
    ImGui::DragFloat("End Size Factor", &endSizeFactor, 0.01f, 0.0f, 10.0f);

    ImGui::Text("Initial Colour Start"); ImGui::ColorEdit3("##IColStart", glm::value_ptr(initialColorStart));
    ImGui::Text("Initial Colour End");   ImGui::ColorEdit3("##IColEnd", glm::value_ptr(initialColorEnd));
    ImGui::Text("End Colour");           ImGui::ColorEdit3("##EndCol", glm::value_ptr(endColor));

    ImGui::Text("Gravity"); ImGui::DragFloat3("##Gravity", &gravity[0], 0.01f);

    // Force controls
    if (ImGui::CollapsingHeader("Forces", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Wind Force"); ImGui::DragFloat3("##WindForce", &windForce[0], 0.01f);
        
        if (ImGui::CollapsingHeader("Attractor/Repulsor", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Position"); ImGui::DragFloat3("##AttractorPos", &attractorPosition[0], 0.01f);
            ImGui::DragFloat("Strength", &attractorStrength, 0.1f, -10.0f, 10.0f);
            ImGui::DragFloat("Radius", &attractorRadius, 0.1f, 0.1f, 100.0f);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Positive values attract, negative values repel");
            }
        }

        if (ImGui::CollapsingHeader("Turbulence", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat("Strength", &turbulenceStrength, 0.1f, 0.0f, 10.0f);
            ImGui::DragFloat("Frequency", &turbulenceFrequency, 0.1f, 0.1f, 10.0f);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("How often the turbulence force updates");
            }
        }
    }

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
    if (!enabled) {
        return;
    }

    emissionTimer += deltaTime;
    turbulenceTimer += deltaTime;

    if (turbulenceTimer >= 1.0f / turbulenceFrequency) {
        turbulenceTimer = 0.0f;
    }

    int particlesToEmit = static_cast<int>(emissionTimer * emissionRate);
    if (particlesToEmit > 0) {
        emissionTimer -= static_cast<float>(particlesToEmit) / emissionRate;
    }

    glm::vec3 emitter_position = glm::vec3(transform[3]);

    for (int i = 0; i < particlesToEmit && particles.size() < static_cast<size_t>(maxParticles); ++i) {
        Particle p;
        p.totalLife = Random::range(particleLifespanMin, particleLifespanMax);
        p.lifeRemaining = p.totalLife;

        float spread = 0.5f;
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
        
        // Initial particle velocity
        glm::vec3 randomVelocity;
        randomVelocity.x = Random::range(initialVelocityMin.x, initialVelocityMax.x);
        randomVelocity.y = Random::range(initialVelocityMin.y, initialVelocityMax.y);
        randomVelocity.z = Random::range(initialVelocityMin.z, initialVelocityMax.z);
 
        p.velocity = randomVelocity;

        // Size from UI parameters
        p.size = Random::range(initialSizeMin, initialSizeMax);
        
        // Rotation from UI parameters
        p.rotation = Random::range(initialRotationMin, initialRotationMax);
        p.angularVelocity = Random::range(angularVelocityMin, angularVelocityMax);
        
        // Colour from UI parameters
        float colorLerpFactor = Random::range(0.0f, 1.0f);
        p.color = glm::mix(initialColorStart, initialColorEnd, colorLerpFactor);

        particles.push_back(p);
    }

    bool first_updated = true;

    for (size_t i = 0; i < particles.size();) {
        Particle& p = particles[i];
        p.lifeRemaining -= deltaTime;

        if (p.lifeRemaining <= 0.0f) {
            particles.erase(particles.begin() + i);
        } else {
            if (first_updated) {
                first_updated = false;
            }

            // Apply gravity
            p.velocity += gravity * deltaTime;

            // Apply "wind" force
            p.velocity += windForce * deltaTime;

            // Apply attractor/repulsor force
            if (attractorStrength != 0.0f) {
                glm::vec3 toAttractor = attractorPosition - p.position;
                float distance = glm::length(toAttractor);
                if (distance < attractorRadius) {
                    float force = attractorStrength * (1.0f - distance / attractorRadius);
                    p.velocity += glm::normalize(toAttractor) * force * deltaTime;
                }
            }

            // Apply turbulence
            if (turbulenceStrength > 0.0f) {
                glm::vec3 turbulence(
                    Random::range(-1.0f, 1.0f),
                    Random::range(-1.0f, 1.0f),
                    Random::range(-1.0f, 1.0f)
                );
                p.velocity += glm::normalize(turbulence) * turbulenceStrength * deltaTime;
            }

            // Update position
            p.position += p.velocity * deltaTime;
            p.rotation += p.angularVelocity * deltaTime;
            
            float lifeRatio = 1.0f - (p.lifeRemaining / p.totalLife);
            
            glm::vec4 initialColor = p.color;
            p.color = glm::mix(initialColor, endColor, lifeRatio);
            
            float sizeAtEmission = p.size;
            p.size = glm::mix(sizeAtEmission, sizeAtEmission * endSizeFactor, lifeRatio);
            
            i++;
        }
    }
}

void ParticleEmitterElement::draw_properties() {
    ImGui::Checkbox("Enabled", &enabled);
    ImGui::Checkbox("World Space Particles", &worldSpaceParticles);
    ImGui::InputFloat("Emission Rate", &emissionRate, 0.1f, 1.0f);
    ImGui::InputInt("Max Particles", &maxParticles, 1, 10);
    ImGui::InputFloat("Particle Lifespan Min", &particleLifespanMin, 0.1f, 1.0f);
    ImGui::InputFloat("Particle Lifespan Max", &particleLifespanMax, 0.1f, 1.0f);
    ImGui::InputFloat("Initial Size Min", &initialSizeMin, 0.1f, 1.0f);
    ImGui::InputFloat("Initial Size Max", &initialSizeMax, 0.1f, 1.0f);
    ImGui::InputFloat("End Size Factor", &endSizeFactor, 0.1f, 1.0f);
    ImGui::InputFloat3("Initial Velocity Min", &initialVelocityMin.x);
    ImGui::InputFloat3("Initial Velocity Max", &initialVelocityMax.x);
    ImGui::InputFloat3("Gravity", &gravity.x);
    ImGui::ColorEdit4("Initial Color Start", &initialColorStart.x);
    ImGui::ColorEdit4("Initial Color End", &initialColorEnd.x);
    ImGui::ColorEdit4("End Color", &endColor.x);
    ImGui::InputFloat("Initial Rotation Min", &initialRotationMin, 1.0f, 10.0f);
    ImGui::InputFloat("Initial Rotation Max", &initialRotationMax, 1.0f, 10.0f);
    ImGui::InputFloat("Angular Velocity Min", &angularVelocityMin, 1.0f, 10.0f);
    ImGui::InputFloat("Angular Velocity Max", &angularVelocityMax, 1.0f, 10.0f);

    // Force controls
    if (ImGui::CollapsingHeader("Forces", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputFloat3("Wind Force", &windForce.x);
        
        if (ImGui::CollapsingHeader("Attractor/Repulsor", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputFloat3("Position", &attractorPosition.x);
            ImGui::InputFloat("Strength", &attractorStrength, 0.1f, 1.0f);
            ImGui::InputFloat("Radius", &attractorRadius, 0.1f, 1.0f);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Positive values attract, negative values repel");
            }
        }

        if (ImGui::CollapsingHeader("Turbulence", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputFloat("Strength", &turbulenceStrength, 0.1f, 1.0f);
            ImGui::InputFloat("Frequency", &turbulenceFrequency, 0.1f, 1.0f);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("How often the turbulence force updates");
            }
        }
    }
}

} // namespace EditorScene 