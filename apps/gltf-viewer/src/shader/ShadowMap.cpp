#include <shader/ShadowMap.hpp>
#include <utils/QuadScreen.hpp>


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
        
        this->computeShader = std::make_unique<shader::Shader>(computeVsPath, computeFsPath);
        this->computeShader->addUniform("uMVPMatrix", shader::UNIFORM_MATRIX_4F);
        
        this->displayShader = std::make_unique<shader::Shader>(displayVsPath, displayFsPath);
        this->computeShader->addUniform("uTex", shader::UNIFORM_SAMPLER2D);
    }
    
    
    ShadowMap::~ShadowMap() {
        glDeleteTextures(1, &this->depthMap);
        glDeleteFramebuffers(1, &this->FBO);
    }
    
    void ShadowMap::render() {
        this->displayShader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->depthMap);
        utils::QuadScreen::getInstance()->render();
        glBindTexture(GL_TEXTURE_2D, 0);
        this->displayShader->stop();
    }
}
