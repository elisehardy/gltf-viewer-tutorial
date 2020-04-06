#include <utils/camera/ViewFrame.hpp>


namespace utils::camera {
    
    ViewFrame::ViewFrame(glm::vec3 l, glm::vec3 u, glm::vec3 f, glm::vec3 e) :
            left(l), up(u), front(f), eye(e) {
    }
    
    
    ViewFrame ViewFrame::fromViewToWorldMatrix(const glm::mat4 &viewToWorldMatrix) {
        return ViewFrame {
                -glm::vec3(viewToWorldMatrix[0]), glm::vec3(viewToWorldMatrix[1]),
                -glm::vec3(viewToWorldMatrix[2]), glm::vec3(viewToWorldMatrix[3])
        };
    }
}
