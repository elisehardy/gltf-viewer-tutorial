#ifndef GLTF_VIEWER_TUTORIAL_QUADSCREEN_HPP
#define GLTF_VIEWER_TUTORIAL_QUADSCREEN_HPP

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <utils/glfw.hpp>


namespace utils {
    
    struct QuadVertex {
        glm::vec3 vertex;
        glm::vec2 texture;
        
        QuadVertex() = default;
    
        QuadVertex(glm::vec3 vertex, glm::vec2 texture);
    };
    
    
    
    class QuadScreen {
        private:
            static constexpr GLuint VERTEX_ATTR_POSITION = 0;
            static constexpr GLuint VERTEX_ATTR_TEXTURE = 1;
            static QuadVertex vertices[6];
            
            GLuint vbo = 0;
            GLuint vao = 0;
        
        private:
            
            QuadScreen();
        
            ~QuadScreen();
            
        public:
            
            static QuadScreen *getInstance();
        
            void render() const;
    };
}

#endif // GLTF_VIEWER_TUTORIAL_QUADSCREEN_HPP
