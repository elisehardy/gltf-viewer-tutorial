#ifndef GLTF_VIEWER_TUTORIAL_LIGHT_HPP
#define GLTF_VIEWER_TUTORIAL_LIGHT_HPP

#include <glad/glad.h>
#include <glm/vec3.hpp>


namespace utils {
    
    struct Light {
        GLfloat phi = 1.f;
        GLfloat theta = 0.6f;
        GLfloat dirIntensity = 1.f;
        GLfloat ambIntensity = 0.07f;
        glm::vec3 color = glm::vec3(1.f);
        glm::vec3 direction;
        bool fromCamera = false;
        
        
        Light() {
            computeDirection();
        }
        
        
        void computeDirection() {
            const float sinPhi = glm::sin(this->phi);
            const float sinTheta = glm::sin(this->theta);
            const float cosPhi = glm::cos(this->phi);
            const float cosTheta = glm::cos(this->theta);
            
            this->direction = glm::vec3(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);
        }
    };
}
#endif // GLTF_VIEWER_TUTORIAL_LIGHT_HPP
