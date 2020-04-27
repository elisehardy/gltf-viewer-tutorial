#ifndef GLTF_VIEWER_TUTORIAL_SHADOWMAP_HPP
#define GLTF_VIEWER_TUTORIAL_SHADOWMAP_HPP

#include <glad/glad.h>

#include <shader/Shader.hpp>
#include <glm/mat4x4.hpp>


namespace shader {
    
    struct ShadowMap {
        GLint resolution;
        GLuint FBO;
        GLuint depthMap;
        GLuint sampler;
        GLfloat bias;
        GLfloat spread;
        GLint sampleCount;
        bool enabled;
        bool display;
        GLboolean modified;
        GLboolean modifiedResolution;
        glm::mat4 viewProjectionMatrix;
        std::unique_ptr<shader::Shader> computeShader;
        std::unique_ptr<shader::Shader> displayShader;
        
        ShadowMap() = default;
        
        ShadowMap(const std::string &computeVsPath, const std::string &computeFsPath, const std::string &displayVsPath,
                  const std::string &displayFsPath);
        
        ~ShadowMap();
    
        void regenTexture();
        
        void render();
    };
}

#endif // GLTF_VIEWER_TUTORIAL_SHADOWMAP_HPP
