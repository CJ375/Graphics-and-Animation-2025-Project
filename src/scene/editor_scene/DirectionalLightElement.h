#ifndef DIRECTIONAL_LIGHT_ELEMENT_H
#define DIRECTIONAL_LIGHT_ELEMENT_H

#include "SceneElement.h"
#include "scene/SceneContext.h"

namespace EditorScene {
    class DirectionalLightElement : public SceneElement {
        public:
            static constexpr const char* ELEMENT_TYPE_NAME = "Directional Light";

            // Local transformation
            glm::vec3 position;
            
        
    }
}

#endif // DIRECTIONAL_LIGHT_ELEMENT_H
