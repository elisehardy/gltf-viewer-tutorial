#include <utils/camera/ICameraController.hpp>


namespace utils::camera {
    
    ICameraController::ICameraController(GLFWwindow *t_window, const glm::vec3 &t_worldUpAxis) :
            window(t_window), camera(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)),
            worldUpAxis(t_worldUpAxis) {
    }
    
    
    void ICameraController::setCamera(const Camera &camera) {
        this->camera = camera;
    }
    
    
    Camera &ICameraController::getCamera() {
        return this->camera;
    }
    
    
    void ICameraController::setWorldUpAxis(const glm::vec3 &worldUpAxis) {
        this->worldUpAxis = worldUpAxis;
    }
    
    
    const glm::vec3 &ICameraController::getWorldUpAxis() const {
        return this->worldUpAxis;
    }
    
    
    glm::dvec2 ICameraController::computeCursorDelta() {
        glm::dvec2 cursorPosition;
        glfwGetCursorPos(this->window, &cursorPosition.x, &cursorPosition.y);
        const glm::dvec2 delta = cursorPosition - this->lastCursorPosition;
        this->lastCursorPosition = cursorPosition;
        return delta;
    }
}
