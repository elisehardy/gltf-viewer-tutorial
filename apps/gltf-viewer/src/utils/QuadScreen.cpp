#include <utils/QuadScreen.hpp>
#include <iostream>


namespace utils {
    
    QuadVertex::QuadVertex(glm::vec3 vertex, glm::vec2 texture) :
            vertex(vertex), texture(texture) {
    }
    
    
    QuadVertex QuadScreen::vertices[6] = {
            {{ -1.f, -1.f, 0.f }, { 0.f, 0.f }},
            {{ -1.f, 1.f,  0.f }, { 0.f, 1.f }},
            {{ 1.f,  1.f,  0.f }, { 1.f, 1.f }},
            {{ 1.f,  1.f,  0.f }, { 1.f, 1.f }},
            {{ 1.f,  -1.f, 0.f }, { 1.f, 0.f }},
            {{ -1.f, -1.f, 0.f }, { 0.f, 0.f }}
    };
    
    
    QuadScreen::QuadScreen() {
        glGenBuffers(1, &this->vbo);
        glGenVertexArrays(1, &this->vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(QuadScreen::vertices), QuadScreen::vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
        glEnableVertexAttribArray(VERTEX_ATTR_TEXTURE);
        glVertexAttribPointer(
                VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(QuadVertex),
                reinterpret_cast<const GLvoid *>(offsetof(QuadVertex, vertex))
        );
        glVertexAttribPointer(
                VERTEX_ATTR_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex),
                reinterpret_cast<const GLvoid *>(offsetof(QuadVertex, texture))
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    
    QuadScreen::~QuadScreen() {
        glDeleteBuffers(1, &this->vbo);
        glDeleteVertexArrays(1, &this->vao);
    }
    
    
    QuadScreen *QuadScreen::getInstance() {
        static QuadScreen quad;
        return &quad;
    }
    
    
    void QuadScreen::render() const {
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(6));
        glBindVertexArray(0);
    }
}
