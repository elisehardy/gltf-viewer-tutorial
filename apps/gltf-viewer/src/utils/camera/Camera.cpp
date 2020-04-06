#include <utils/camera/Camera.hpp>


namespace utils::camera {
    
    Camera::Camera(glm::vec3 e, glm::vec3 c, glm::vec3 u) :
            eye(e), center(c), up(u) {
        const glm::vec3 front = this->center - this->eye;
        const glm::vec3 left = cross(this->up, front);
        assert(left != glm::vec3(0));
        this->up = normalize(cross(front, left));
    }
    
    
    glm::mat4 Camera::getViewMatrix() const {
        return glm::lookAt(this->eye, this->center, this->up);
    }
    
    
    void Camera::truckLeft(GLfloat offset) {
        const glm::vec3 front = this->center - this->eye;
        const glm::vec3 left = normalize(cross(this->up, front));
        const glm::vec3 translationVector = offset * left;
        
        this->eye += translationVector;
        this->center += translationVector;
    }
    
    
    void Camera::pedestalUp(GLfloat offset) {
        const glm::vec3 translationVector = offset * this->up;
        
        this->eye += translationVector;
        this->center += translationVector;
    }
    
    
    void Camera::dollyIn(GLfloat offset) {
        const glm::vec3 front = normalize(this->center - this->eye);
        const glm::vec3 translationVector = offset * front;
        
        this->eye += translationVector;
        this->center += translationVector;
    }
    
    
    void Camera::moveLocal(GLfloat truckLeftOffset, GLfloat pedestalUpOffset, GLfloat dollyIn) {
        const glm::vec3 front = normalize(this->center - this->eye);
        const glm::vec3 left = normalize(cross(this->up, front));
        const glm::vec3 translationVector = truckLeftOffset * left + pedestalUpOffset * this->up + dollyIn * front;
        
        this->eye += translationVector;
        this->center += translationVector;
    }
    
    
    void Camera::rollRight(GLfloat radians) {
        const glm::vec3 front = this->center - this->eye;
        const glm::mat4 rollMatrix = glm::rotate(glm::mat4(1), radians, front);
        
        this->up = glm::vec3(rollMatrix * glm::vec4(this->up, 0.f));
    }
    
    
    void Camera::tiltDown(GLfloat radians) {
        const glm::vec3 front = this->center - this->eye;
        const glm::vec3 left = cross(this->up, front);
        const glm::mat4 tiltMatrix = glm::rotate(glm::mat4(1), radians, left);
        const glm::vec3 newFront = glm::vec3(tiltMatrix * glm::vec4(front, 0.f));
        
        this->center = this->eye + newFront;
        this->up = glm::vec3(tiltMatrix * glm::vec4(up, 0.f));
    }
    
    
    void Camera::panLeft(GLfloat radians) {
        const glm::vec3 front = this->center - this->eye;
        const glm::mat4 panMatrix = glm::rotate(glm::mat4(1), radians, this->up);
        const glm::vec3 newFront = glm::vec3(panMatrix * glm::vec4(front, 0.f));
        
        this->center = this->eye + newFront;
    }
    
    
    void Camera::rotateLocal(GLfloat rollRight, GLfloat tiltDown, GLfloat panLeft) {
        const glm::vec3 front = this->center - this->eye;
        const glm::mat4 rollMatrix = glm::rotate(glm::mat4(1), rollRight, front);
        
        this->up = glm::vec3(rollMatrix * glm::vec4(this->up, 0.f));
        
        const glm::vec3 left = cross(this->up, front);
        const glm::mat4 tiltMatrix = glm::rotate(glm::mat4(1), tiltDown, left);
        const glm::vec3 newFront = glm::vec3(tiltMatrix * glm::vec4(front, 0.f));
        
        this->center = this->eye + newFront;
        this->up = glm::vec3(tiltMatrix * glm::vec4(this->up, 0.f));
        
        const glm::mat4 panMatrix = glm::rotate(glm::mat4(1), panLeft, this->up);
        const glm::vec3 newNewFront = glm::vec3(panMatrix * glm::vec4(newFront, 0.f));
        
        this->center = this->eye + newNewFront;
    }
    
    
    void Camera::rotateWorld(GLfloat radians, const glm::vec3 &axis) {
        const glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1), radians, axis);
        const glm::vec3 front = this->center - this->eye;
        const glm::vec3 newFront = glm::vec3(rotationMatrix * glm::vec4(front, 0));
        
        this->center = this->eye + newFront;
        this->up = glm::vec3(rotationMatrix * glm::vec4(this->up, 0));
    }
    
    
    glm::vec3 Camera::getEye() const {
        return this->eye;
    }
    
    
    glm::vec3 Camera::getCenter() const {
        return this->center;
    }
    
    
    glm::vec3 Camera::getUp() const {
        return this->up;
    }
    
    
    glm::vec3 Camera::getFront(bool normalize) const {
        const glm::vec3 f = this->center - this->eye;
        
        return normalize ? glm::normalize(f) : f;
    }
    
    
    glm::vec3 Camera::getLeft(bool normalize) const {
        const glm::vec3 f = getFront(false);
        const glm::vec3 l = glm::cross(this->up, f);
        
        return normalize ? glm::normalize(l) : l;
    }
}
