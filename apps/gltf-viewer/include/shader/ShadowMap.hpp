#ifndef GLTF_VIEWER_TUTORIAL_SHADOWMAP_HPPaPos
#define GLTF_VIEWER_TUTORIAL_SHADOWMAP_HPP

#include <glad/glad.h>

#include <shader/Shader.hpp>
#include <glm/mat4x4.hpp>


namespace shader {
    
    struct ShadowMap {
        static constexpr int32_t SHADOW_MAP_RESOLUTION = 4096;
        
        GLuint FBO;
        GLfloat bias;
        GLfloat spread;
        GLint sampleCount;
        GLuint depthMap;
        bool enabled;
        bool display;
        GLboolean modified;
        glm::mat4 viewProjectionMatrix;
        std::unique_ptr<shader::Shader> shader;
    
        ShadowMap() = default;
        
        ShadowMap(const std::string &vsPath, const std::string &fsPath);
        
        ~ShadowMap();
    };
}

#endif // GLTF_VIEWER_TUTORIAL_SHADOWMAP_HPP
