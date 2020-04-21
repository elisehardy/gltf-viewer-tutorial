#ifndef GLTF_VIEWER_TUTORIAL_CAMERA_HPP
#define GLTF_VIEWER_TUTORIAL_CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>


namespace utils::camera {
    
    // Camera defined by an eye position, a center position and an up vector
    class Camera {
        private:
            glm::vec3 eye;
            glm::vec3 center;
            glm::vec3 up;
        
        public:
            
            Camera() = default;
            
            Camera(glm::vec3 e, glm::vec3 c, glm::vec3 u);
            
            void truckLeft(GLfloat offset);
            
            void pedestalUp(GLfloat offset);
            
            void dollyIn(GLfloat offset);
            
            void moveLocal(GLfloat truckLeftOffset, GLfloat pedestalUpOffset, GLfloat dollyIn);
            
            void rollRight(GLfloat radians);
            
            void tiltDown(GLfloat radians);
            
            void panLeft(GLfloat radians);
            
            void rotateLocal(GLfloat rollRight, GLfloat tiltDown, GLfloat panLeft);
            
            void rotateWorld(GLfloat radians, const glm::vec3 &axis);
            
            [[nodiscard]] glm::mat4 getViewMatrix() const;
            
            [[nodiscard]] glm::vec3 getEye() const;
            
            [[nodiscard]] glm::vec3 getCenter() const;
            
            [[nodiscard]] glm::vec3 getUp() const;
            
            [[nodiscard]] glm::vec3 getFront(bool normalize = true) const;
            
            [[nodiscard]] glm::vec3 getLeft(bool normalize = true) const;
    };
}

#endif // GLTF_VIEWER_TUTORIAL_CAMERA_HPP
