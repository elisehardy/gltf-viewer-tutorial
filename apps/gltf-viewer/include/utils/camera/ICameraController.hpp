#ifndef GLTF_VIEWER_TUTORIAL_ICAMERACONTROLLER_HPP
#define GLTF_VIEWER_TUTORIAL_ICAMERACONTROLLER_HPP

#include <glm/glm.hpp>
#include <utils/camera/Camera.hpp>

#include <GLFW/glfw3.h>


namespace utils::camera {
    
    class ICameraController {
        protected:
            GLFWwindow *window = nullptr;
            Camera camera;
            
            glm::vec3 worldUpAxis;
            glm::dvec2 lastCursorPosition = glm::dvec2();
            bool leftButtonPressed = false;
            bool middleButtonPressed = false;
            bool rightButtonPressed = false;
        
        protected:
            
            [[nodiscard]] glm::dvec2 computeCursorDelta();
        
        public:
            
            explicit ICameraController(GLFWwindow *window, const glm::vec3 &worldUpAxis = glm::vec3(0, 1, 0));
            
            virtual ~ICameraController() = default;
            
            void setCamera(const Camera &camera);
            
            [[nodiscard]]  Camera &getCamera();
            
            void setWorldUpAxis(const glm::vec3 &worldUpAxis);
            
            [[nodiscard]] const glm::vec3 &getWorldUpAxis() const;
            
            virtual bool update(GLfloat elapsedTime) = 0;
    };
}
#endif //GLTF_VIEWER_TUTORIAL_ICAMERACONTROLLER_HPP
