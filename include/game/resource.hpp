#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "raylib.h"
#include <iostream>
#include <string>
#include <unordered_map>

class ResourceManager
{
  private:
    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Shader> shaders;

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
            std::cerr << "Shader not found : " << name << "\n";
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
    }
};

#endif