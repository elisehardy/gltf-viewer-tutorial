#ifndef GLTF_VIEWER_TUTORIAL_FIRSTPERSONCAMERACONTROLLER_HPP
#define GLTF_VIEWER_TUTORIAL_FIRSTPERSONCAMERACONTROLLER_HPP

#include <utils/camera/ICameraController.hpp>


namespace utils::camera {
    
    class FirstPersonCameraController : public ICameraController {
        private:
            GLfloat speed;
        
        public:
            explicit FirstPersonCameraController(GLFWwindow *window, GLfloat speed = 1.f,
                                                 const glm::vec3 &worldUpAxis = glm::vec3(0, 1, 0));
            
            void setSpeed(GLfloat speed);
            
            void increaseSpeed(GLfloat delta);
            
            [[nodiscard]] GLfloat getSpeed() const;
            
            /**
             * Update the view matrix based on input events and elapsed time.
             *
             * @return true if the view matrix has been modified.
             */
            bool update(GLfloat elapsedTime) override;
    };
}

#endif // GLTF_VIEWER_TUTORIAL_FIRSTPERSONCAMERACONTROLLER_HPP
