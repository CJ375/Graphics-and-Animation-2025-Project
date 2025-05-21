#include "EntityElement.h"



#include "rendering/imgui/ImGuiManager.h"

#include "scene/SceneContext.h"



std::unique_ptr<EditorScene::EntityElement> EditorScene::EntityElement::new_default(const SceneContext& scene_context, ElementRef parent) {
    auto rendered_entity = EntityRenderer::Entity::create(
        scene_context.model_loader.load_from_file<EntityRenderer::VertexData>("cube.obj"),
        EntityRenderer::InstanceData{
            glm::mat4{}, // Set via update_instance_data()
            EntityRenderer::EntityMaterial{
                {1.0f, 1.0f, 1.0f, 1.0f},
                {1.0f, 1.0f, 1.0f, 1.0f},
                {1.0f, 1.0f, 1.0f, 1.0f},
                512.0f,
            }

        },

        EntityRenderer::RenderData{
            scene_context.texture_loader.default_white_texture(),
            scene_context.texture_loader.default_white_texture()
        }

    );



    auto new_entity = std::make_unique<EntityElement>(
        parent,
        "New Entity",
        glm::vec3{0.0f},
        glm::vec3{0.0f},
        glm::vec3{1.0f},

        rendered_entity
    );

    new_entity->update_instance_data();

    return new_entity;

}



std::unique_ptr<EditorScene::EntityElement> EditorScene::EntityElement::from_json(const SceneContext& scene_context, EditorScene::ElementRef parent, const json& j) {

    auto new_entity = new_default(scene_context, parent);
    new_entity->update_local_transform_from_json(j);
    new_entity->update_material_from_json(j);

    new_entity->rendered_entity->model = scene_context.model_loader.load_from_file<EntityRenderer::VertexData>(j["model"]);
    new_entity->rendered_entity->render_data.diffuse_texture = texture_from_json(scene_context, j["diffuse_texture"]);
    new_entity->rendered_entity->render_data.specular_map_texture = texture_from_json(scene_context, j["specular_map_texture"]);

    new_entity->update_instance_data();

    return new_entity;

}



json EditorScene::EntityElement::into_json() const {
    if (!rendered_entity->model->get_filename().has_value()) {
        return {
            {"error", Formatter() << "Entity [" << name << "]'s model does not have a filename so can not be exported, and has been skipped."}
        };

    }



    return {
        local_transform_into_json(),
        material_into_json(),

        {"model", rendered_entity->model->get_filename().value()},
        {"diffuse_texture", texture_to_json(rendered_entity->render_data.diffuse_texture)},
        {"specular_map_texture", texture_to_json(rendered_entity->render_data.specular_map_texture)},

    };

}



void EditorScene::EntityElement::add_imgui_edit_section(MasterRenderScene& render_scene, const SceneContext& scene_context) {
    ImGui::Text("Entity");
    SceneElement::add_imgui_edit_section(render_scene, scene_context);
    add_local_transform_imgui_edit_section(render_scene, scene_context);
    add_material_imgui_edit_section(render_scene, scene_context);

    // Task D - Material Properties

    // Diffuse properties
    ImGui::Text("Diffuse");
    ImGui::ColorEdit3("Diffuse Tint", &rendered_entity->instance_data.material.diffuse_tint[0]);
    ImGui::DragFloat("Diffuse Factor", &rendered_entity->instance_data.material.diffuse_tint.a, 0.01f, 0.0f, 10.0f);
    ImGui::Spacing();

    // Specular properties
    ImGui::Text("Specular");
    ImGui::ColorEdit3("Specular Tint", &rendered_entity->instance_data.material.specular_tint[0]);
    ImGui::DragFloat("Specular Factor", &rendered_entity->instance_data.material.specular_tint.a, 0.01f, 0.0f, 10.0f);
    ImGui::Spacing();

    // Ambient properties
    ImGui::Text("Ambient");
    ImGui::ColorEdit3("Ambient Tint", &rendered_entity->instance_data.material.ambient_tint[0]);
    ImGui::DragFloat("Ambient Factor", &rendered_entity->instance_data.material.ambient_tint.a, 0.01f, 0.0f, 10.0f);
    ImGui::Spacing();

    // Shininess properties
    ImGui::DragFloat("Shininess", &rendered_entity->instance_data.material.shininess, 1.0f, 0.0f, 2.0f);
    ImGui::Spacing();

    ImGui::DragDisableCursor(scene_context.window);
    ImGui::Spacing();

    ImGui::Text("Model & Textures");
    scene_context.model_loader.add_imgui_model_selector("Model Selection", rendered_entity->model);
    scene_context.texture_loader.add_imgui_texture_selector("Diffuse Texture", rendered_entity->render_data.diffuse_texture);
    scene_context.texture_loader.add_imgui_texture_selector("Specular Map", rendered_entity->render_data.specular_map_texture, false);
}



void EditorScene::EntityElement::update_instance_data() {
    transform = calc_model_matrix();

    if (!EditorScene::is_null(parent)) {
        // Post multiply by transform so that local transformations are applied first
        transform = (*parent)->transform * transform;
    }



    rendered_entity->instance_data.model_matrix = transform;
    rendered_entity->instance_data.material = material;
}



const char* EditorScene::EntityElement::element_type_name() const {
    return ELEMENT_TYPE_NAME;
}