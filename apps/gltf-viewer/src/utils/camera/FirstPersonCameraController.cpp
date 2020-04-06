#include <utils/camera/FirstPersonCameraController.hpp>


namespace utils::camera {
    
    FirstPersonCameraController::FirstPersonCameraController(GLFWwindow *window, GLfloat speed,
                                                             const glm::vec3 &worldUpAxis) :
            ICameraController(window, worldUpAxis), speed(speed) {
    }
    
    
    void FirstPersonCameraController::setSpeed(GLfloat speed) {
        this->speed = speed;
    }
    
    
    void FirstPersonCameraController::increaseSpeed(GLfloat delta) {
        this->speed += delta;
        this->speed = glm::max(this->speed, 0.f);
    }
    
    
    GLfloat FirstPersonCameraController::getSpeed() const {
        return this->speed;
    }
    
    
    bool FirstPersonCameraController::update(GLfloat elapsedTime) {
        if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_LEFT) && !this->leftButtonPressed) {
            this->leftButtonPressed = true;
            glfwGetCursorPos(this->window, &this->lastCursorPosition.x, &this->lastCursorPosition.y);
        }
        else if (!glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_LEFT) && this->leftButtonPressed) {
            this->leftButtonPressed = false;
        }
        const glm::dvec2 cursorDelta = this->leftButtonPressed ? computeCursorDelta() : glm::dvec2(0);
        
        GLfloat truckLeft = 0.f;
        GLfloat pedestalUp = 0.f;
        GLfloat dollyIn = 0.f;
        GLfloat rollRightAngle = 0.f;
        bool hasMoved = false;
        
        if (glfwGetKey(this->window, GLFW_KEY_W)) {
            dollyIn += this->speed * elapsedTime;
            hasMoved = true;
        }
        
        // Truck left
        if (glfwGetKey(this->window, GLFW_KEY_A)) {
            truckLeft += this->speed * elapsedTime;
            hasMoved = true;
        }
        
        // Pedestal up
        if (glfwGetKey(this->window, GLFW_KEY_UP)) {
            pedestalUp += this->speed * elapsedTime;
            hasMoved = true;
        }
        
        // Dolly out
        if (glfwGetKey(window, GLFW_KEY_S)) {
            dollyIn -= this->speed * elapsedTime;
            hasMoved = true;
        }
        
        // Truck right
        if (glfwGetKey(window, GLFW_KEY_D)) {
            truckLeft -= this->speed * elapsedTime;
            hasMoved = true;
        }
        
        // Pedestal down
        if (glfwGetKey(window, GLFW_KEY_DOWN)) {
            pedestalUp -= this->speed * elapsedTime;
            hasMoved = true;
        }
        
        if (glfwGetKey(this->window, GLFW_KEY_Q)) {
            rollRightAngle -= 0.001f;
            hasMoved = true;
        }
        if (glfwGetKey(this->window, GLFW_KEY_E)) {
            rollRightAngle += 0.001f;
            hasMoved = true;
        }
        
        // cursor going right, so minus because we want pan left angle:
        GLfloat panLeftAngle = 0.f, tiltDownAngle = 0.f;
        if (static_cast<bool>(cursorDelta.x) || static_cast<bool>(cursorDelta.y)) {
            panLeftAngle = -0.01f * static_cast<GLfloat>(cursorDelta.x);
            tiltDownAngle = 0.01f * static_cast<GLfloat>(cursorDelta.y);
            hasMoved = true;
        }
        
        if (!hasMoved) {
            return false;
        }
        
        this->camera.moveLocal(truckLeft, pedestalUp, dollyIn);
        this->camera.rotateLocal(rollRightAngle, tiltDownAngle, 0.f);
        this->camera.rotateWorld(panLeftAngle, this->worldUpAxis);
        
        return true;
    }
}
