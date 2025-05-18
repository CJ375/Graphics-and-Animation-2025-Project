#ifndef BASE_ENTITY_SHADER_H
#define BASE_ENTITY_SHADER_H

#include <utility>
#include <vector>
#include <unordered_set>

#include "glm/glm.hpp"

#include "ShaderInterface.h"
#include "rendering/scene/Lights.h"
#include "rendering/scene/GlobalData.h"
#include "rendering/scene/RenderScene.h"
#include "rendering/scene/RenderedEntity.h"
#include "rendering/resources/ModelLoader.h"
#include "rendering/resources/TextureHandle.h"
#include "rendering/memory/UniformBufferArray.h"
#include "rendering/cameras/CameraInterface.h"

struct BaseEntityInstanceData {
    explicit BaseEntityInstanceData(const glm::mat4& model_matrix) : model_matrix(model_matrix) {}

    glm::mat4 model_matrix;
};

struct BaseEntityGlobalData : public GlobalDataCameraInterface {
    BaseEntityGlobalData() = default;

    BaseEntityGlobalData(const glm::mat4& projection_view_matrix, const glm::vec3& camera_position, float gamma,
                         const glm::vec3& camera_front, const glm::vec3& camera_up, const glm::vec3& camera_right)
        : projection_view_matrix(projection_view_matrix), camera_position(camera_position), gamma(gamma),
          camera_front(camera_front), camera_up(camera_up), camera_right(camera_right) {}

    glm::mat4 projection_view_matrix{};
    glm::vec3 camera_position{};
    float gamma = 1.0f;
    glm::vec3 camera_front{};
    glm::vec3 camera_up{};
    glm::vec3 camera_right{};

    void use_camera(const CameraInterface& camera_interface) override {
        glm::mat4 view_matrix = camera_interface.get_view_matrix();
        projection_view_matrix = camera_interface.get_projection_matrix() * view_matrix;
        camera_position = camera_interface.get_position();
        gamma = camera_interface.get_gamma();
        
        camera_right = glm::vec3(view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
        camera_up = glm::vec3(view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]);
        camera_front = -glm::vec3(view_matrix[0][2], view_matrix[1][2], view_matrix[2][2]);
    }
};

class BaseEntityShader : public ShaderInterface {
protected:
    int model_matrix_location{};
    int projection_view_matrix_location{};
    // Global Data
    int ws_view_position_location{};
    int inverse_gamma_location{};
public:
    BaseEntityShader(std::string name, const std::string& vertex_path, const std::string& fragment_path,
                     std::unordered_map<std::string, std::string> vert_defines = {},
                     std::unordered_map<std::string, std::string> frag_defines = {});

    void set_instance_data(const BaseEntityInstanceData& instance_data);

    void set_global_data(const BaseEntityGlobalData& global_data);
protected:
    virtual void get_uniforms_set_bindings();
};

#endif //BASE_ENTITY_SHADER_H
