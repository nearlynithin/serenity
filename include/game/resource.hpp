#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "raylib.h"
#include "rlgl.h"
#include "shader.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

// Some macros related to shaders
#define SHADOWMAP_RESOLUTION 2048
#define SHADOW_MAP_SLOT 1

struct ModelAnim
{
    ModelAnimation *modelAnim;
    int animsCount;
};

class ResourceManager
{
  private:
    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, std::shared_ptr<gfx::Shader>> shaders;
    std::unordered_map<std::string, Model> models;
    std::unordered_map<std::string, ModelAnim> modelAnimations;
    RenderTexture2D shadowMap;

  public:

    void LoadAllShaders();
    void LoadAllTextures();
    void LoadAllModels();

    // Texture Manager
    void LoadTex(const std::string &textureName, const std::string &filepath)
    {
        std::cout << filepath.c_str();
        textures[textureName] = LoadTexture(filepath.c_str());

        if (!IsTextureValid(textures[textureName]))
            std::cout << "Not ready " << textureName << "\n";
    }

    Texture2D &GetTexture(const std::string &name)
    {
        auto it = textures.find(name);
        if (it == textures.end())
        {
            std::cerr << "Texture not found : " << name << "\n";
        }
        return it->second;
    }

    void LoadShadowmapRenderTexture(int width, int height)
    {
        shadowMap.id = rlLoadFramebuffer(); // Load an empty framebuffer
        shadowMap.texture.width = width;
        shadowMap.texture.height = height;

        if (shadowMap.id > 0)
        {
            rlEnableFramebuffer(shadowMap.id);

            // Create depth texture
            // We don't need a color texture for the shadowmap
            shadowMap.depth.id = rlLoadTextureDepth(width, height, false);
            shadowMap.depth.width = width;
            shadowMap.depth.height = height;
            shadowMap.depth.format = 19; // DEPTH_COMPONENT_24BIT?
            shadowMap.depth.mipmaps = 1;

            // Attach depth texture to FBO
            rlFramebufferAttach(shadowMap.id, shadowMap.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

            // Check if fbo is complete with attachments (valid)
            if (rlFramebufferComplete(shadowMap.id))
                std::cout << "FBO: [ID " << shadowMap.id << "] Framebuffer object created successfully\n";

            rlDisableFramebuffer();
        }
        else
            std::cerr << "FBO: Framebuffer object can not be created\n";
    }

    RenderTexture2D *getShadowMap()
    {
        return &shadowMap;
    }


    // Shader Manager
    void Loadshader(const std::string &shaderName, const std::string &vertexShaderPath,
                    const std::string &fragmentShaderPath)
    {
        auto [it, inserted] =
            shaders.emplace(shaderName, std::make_shared<gfx::Shader>(shaderName, vertexShaderPath.c_str(),
                                                                      fragmentShaderPath.c_str()));

        if (!IsShaderValid(it->second->getShader()))
        {
            std::cerr << "Shader not ready : " << shaderName << "\n";
        }
    }

    gfx::Shader *getShader(const std::string &name)
    {
        auto it = shaders.find(name);
        if (it == shaders.end())
        {
            std::cout << "Shader not found : " << name << "\n";
        }
        return it->second.get();
    }

    // Model Manager
    void Loadmodel(const std::string &modelName, const std::string &filename)
    {
        models[modelName] = LoadModel(filename.c_str());
        if (!IsModelValid(models[modelName]))
        {
            std::cerr << "Model not ready : " << modelName << "\n";
        }
    }
    void loadModelAnimation(const std::string &animationName, const std::string &filename)
    {
        ModelAnim modelAnim;
        modelAnim.modelAnim = LoadModelAnimations(filename.c_str(), &modelAnim.animsCount);
        if (modelAnim.animsCount <= 0)
        {
            std::cerr << "No animations found in file : " << filename << "\n";
            return;
        }
        modelAnimations[animationName] = modelAnim;
    }

    ModelAnim &getModelAnimation(const std::string &name)
    {
        auto it = modelAnimations.find(name);
        if (it == modelAnimations.end())
        {
            std::cerr << "Animation : " << name << " was not loaded\n";
        }
        return it->second;
    }

    void LoadSkyboxModel(const std::string &modelName, float width, float height, float length)
    {
        models[modelName] = LoadModelFromMesh(GenMeshCube(width, height, length));
        if (!IsModelValid(models[modelName]))
        {
            std::cerr << "skybox not ready\n";
        }
    }

    Model &getModel(const std::string &modelName)
    {
        auto it = models.find(modelName);
        if (it == models.end())
        {
            std::cerr << "Model not found : " << modelName << "\n";
        }
        return it->second;
    }

    // Manually unloading resources
    void UnloadAll()
    {
        if (!textures.empty())
        {
            for (auto &texture : textures)
            {
                UnloadTexture(texture.second);
            }
            textures.clear();
            std::cout << "Textures unloaded\n";
        }

        if (!shaders.empty())
        {
            for (auto &shader : shaders)
            {
                if (!shader.first.empty())
                    UnloadShader(shader.second->getShader());
            }
            shaders.clear();
            std::cout << "Shaders unloaded\n";
        }

        if (!models.empty())
        {
            for (auto &model : models)
            {
                UnloadModel(model.second);
            }
            models.clear();
            std::cout << "Models unloaded\n";
        }
    }
};

#endif
