#ifndef GLTF_VIEWER_TUTORIAL_VIEWFRAME_HPP
#define GLTF_VIEWER_TUTORIAL_VIEWFRAME_HPP

#include <glm/glm.hpp>


namespace utils::camera {
    
    struct ViewFrame {
        glm::vec3 left;
        glm::vec3 up;
        glm::vec3 front;
        glm::vec3 eye;
        
        
        ViewFrame(glm::vec3 l, glm::vec3 u, glm::vec3 f, glm::vec3 e);
        
        
        static ViewFrame fromViewToWorldMatrix(const glm::mat4 &viewToWorldMatrix);
    };
}
#endif // GLTF_VIEWER_TUTORIAL_VIEWFRAME_HPP
