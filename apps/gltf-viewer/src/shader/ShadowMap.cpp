#include <shader/ShadowMap.hpp>
#include <utils/QuadScreen.hpp>
#include <iostream>


namespace shader {
    
    ShadowMap::ShadowMap(const std::string &computeVsPath, const std::string &computeFsPath,
                         const std::string &displayVsPath, const std::string &displayFsPath) :
            resolution(1024), bias(0.00f), spread(0.001f), sampleCount(16), enabled(true), display(false),
            modified(true), modifiedResolution(true) {
        glGenFramebuffers(1, &this->FBO);
        
        
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        
        
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
    
    void ShadowMap::regenTexture() {
        glDeleteTextures(1, &this->depthMap);
        glGenTextures(1, &this->depthMap);
    
        glBindTexture(GL_TEXTURE_2D, this->depthMap);
        glTexImage2D(
                GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, this->resolution,
                this->resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
        glBindTexture(GL_TEXTURE_2D, this->depthMap);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthMap, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
        glBindTexture(GL_TEXTURE_2D, 0);
        this->modified = true;
    }
    
    void ShadowMap::render() {
        this->displayShader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->depthMap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        utils::QuadScreen::getInstance()->render();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glBindTexture(GL_TEXTURE_2D, 0);
        this->displayShader->stop();
    }
}
