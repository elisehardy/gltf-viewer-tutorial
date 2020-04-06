#include <utils/camera/TrackballCameraController.hpp>


namespace utils::camera {
    
    TrackballCameraController::TrackballCameraController(GLFWwindow *window) :
            ICameraController(window) {
    }
    
    
    bool TrackballCameraController::update(GLfloat elapsedTime) {
        if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_MIDDLE) && !this->middleButtonPressed) {
            this->middleButtonPressed = true;
            glfwGetCursorPos(this->window, &this->lastCursorPosition.x, &this->lastCursorPosition.y);
        }
        else if (!glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_MIDDLE) && this->middleButtonPressed) {
            this->middleButtonPressed = false;
        }
        const glm::dvec2 cursorDelta = this->middleButtonPressed ? computeCursorDelta() : glm::dvec2(0);
        
        // Pan
        if (glfwGetKey(this->window, GLFW_KEY_LEFT_SHIFT)) {
            const GLfloat truckLeft = 0.01f * static_cast<GLfloat>(cursorDelta.x);
            const GLfloat pedestalUp = 0.01f * static_cast<GLfloat>(cursorDelta.y);
            
            if (!(truckLeft || pedestalUp)) {
                return false;
            }
            
            this->camera.moveLocal(truckLeft, pedestalUp, 0.f);
            
            return true;
        }
        
        // Zoom
        if (glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL)) {
            GLfloat mouseOffset = 0.01f * static_cast<GLfloat>(cursorDelta.x);
            if (mouseOffset == 0.f) {
                return false;
            }
            
            const glm::vec3 viewVector = this->camera.getCenter() - this->camera.getEye();
            const GLfloat l = glm::length(viewVector);
            if (mouseOffset > 0.f) {
                mouseOffset = glm::min(mouseOffset, l - 1e-4f);
            }
            
            const glm::vec3 front = viewVector / l;
            const glm::vec3 translationVector = mouseOffset * front;
            
            // Update camera with new eye position
            const glm::vec3 newEye = this->camera.getEye() + translationVector;
            this->camera = Camera(newEye, this->camera.getCenter(), this->worldUpAxis);
            
            return true;
        }
        
        // Rotate around target
        const GLfloat longitudeAngle = 0.01f * static_cast<GLfloat>(cursorDelta.y); // Vertical angle
        const GLfloat latitudeAngle = -0.01f * static_cast<GLfloat>(cursorDelta.x); // Horizontal angle
        if (!(longitudeAngle || latitudeAngle)) {
            return false;
        }
        
        const glm::vec3 depthAxis = this->camera.getEye() - this->camera.getCenter();
        const glm::mat4 latitudeRotationMatrix = rotate(glm::mat4(1), latitudeAngle, this->worldUpAxis);
        const glm::vec3 horizontalAxis = this->camera.getLeft();
        const glm::mat4 rotationMatrix = rotate(latitudeRotationMatrix, longitudeAngle, horizontalAxis);
        const glm::vec3 rotatedDepthAxis = glm::vec3(rotationMatrix * glm::vec4(depthAxis, 0));
        const glm::vec3 newEye = this->camera.getCenter() + rotatedDepthAxis;
        
        this->camera = Camera(newEye, this->camera.getCenter(), this->worldUpAxis);
        
        return true;
    }
}
