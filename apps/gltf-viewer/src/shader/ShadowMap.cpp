#include <shader/ShadowMap.hpp>


namespace shader {
    
    ShadowMap::ShadowMap(const std::string &vsPath, const std::string &fsPath) :
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
        
        this->shader = std::make_unique<shader::Shader>(vsPath, fsPath);
        this->shader->addUniform("uMVPMatrix", shader::UNIFORM_MATRIX_4F);
    }
    
    
    ShadowMap::~ShadowMap() {
        glDeleteTextures(1, &this->depthMap);
        glDeleteFramebuffers(1, &this->FBO);
    }
}
