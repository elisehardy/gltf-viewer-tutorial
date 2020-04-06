#ifndef GLTF_VIEWER_TUTORIAL_SHADOWMAP_HPP
#define GLTF_VIEWER_TUTORIAL_SHADOWMAP_HPP

#include <glad/glad.h>

#include <shader/Shader.hpp>
#include <glm/mat4x4.hpp>


namespace shader {
    struct ShadowMap {
        static constexpr int32_t SHADOW_MAP_RESOLUTION = 512;
        
        GLuint FBO;
        GLfloat bias;
        GLuint depthMap;
        GLboolean modified;
        glm::mat4 viewProjectionMatrix;
        std::unique_ptr<shader::Shader> shader;
        
        
        ShadowMap() = default;
        
        ShadowMap(const std::string &vsPath, const std::string &fsPath);
        
        ~ShadowMap();
    };
}

#endif //GLTF_VIEWER_TUTORIAL_SHADOWMAP_HPP
