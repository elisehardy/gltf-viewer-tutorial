#ifndef GLTF_VIEWER_TUTORIAL_TRACKBALLCAMERACONTROLLER_HPP
#define GLTF_VIEWER_TUTORIAL_TRACKBALLCAMERACONTROLLER_HPP

#include <utils/camera/ICameraController.hpp>


namespace utils::camera {
    
    class TrackballCameraController : public ICameraController {
        private:
            glm::mat4 viewMatrix = glm::mat4(1);
            glm::mat4 RcpViewMatrix = glm::mat4(1);
        
        public:
            
            explicit TrackballCameraController(GLFWwindow *window);
            
            /**
             * Update the view matrix based on input events and elapsed time
             */
            bool update(GLfloat elapsedTime) override;
    };
}

#endif // GLTF_VIEWER_TUTORIAL_TRACKBALLCAMERACONTROLLER_HPP
