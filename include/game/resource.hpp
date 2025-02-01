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

    Texture2D GetTexture(const std::string &name)
    {
        auto it = textures.find(name);
        if (it == textures.end())
        {
            std::cerr << "Texture not found : " << name << "\n";
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
    }
};

#endif