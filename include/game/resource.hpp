#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "raylib.h"
#include "rlgl.h"
#include <iostream>
#include <string>
#include <unordered_map>

// Some macros related to shaders
#define SHADOWMAP_RESOLUTION 512
#define SHADOW_MAP_SLOT 1

class ResourceManager
{
  private:
    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Shader> shaders;
    std::unordered_map<std::string, Model> models;

  public:
    static ResourceManager &getInstance()
    {
        static ResourceManager instance;
        return instance;
    }

    // Texture Manager
    void LoadTex(const std::string &textureName, const std::string &filepath)
    {
        std::cout << filepath.c_str();
        textures[textureName] = LoadTexture(filepath.c_str());

        if (!IsTextureReady(textures[textureName]))
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

    RenderTexture2D LoadShadowmapRenderTexture(int width, int height)
    {
        RenderTexture2D target = {0};

        target.id = rlLoadFramebuffer(); // Load an empty framebuffer
        target.texture.width = width;
        target.texture.height = height;

        if (target.id > 0)
        {
            rlEnableFramebuffer(target.id);

            // Create depth texture
            // We don't need a color texture for the shadowmap
            target.depth.id = rlLoadTextureDepth(width, height, false);
            target.depth.width = width;
            target.depth.height = height;
            target.depth.format = 19; // DEPTH_COMPONENT_24BIT?
            target.depth.mipmaps = 1;

            // Attach depth texture to FBO
            rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

            // Check if fbo is complete with attachments (valid)
            if (rlFramebufferComplete(target.id))
                std::cout << "FBO: [ID " << target.id << "] Framebuffer object created successfully\n";

            rlDisableFramebuffer();
        }
        else
            std::cerr << "FBO: Framebuffer object can not be created\n";

        return target;
    }

    // Shader Manager
    void Loadshader(const std::string &shaderName, const std::string &vertexShaderPath,
                    const std::string &fragmentShaderPath)
    {
        shaders[shaderName] = LoadShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

        if (!IsShaderReady(shaders[shaderName]))
        {
            std::cerr << "Shader not ready : " << shaderName << "\n";
        }
    }

    Shader &getShader(const std::string &name)
    {
        auto it = shaders.find(name);
        if (it == shaders.end())
        {
            std::cout << "Shader not found : " << name << "\n";
        }
        return it->second;
    }

    // Model Manager
    void Loadmodel(const std::string &modelName, const std::string &filename)
    {
        models[modelName] = LoadModel(filename.c_str());
        if (!IsModelReady(models[modelName]))
        {
            std::cerr << "Model not ready : " << modelName << "\n";
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
                UnloadShader(shader.second);
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

class ResourceLoader
{
  public:
    static RenderTexture2D &getShadowMap()
    {
        static RenderTexture2D shadowMap;
        return shadowMap;
    }
    static void LoadAllShaders();
    static void LoadAllTextures();
    static void LoadAllModels();
};

#endif