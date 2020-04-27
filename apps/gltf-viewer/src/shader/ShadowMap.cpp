#include <shader/ShadowMap.hpp>
#include <utils/QuadScreen.hpp>
#include <iostream>


namespace shader {
    
    ShadowMap::ShadowMap(const std::string &computeVsPath, const std::string &computeFsPath,
                         const std::string &displayVsPath, const std::string &displayFsPath) :
            bias(0.00f), spread(0.001f), sampleCount(16), enabled(true), display(false), modified(true) {
        glGenFramebuffers(1, &this->FBO);
        glGenTextures(1, &this->depthMap);
        
        glBindTexture(GL_TEXTURE_2D, this->depthMap);
        glTexImage2D(
                GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_MAP_RESOLUTION,
                SHADOW_MAP_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        
        glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        this->computeShader = std::make_unique<shader::Shader>(computeVsPath, computeFsPath);
        this->computeShader->addUniform("uMVPMatrix", shader::UNIFORM_MATRIX_4F);
        
        this->displayShader = std::make_unique<shader::Shader>(displayVsPath, displayFsPath);
    }
    
    
    ShadowMap::~ShadowMap() {
        glDeleteTextures(1, &this->depthMap);
        glDeleteFramebuffers(1, &this->FBO);
    }
    
    
    static float linearize_depth(float d) {
        float zNear = 0.01f * 18.5479f, zFar = 2.f * 18.5479f;
        float z_n = 2.0f * d - 1.0f;
        return 2.0f * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
    }
    
    
    void ShadowMap::render() {
        this->displayShader->use();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->depthMap);
//        GLfloat *data;
//        data = new GLfloat[shader::ShadowMap::SHADOW_MAP_RESOLUTION * shader::ShadowMap::SHADOW_MAP_RESOLUTION];
//        glGetTexImage(
//                GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data
//        );
//        for (int x = 0; x < shader::ShadowMap::SHADOW_MAP_RESOLUTION; x++) {
//            for (int y = 0; y < shader::ShadowMap::SHADOW_MAP_RESOLUTION; y++) {
//                std::cout << data[x * shader::ShadowMap::SHADOW_MAP_RESOLUTION + y] << " ";
//            }
//            std::cout << std::endl;
//        }
        
        utils::QuadScreen::getInstance()->render();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glBindTexture(GL_TEXTURE_2D, 0);
        this->displayShader->stop();
    }
}
