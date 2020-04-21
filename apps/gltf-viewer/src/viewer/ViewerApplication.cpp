#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tiny_gltf.h>

#include <utils/camera/FirstPersonCameraController.hpp>
#include <utils/camera/TrackballCameraController.hpp>
#include <viewer/ViewerApplication.hpp>


static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        glfwSetWindowShouldClose(window, 1);
    }
}


namespace viewer {
    
    ViewerApplication::ViewerApplication(const fs::path &appPath, uint32_t width, uint32_t height, fs::path glTFFile,
                                         const std::vector<GLfloat> &lookatArgs, std::string vertexShader,
                                         std::string fragmentShader) :
            glfwHandle(width, height, "glTF Viewer") {
        
        std::string appName = appPath.stem().string();
        fs::path shadersRoot = appPath.parent_path() / "shaders" / appName;
        vertexShader = shadersRoot / (vertexShader.empty() ? DEFAULT_VS : vertexShader);
        fragmentShader = shadersRoot / (fragmentShader.empty() ? DEFAULT_FS : fragmentShader);
        this->initshader(vertexShader, fragmentShader);
        
        std::string smVertexShader = shadersRoot / DEFAULT_SM_VS;
        std::string smFragmentShader = shadersRoot / DEFAULT_SM_FS;
        this->shadowMap = std::make_unique<shader::ShadowMap>(smVertexShader, smFragmentShader);
        
        this->initWhiteTexture();
        
        this->loadGltfFile(glTFFile);
        this->createTextureObjects();
        this->createBufferObjects();
        this->createVertexArrayObjects();
        
        this->initCamera(lookatArgs);
        
        ImGui::GetIO().IniFilename = (appName + ".imgui.ini").c_str(); // At exit, ImGUI will store its windows
        glfwSetKeyCallback(this->glfwHandle.window(), keyCallback);
        printGLVersion();
    }
    
    
    void ViewerApplication::initshader(const std::string &vsPath, const std::string &fsPath) {
        this->shader = std::make_unique<shader::Shader>(vsPath, fsPath);
        this->shader->addUniform("uMVPMatrix", shader::UNIFORM_MATRIX_4F);
        this->shader->addUniform("uMVMatrix", shader::UNIFORM_MATRIX_4F);
        this->shader->addUniform("uNormalMatrix", shader::UNIFORM_MATRIX_4F);
        
        this->shader->addUniform("uDirLightViewProjMatrix", shader::UNIFORM_MATRIX_4F);
        this->shader->addUniform("uLightDirection", shader::UNIFORM_3_F);
        this->shader->addUniform("uDirLightIntensity", shader::UNIFORM_1_F);
        this->shader->addUniform("uAmbLightIntensity", shader::UNIFORM_1_F);
        this->shader->addUniform("uLightColor", shader::UNIFORM_3_F);
        
        this->shader->addUniform("uShadowMap", shader::UNIFORM_SAMPLER2D);
        this->shader->addUniform("uShadowMapBias", shader::UNIFORM_1_F);
        this->shader->addUniform("uShadowMapEnabled", shader::UNIFORM_1_I);
        
        this->shader->addUniform("uBaseColorFactor", shader::UNIFORM_4_F);
        this->shader->addUniform("uMetallicFactor", shader::UNIFORM_1_F);
        this->shader->addUniform("uRoughnessFactor", shader::UNIFORM_1_F);
        this->shader->addUniform("uBaseColorTexture", shader::UNIFORM_SAMPLER2D);
        this->shader->addUniform("uMetallicRoughnessTexture", shader::UNIFORM_SAMPLER2D);
        this->shader->addUniform("uRoughnessFactor", shader::UNIFORM_1_F);
        this->shader->addUniform("uEmissiveFactor", shader::UNIFORM_1_F);
        this->shader->addUniform("uEmissiveTexture", shader::UNIFORM_SAMPLER2D);
        
        this->shader->addUniform("uOcclusionTexture", shader::UNIFORM_SAMPLER2D);
        this->shader->addUniform("uOcclusionStrength", shader::UNIFORM_1_F);
    }
    
    
    void ViewerApplication::initCamera(const std::vector<GLfloat> &lookatArgs) {
        glm::vec3 bboxMin, bboxMax, direction, eye;
        computeSceneBounds(model, bboxMin, bboxMax);
        direction = bboxMax - bboxMin;
        this->sceneCenter = (bboxMax + bboxMin) * 0.5f;
        this->sceneDiameter = glm::length(direction);
        
        this->cameraController = std::make_unique<utils::camera::FirstPersonCameraController>(
                this->glfwHandle.window(), 0.3f * this->sceneDiameter
        );
        
        if (!lookatArgs.empty()) {
            
            this->cameraController->setCamera(
                    {
                            glm::vec3(lookatArgs[0], lookatArgs[1], lookatArgs[2]),
                            glm::vec3(lookatArgs[3], lookatArgs[4], lookatArgs[5]),
                            glm::vec3(lookatArgs[6], lookatArgs[7], lookatArgs[8])
                    }
            );
        }
        else {
            if (direction.z > 0) {
                eye = this->sceneCenter + direction;
            }
            else {
                eye = this->sceneCenter + 2.f * glm::cross(direction, glm::vec3(0, 1, 0));
            }
            this->cameraController->setCamera({ eye, this->sceneCenter, glm::vec3(0, 1, 0) });
        }
        
        glm::ivec2 windowSize = this->glfwHandle.getSize();
        this->projMatrix = glm::perspective(
                70.f, static_cast<GLfloat>(windowSize.x) / windowSize.y, 0.001f * this->sceneDiameter,
                1.5f * this->sceneDiameter
        );
    }
    
    
    void ViewerApplication::initWhiteTexture() {
        float white[] = { 1, 1, 1, 1 };
        
        glGenTextures(1, &whiteTexture);
        glBindTexture(GL_TEXTURE_2D, whiteTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    
    void ViewerApplication::loadGltfFile(const std::string &glTFFilePath) {
        tinygltf::TinyGLTF loader;
        std::string warn, err;
        
        bool ret = loader.LoadASCIIFromFile(&this->model, &err, &warn, glTFFilePath);
        
        if (!warn.empty()) {
            std::cerr << warn << std::endl;
        }
        if (!err.empty()) {
            std::cerr << err << std::endl;
        }
        
        if (!ret) {
            throw std::runtime_error("Could not parse glTF file '" + glTFFilePath + "'.");
        }
    }
    
    
    void ViewerApplication::createTextureObjects() {
        tinygltf::Sampler defaultSampler;
        
        defaultSampler.minFilter = GL_LINEAR;
        defaultSampler.magFilter = GL_LINEAR;
        defaultSampler.wrapS = GL_REPEAT;
        defaultSampler.wrapT = GL_REPEAT;
        defaultSampler.wrapR = GL_REPEAT;
        
        this->textureObjects.resize(this->model.textures.size());
        this->textureObjects.clear();
        
        glActiveTexture(GL_BASE_COLOR_TEXTURE);
        glGenTextures(GLsizei(this->model.textures.size()), this->textureObjects.data());
        
        for (size_t i = 0; i < this->model.textures.size(); ++i) {
            const auto &texture = this->model.textures[i];
            const auto &image = this->model.images[texture.source];
            const auto &sampler = texture.sampler >= 0 ? this->model.samplers[texture.sampler] : defaultSampler;
            glBindTexture(GL_TEXTURE_2D, this->textureObjects[i]);
            glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, image.pixel_type,
                    image.image.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            sampler.minFilter != -1 ? sampler.minFilter : GL_LINEAR
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                            sampler.magFilter != -1 ? sampler.magFilter : GL_LINEAR
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, sampler.wrapR);
            
            if (sampler.minFilter == GL_NEAREST_MIPMAP_NEAREST || sampler.minFilter == GL_NEAREST_MIPMAP_LINEAR ||
                sampler.minFilter == GL_LINEAR_MIPMAP_NEAREST || sampler.minFilter == GL_LINEAR_MIPMAP_LINEAR) {
                glGenerateMipmap(GL_TEXTURE_2D);
            }
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    
    void ViewerApplication::createBufferObjects() {
        this->bufferObjects.resize(this->model.buffers.size());
        this->bufferObjects.clear();
        
        glGenBuffers(this->model.buffers.size(), this->bufferObjects.data());
        for (uint32_t i = 0; i < this->model.buffers.size(); ++i) {
            glBindBuffer(GL_ARRAY_BUFFER, this->bufferObjects[i]);
            glBufferStorage(GL_ARRAY_BUFFER, this->model.buffers[i].data.size(), this->model.buffers[i].data.data(), 0);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    
    void ViewerApplication::createVertexArrayObjects() {
        tinygltf::BufferView bufferView;
        tinygltf::Primitive primitive;
        tinygltf::Accessor accessor;
        tinygltf::Mesh mesh;
        VaoRange vaoRange {};
        int accessorIndex, bufferIndex;
        uint64_t byteOffset;
        GLuint vao;
        
        this->vertexArrayObjects.resize(0);
        this->meshToVertexArrays.resize(this->model.meshes.size());
        
        for (uint32_t i = 0; i < this->model.meshes.size(); ++i) {
            mesh = this->model.meshes[i];
            
            vaoRange = this->meshToVertexArrays[i];
            vaoRange.begin = this->vertexArrayObjects.size();
            vaoRange.count = mesh.primitives.size();
            
            this->vertexArrayObjects.resize(this->vertexArrayObjects.size() + mesh.primitives.size());
            
            glGenVertexArrays(vaoRange.count, &this->vertexArrayObjects[vaoRange.begin]);
            for (uint32_t j = 0; j < mesh.primitives.size(); ++j) {
                vao = this->vertexArrayObjects[vaoRange.begin + j];
                primitive = mesh.primitives[j];
                glBindVertexArray(vao);
                
                auto iterator = primitive.attributes.find("POSITION");
                if (iterator != end(primitive.attributes)) {
                    accessorIndex = (*iterator).second;
                    accessor = this->model.accessors[accessorIndex];
                    bufferView = this->model.bufferViews[accessor.bufferView];
                    bufferIndex = bufferView.buffer;
                    byteOffset = accessor.byteOffset + bufferView.byteOffset;
                    
                    glEnableVertexAttribArray(VERTEX_ATTRIB_POSITION);
                    glBindBuffer(GL_ARRAY_BUFFER, this->bufferObjects[bufferIndex]);
                    glVertexAttribPointer(
                            VERTEX_ATTRIB_POSITION, accessor.type, accessor.componentType, GL_FALSE,
                            bufferView.byteStride, reinterpret_cast<const GLvoid *>(byteOffset)
                    );
                }
                
                iterator = primitive.attributes.find("NORMAL");
                if (iterator != end(primitive.attributes)) {
                    accessorIndex = (*iterator).second;
                    accessor = this->model.accessors[accessorIndex];
                    bufferView = this->model.bufferViews[accessor.bufferView];
                    bufferIndex = bufferView.buffer;
                    
                    glEnableVertexAttribArray(VERTEX_ATTRIB_NORMAL);
                    glBindBuffer(GL_ARRAY_BUFFER, this->bufferObjects[bufferIndex]);
                    glVertexAttribPointer(
                            VERTEX_ATTRIB_NORMAL, accessor.type, accessor.componentType, GL_FALSE,
                            bufferView.byteStride,
                            reinterpret_cast<const GLvoid *>(accessor.byteOffset + bufferView.byteOffset)
                    );
                }
                
                iterator = primitive.attributes.find("TEXCOORD_0");
                if (iterator != end(primitive.attributes)) {
                    accessorIndex = (*iterator).second;
                    accessor = this->model.accessors[accessorIndex];
                    bufferView = this->model.bufferViews[accessor.bufferView];
                    bufferIndex = bufferView.buffer;
                    
                    glEnableVertexAttribArray(VERTEX_ATTRIB_TEXTURE);
                    glBindBuffer(GL_ARRAY_BUFFER, this->bufferObjects[bufferIndex]);
                    glVertexAttribPointer(
                            VERTEX_ATTRIB_TEXTURE, accessor.type, accessor.componentType, GL_FALSE,
                            bufferView.byteStride,
                            reinterpret_cast<const GLvoid *>(accessor.byteOffset + bufferView.byteOffset)
                    );
                }
                
                // Index array if defined
                if (primitive.indices >= 0) {
                    accessorIndex = primitive.indices;
                    accessor = model.accessors[accessorIndex];
                    bufferView = model.bufferViews[accessor.bufferView];
                    bufferIndex = bufferView.buffer;
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObjects[bufferIndex]);
                }
            }
        }
        
        glBindVertexArray(0);
    }
    
    
    void ViewerApplication::bindMaterial(GLint materialIndex) {
        GLint uBaseColorTexture = this->shader->getUniformLocation("uBaseColorTexture");
        GLint uBaseColorFactor = this->shader->getUniformLocation("uBaseColorFactor");
        GLint uMetallicRoughnessTexture = this->shader->getUniformLocation("uMetallicRoughnessTexture");
        GLint uMetallicFactor = this->shader->getUniformLocation("uMetallicFactor");
        GLint uRoughnessFactor = this->shader->getUniformLocation("uRoughnessFactor");
        GLint uEmissiveTexture = this->shader->getUniformLocation("uEmissiveTexture");
        GLint uEmissiveFactor = this->shader->getUniformLocation("uEmissiveFactor");
        GLint uOcclusionTexture = this->shader->getUniformLocation("uOcclusionTexture");
        GLint uOcclusionStrength = this->shader->getUniformLocation("uOcclusionStrength");
        
        if (materialIndex >= 0) {
            const tinygltf::Material &material = this->model.materials[materialIndex];
            const tinygltf::PbrMetallicRoughness &pbrMetallicRoughness = material.pbrMetallicRoughness;
            const tinygltf::TextureInfo &emissiveTexture = material.emissiveTexture;
            const std::vector<double> &emissiveFactor = material.emissiveFactor;
            const tinygltf::OcclusionTextureInfo occlusionTexture = material.occlusionTexture;
            
            if (uBaseColorTexture >= 0) {
                auto textureObject = this->whiteTexture;
                if (pbrMetallicRoughness.baseColorTexture.index >= 0) {
                    const tinygltf::Texture
                            &texture = this->model.textures[pbrMetallicRoughness.baseColorTexture.index];
                    if (texture.source >= 0) {
                        textureObject = this->textureObjects[texture.source];
                    }
                }
                glActiveTexture(GL_BASE_COLOR_TEXTURE);
                glBindTexture(GL_TEXTURE_2D, textureObject);
                glUniform1i(uBaseColorTexture, GL_BASE_COLOR_TEXTURE_ID);
            }
            
            if (uBaseColorFactor >= 0) {
                glUniform4f(uBaseColorFactor, static_cast<float>(pbrMetallicRoughness.baseColorFactor[0]),
                            static_cast<float>(pbrMetallicRoughness.baseColorFactor[1]),
                            static_cast<float>(pbrMetallicRoughness.baseColorFactor[2]),
                            static_cast<float>(pbrMetallicRoughness.baseColorFactor[3]));
            }
            
            if (uMetallicRoughnessTexture >= 0) {
                GLuint textureObject = 0;
                if (pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
                    const tinygltf::Texture &texture =
                            this->model.textures[pbrMetallicRoughness.metallicRoughnessTexture.index];
                    if (texture.source >= 0) {
                        textureObject = this->textureObjects[texture.source];
                    }
                }
                glActiveTexture(GL_METALLIC_ROUGHNESS_TEXTURE);
                glBindTexture(GL_TEXTURE_2D, textureObject);
                glUniform1i(uMetallicRoughnessTexture, GL_METALLIC_ROUGHNESS_TEXTURE_ID);
            }
            
            if (uEmissiveTexture > 0) {
                auto textureObject = 0;
                if (emissiveTexture.index >= 0) {
                    const tinygltf::Texture &texture = model.textures[emissiveTexture.index];
                    if (texture.source >= 0) {
                        textureObject = textureObjects[texture.source];
                    }
                }
                glActiveTexture(GL_EMISSIVE_TEXTURE);
                glBindTexture(GL_TEXTURE_2D, textureObject);
                glUniform1i(uEmissiveTexture, GL_EMISSIVE_TEXTURE_ID);
            }
            
            if (uEmissiveFactor >= 0) {
                glUniform3f(uEmissiveFactor, (float) emissiveFactor[0], (float) emissiveFactor[1],
                            (float) emissiveFactor[2]
                );
            }
            if (uOcclusionTexture >= 0) {
                auto textureObject = 0;
                if (occlusionTexture.index >= 0) {
                    const auto &texture = model.textures[occlusionTexture.index];
                    if (texture.source >= 0) {
                        textureObject = textureObjects[texture.source];
                    }
                }
                glActiveTexture(GL_OCCLUSION_TEXTURE);
                glBindTexture(GL_TEXTURE_2D, textureObject);
                glUniform1i(uOcclusionTexture, GL_OCCLUSION_TEXTURE_ID);
                glUniform1f(uOcclusionStrength, occlusionTexture.strength);
            }
            
            if (uMetallicFactor >= 0) {
                glUniform1f(uMetallicFactor, static_cast<float>(pbrMetallicRoughness.metallicFactor));
            }
            
            if (uRoughnessFactor >= 0) {
                glUniform1f(uRoughnessFactor, static_cast<float>(pbrMetallicRoughness.roughnessFactor));
            }
        }
        else {
            if (uBaseColorTexture >= 0) {
                glActiveTexture(GL_BASE_COLOR_TEXTURE);
                glBindTexture(GL_TEXTURE_2D, this->whiteTexture);
                glUniform1i(uBaseColorTexture, GL_BASE_COLOR_TEXTURE_ID);
            }
            
            if (uBaseColorFactor >= 0) {
                glUniform4f(uBaseColorFactor, 1, 1, 1, 1);
            }
            
            if (uMetallicRoughnessTexture >= 0) {
                glActiveTexture(GL_METALLIC_ROUGHNESS_TEXTURE);
                glBindTexture(GL_TEXTURE_2D, 0);
                glUniform1i(uMetallicRoughnessTexture, GL_METALLIC_ROUGHNESS_TEXTURE_ID);
            }
            
            if (uEmissiveFactor >= 0) {
                glUniform3f(uEmissiveFactor, 1, 1, 1);
            }
            
            if (uEmissiveTexture > 0) {
                glActiveTexture(GL_EMISSIVE_TEXTURE);
                glBindTexture(GL_TEXTURE_2D, 0);
                glUniform1i(uEmissiveTexture, GL_EMISSIVE_TEXTURE_ID);
            }
            
            if (uMetallicFactor >= 0) {
                glUniform1f(uMetallicFactor, 1.f);
            }
            
            if (uRoughnessFactor >= 0) {
                glUniform1f(uRoughnessFactor, 1.f);
            }
            
            if (uEmissiveFactor >= 0) {
                glUniform3f(uEmissiveFactor, 1, 1, 1);
            }
            
            if (uOcclusionTexture >= 0) {
                glActiveTexture(GL_OCCLUSION_TEXTURE);
                glBindTexture(GL_TEXTURE_2D, 0);
                glUniform1i(uOcclusionTexture, GL_OCCLUSION_TEXTURE_ID);
                glUniform1f(uOcclusionStrength, 0);
            }
        }
    }
    
    
    void ViewerApplication::drawNode(int nodeIdx, const glm::mat4 &parentMatrix) {
        tinygltf::Node node = model.nodes[nodeIdx];
        glm::mat4 modelMatrix = getLocalToWorldMatrix(node, parentMatrix);
        
        if (node.mesh >= 0) {
            glm::mat4 viewMatrix = this->cameraController->getCamera().getViewMatrix();
            glm::mat4 mvMatrix = viewMatrix * modelMatrix;
            glm::mat4 mvpMatrix = this->projMatrix * mvMatrix;
            glm::mat4 normalMatrix = glm::transpose(glm::inverse(mvMatrix));
            
            this->shader->loadUniform("uMVPMatrix", glm::value_ptr(mvpMatrix));
            this->shader->loadUniform("uMVMatrix", glm::value_ptr(mvMatrix));
            this->shader->loadUniform("uNormalMatrix", glm::value_ptr(normalMatrix));
            
            VaoRange vaoRange = meshToVertexArrays[node.mesh];
            tinygltf::Mesh mesh = model.meshes[node.mesh];
            for (uint32_t i = 0; i < mesh.primitives.size(); ++i) {
                GLuint vao = vertexArrayObjects[vaoRange.begin + i];
                tinygltf::Primitive primitive = mesh.primitives[i];
                bindMaterial(primitive.material);
                
                glBindVertexArray(vao);
                if (primitive.indices >= 0) {
                    tinygltf::Accessor accessor = model.accessors[primitive.indices];
                    tinygltf::BufferView bufferView = model.bufferViews[accessor.bufferView];
                    uint64_t byteOffset = accessor.byteOffset + bufferView.byteOffset;
                    glDrawElements(primitive.mode, accessor.count, accessor.componentType,
                                   reinterpret_cast<const GLvoid *>(byteOffset));
                }
                else {
                    uint32_t index = (*begin(primitive.attributes)).second;
                    tinygltf::Accessor accessor = model.accessors[index];
                    glDrawArrays(primitive.mode, 0, accessor.count);
                }
            }
        }
        
        // Draw children
        std::for_each(node.children.begin(), node.children.end(),
                      [this, &modelMatrix](uint32_t i) { this->drawNode(i, modelMatrix); }
        );
    }
    
    
    void ViewerApplication::drawScene() {
        glm::ivec2 windowSize = this->glfwHandle.getSize();
        glViewport(0, 0, windowSize.x, windowSize.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 viewMatrix = this->cameraController->getCamera().getViewMatrix();
        
        this->shader->use();
        if (this->light.fromCamera) {
            this->shader->loadUniform("uLightDirection", glm::value_ptr(glm::vec3(0, 0, 1)));
        }
        else {
            this->shader->loadUniform("uLightDirection",
                                      glm::value_ptr(glm::normalize(
                                              glm::vec3(viewMatrix * glm::vec4(this->light.direction, 0.)))));
        }
        this->shader->loadUniform("uDirLightIntensity", &this->light.dirIntensity);
        this->shader->loadUniform("uAmbLightIntensity", &this->light.ambIntensity);
        this->shader->loadUniform("uLightColor", glm::value_ptr(this->light.color));
        
        const auto invViewMatrix = glm::inverse(this->cameraController->getCamera().getViewMatrix());
        glActiveTexture(GL_SHADOW_TEXTURE);
        glBindTexture(GL_TEXTURE_2D, this->shadowMap->depthMap);
        GLint shadowTextureId = GL_SHADOW_TEXTURE_ID;
        GLint shadowEnabled = this->shadowMap->enabled;
        this->shader->loadUniform(
                "uDirLightViewProjMatrix", glm::value_ptr(this->shadowMap->viewProjectionMatrix * invViewMatrix)
        );
        this->shader->loadUniform("uShadowMapBias", &this->shadowMap->bias);
        this->shader->loadUniform("uShadowMap", &shadowTextureId);
        this->shader->loadUniform("uShadowMapEnabled", &shadowEnabled);
        
        if (this->model.defaultScene >= 0) {
            std::for_each(
                    this->model.scenes[this->model.defaultScene].nodes.begin(),
                    this->model.scenes[this->model.defaultScene].nodes.end(),
                    [this](uint32_t i) { this->drawNode(i, glm::mat4(1)); }
            );
        }
        
        this->shader->stop();
    }
    
    
    void ViewerApplication::drawImGUI() {
        const utils::camera::Camera camera = cameraController->getCamera();
        
        imguiNewFrame();
        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate
            );
            if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
                const glm::vec3 eye = camera.getEye();
                const glm::vec3 center = camera.getCenter();
                const glm::vec3 up = camera.getUp();
                const glm::vec3 front = camera.getFront();
                const glm::vec3 left = camera.getLeft();
                
                ImGui::Text("eye: %.3f %.3f %.3f", eye.x, eye.y, eye.z);
                ImGui::Text("center: %.3f %.3f %.3f", center.x, center.y, center.z);
                ImGui::Text("up: %.3f %.3f %.3f", up.x, up.y, up.z);
                ImGui::Text("front: %.3f %.3f %.3f", front.x, front.y, front.z);
                ImGui::Text("left: %.3f %.3f %.3f", left.x, left.y, left.z);
                
                if (ImGui::Button("CLI camera args to clipboard")) {
                    std::stringstream ss;
                    ss << "--lookat " << eye.x << "," << eye.y << "," << eye.z << "," << center.x << "," << center.y
                       << ","
                       << center.z << "," << up.x << "," << up.y << "," << up.z;
                    
                    const std::string str = ss.str();
                    glfwSetClipboardString(this->glfwHandle.window(), str.c_str());
                }
                
                static int cameraControllerType = 0;
                if (ImGui::RadioButton("First Person", &cameraControllerType, 0) ||
                    ImGui::RadioButton("Trackball", &cameraControllerType, 1)) {
                    const utils::camera::Camera currentCamera = cameraController->getCamera();
                    if (cameraControllerType == 0) {
                        cameraController = std::make_unique<utils::camera::FirstPersonCameraController>(
                                this->glfwHandle.window(), 0.5f * this->sceneDiameter
                        );
                    }
                    else {
                        cameraController =
                                std::make_unique<utils::camera::TrackballCameraController>(this->glfwHandle.window());
                    }
                    cameraController->setCamera(currentCamera);
                }
            }
            if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::SliderFloat("Theta", &this->light.theta, 0, M_PIf32) ||
                    ImGui::SliderFloat("Phi", &this->light.phi, 0, 2.f * M_PIf32)) {
                    this->light.computeDirection();
                    this->shadowMap->modified = true;
                }
                
                ImGui::SliderFloat("Directional Light Intensity", &this->light.dirIntensity, 0, 1.f);
                ImGui::SliderFloat("Ambient Light Intensity", &this->light.ambIntensity, 0, 1.f);
                ImGui::ColorEdit3("color", reinterpret_cast<float *>(&this->light.color));
                ImGui::Checkbox("Light from camera", &this->light.fromCamera);
                ImGui::Separator();
                ImGui::Checkbox("Enable shadow", &this->shadowMap->enabled);
                ImGui::SliderFloat("Shadow Map Bias", &this->shadowMap->bias, 0, 0.5f);
            }
            ImGui::End();
        }
        
        imguiRenderFrame();
    }
    
    
    static glm::vec3 computeDirectionVectorUp(float phiRadians, float thetaRadians) {
        const GLfloat cosPhi = glm::cos(phiRadians);
        const GLfloat sinPhi = glm::sin(phiRadians);
        const GLfloat cosTheta = glm::cos(thetaRadians);
        return -glm::normalize(glm::vec3(sinPhi * cosTheta, -glm::sin(thetaRadians), cosPhi * cosTheta));
    }
    
    
    void ViewerApplication::drawNodeShadowMap(int nodeIdx, const glm::mat4 &parentMatrix) {
        tinygltf::Node node = model.nodes[nodeIdx];
        glm::mat4 modelMatrix = getLocalToWorldMatrix(node, parentMatrix);
        
        if (node.mesh >= 0) {
            glm::mat4 MVPMatrix = this->shadowMap->viewProjectionMatrix * modelMatrix;
            
            this->shadowMap->shader->loadUniform("uMVPMatrix", glm::value_ptr(MVPMatrix));
            
            VaoRange vaoRange = meshToVertexArrays[node.mesh];
            tinygltf::Mesh mesh = model.meshes[node.mesh];
            for (uint32_t i = 0; i < mesh.primitives.size(); ++i) {
                GLuint vao = vertexArrayObjects[vaoRange.begin + i];
                tinygltf::Primitive primitive = mesh.primitives[i];
                
                glBindVertexArray(vao);
                if (primitive.indices >= 0) {
                    tinygltf::Accessor accessor = model.accessors[primitive.indices];
                    tinygltf::BufferView bufferView = model.bufferViews[accessor.bufferView];
                    uint64_t byteOffset = accessor.byteOffset + bufferView.byteOffset;
                    glDrawElements(primitive.mode, accessor.count, accessor.componentType,
                                   reinterpret_cast<const GLvoid *>(byteOffset));
                }
                else {
                    uint32_t index = (*begin(primitive.attributes)).second;
                    tinygltf::Accessor accessor = model.accessors[index];
                    glDrawArrays(primitive.mode, 0, accessor.count);
                }
            }
        }
        
        // Draw children
        std::for_each(node.children.begin(), node.children.end(),
                      [this, &modelMatrix](uint32_t i) { this->drawNodeShadowMap(i, modelMatrix); }
        );
    }
    
    
    void ViewerApplication::computeShadowMap() {
        const glm::vec3 dirLightUpVector = computeDirectionVectorUp(this->light.phi, this->light.theta);
        const glm::mat4 dirLightViewMatrix = glm::lookAt(
                this->sceneCenter + this->light.direction * this->sceneDiameter, this->sceneCenter, dirLightUpVector
        );
        const glm::mat4 dirLightProjMatrix = glm::ortho(
                -this->sceneDiameter, this->sceneDiameter, -this->sceneDiameter, this->sceneDiameter,
                0.01f * this->sceneDiameter, 2.f * this->sceneDiameter
        );
        
        this->shadowMap->viewProjectionMatrix = dirLightProjMatrix * dirLightViewMatrix;
        
        this->shadowMap->shader->use();
        glCullFace(GL_FRONT);
        glViewport(0, 0, shader::ShadowMap::SHADOW_MAP_RESOLUTION, shader::ShadowMap::SHADOW_MAP_RESOLUTION);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->shadowMap->FBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        if (this->model.defaultScene >= 0) {
            std::for_each(this->model.scenes[this->model.defaultScene].nodes.begin(),
                          this->model.scenes[this->model.defaultScene].nodes.end(),
                          [this](uint32_t i) { this->drawNodeShadowMap(i, glm::mat4(1)); }
            );
        }
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glCullFace(GL_BACK);
        this->shadowMap->shader->stop();
        
        this->shadowMap->modified = false;
    }
    
    
    int ViewerApplication::run() {
        double seconds, ellapsedTime;
        
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
        for (auto iterationCount = 0u; !this->glfwHandle.shouldClose(); ++iterationCount) {
            seconds = glfwGetTime();
            
            if (this->shadowMap->enabled && this->shadowMap->modified) {
                this->computeShadowMap();
            }
            
            this->drawScene();
            this->drawImGUI();
            
            glfwPollEvents();
            ellapsedTime = glfwGetTime() - seconds;
            if (!ImGui::GetIO().WantCaptureMouse) {
                cameraController->update(static_cast<GLfloat>(ellapsedTime));
            }
            
            this->glfwHandle.swapBuffers();
        }
        
        // TODO clean up allocated GL data
        
        return 0;
    }
} // namespace viewer
