#pragma once

#include <utils/GLFWHandle.hpp>
#include <utils/filesystem.hpp>
#include <utils/gltf.hpp>
#include <utils/Light.hpp>
#include <utils/camera/ICameraController.hpp>
#include <shader/ShadowMap.hpp>
#include <shader/Shader.hpp>
#include <utils/camera/FirstPersonCameraController.hpp>


namespace viewer {
    
    enum Texture {
        GL_BASE_COLOR_TEXTURE = GL_TEXTURE0,
        GL_METALLIC_ROUGHNESS_TEXTURE = GL_TEXTURE1,
        GL_EMISSIVE_TEXTURE = GL_TEXTURE2,
        GL_OCCLUSION_TEXTURE = GL_TEXTURE3,
        GL_SHADOW_TEXTURE = GL_TEXTURE4,
    };
    
    enum TextureId {
        GL_BASE_COLOR_TEXTURE_ID = 0,
        GL_METALLIC_ROUGHNESS_TEXTURE_ID = 1,
        GL_EMISSIVE_TEXTURE_ID = 2,
        GL_OCCLUSION_TEXTURE_ID = 3,
        GL_SHADOW_TEXTURE_ID = 4,
    };
    
    
    
    struct VaoRange {
        uint64_t begin; /**< Index of first element in vertexArrayObjects. */
        uint64_t count; /**< Number of elements in range. */
        
        VaoRange() = default;
    };
    
    
    
    class ViewerApplication {
        private:
            static constexpr GLuint VERTEX_ATTRIB_POSITION = 0;
            static constexpr GLuint VERTEX_ATTRIB_NORMAL = 1;
            static constexpr GLuint VERTEX_ATTRIB_TEXTURE = 2;
            
            static constexpr char DEFAULT_VS[] = "forward.vs.glsl";
            static constexpr char DEFAULT_FS[] = "pbr_directional_light.fs.glsl";
            
            static constexpr char DEFAULT_SM_COMPUTE_VS[] = "directionalSM.vs.glsl";
            static constexpr char DEFAULT_SM_COMPUTE_FS[] = "directionalSM.fs.glsl";
            static constexpr char DEFAULT_SM_DISPLAY_VS[] = "quadscreen.vs.glsl";
            static constexpr char DEFAULT_SM_DISPLAY_FS[] = "quadscreen.fs.glsl";
            
            static constexpr int32_t DEFAULT_WINDOW_WIDTH = 1024;
            static constexpr int32_t DEFAULT_WINDOW_HEIGHT = 768;
        
        private:
            GLFWHandle glfwHandle; // Must be declared before any OpenGL resources (GLProgram, GLShader..)
            std::unique_ptr<utils::camera::ICameraController> cameraController;
            std::unique_ptr<shader::ShadowMap> shadowMap;
            std::unique_ptr<shader::Shader> shader;
            utils::Light light;
            
            tinygltf::Model model;
            std::vector<GLuint> textureObjects;
            std::vector<GLuint> bufferObjects;
            std::vector<GLuint> vertexArrayObjects;
            std::vector<VaoRange> meshToVertexArrays;
            GLuint whiteTexture = 0;
            glm::mat4 projMatrix;
            GLfloat sceneDiameter;
            glm::vec3 sceneCenter;
            
            bool displaySM;
        
        private:
            
            void initshader(const std::string &vsPath, const std::string &fsPath);
            
            void initCamera(const std::vector<GLfloat> &lookatArgs);
            
            void initWhiteTexture();
            
            void loadGltfFile(const std::string &glTFFilePath);
            
            void createTextureObjects();
            
            void createBufferObjects();
            
            void createVertexArrayObjects();
            
            void bindMaterial(GLint materialIndex);
            
            void drawScene();
            
            void drawNode(int nodeIdx, const glm::mat4 &parentMatrix);
            
            void drawImGUI();
            
            void drawNodeShadowMap(int nodeIdx, const glm::mat4 &parentMatrix);
            
            void computeShadowMap();
        
        public:
            
            ViewerApplication(const fs::path &appPath, uint32_t width, uint32_t height, fs::path glTFFile,
                              const std::vector<GLfloat> &lookatArgs, std::string vertexShader,
                              std::string fragmentShader);
            
            int run();
    };
}
