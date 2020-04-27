#ifndef GLTF_VIEWER_TUTORIAL_SHADOWMAP_HPP
#define GLTF_VIEWER_TUTORIAL_SHADOWMAP_HPP

#include <glad/glad.h>

#include <shader/Shader.hpp>
#include <glm/mat4x4.hpp>


namespace shader {
    
    struct ShadowMap {
        static constexpr int32_t SHADOW_MAP_RESOLUTION = 4096;
        
        GLuint FBO;
        GLuint depthMap;
        GLuint sampler;
        GLfloat bias;
        GLfloat spread;
        GLint sampleCount;
        bool enabled;
        bool display;
        GLboolean modified;
        glm::mat4 viewProjectionMatrix;
        std::unique_ptr<shader::Shader> computeShader;
        std::unique_ptr<shader::Shader> displayShader;
        
        ShadowMap() = default;
        
        ShadowMap(const std::string &computeVsPath, const std::string &computeFsPath, const std::string &displayVsPath,
                  const std::string &displayFsPath);
        
        ~ShadowMap();
    
        void render();
    };
}

#endif // GLTF_VIEWER_TUTORIAL_SHADOWMAP_HPP
