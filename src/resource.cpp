#include "game/resource.hpp"
#include "raylib.h"

void ResourceLoader::LoadAllTextures()
{
    auto &resourceManager = ResourceManager::getInstance();
    resourceManager.LoadTex("terrainTexture", "assets/ground.png");

    RenderTexture2D shadowMap = ResourceLoader::getShadowMap();

    shadowMap = resourceManager.LoadShadowmapRenderTexture(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
    for (int param : {RL_TEXTURE_MIN_FILTER, RL_TEXTURE_MAG_FILTER, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_T})
    {
        rlTextureParameters(shadowMap.depth.id, param, RL_TEXTURE_FILTER_NEAREST);
    }
}

void ResourceLoader::LoadAllShaders()
{
    auto &resourceManager = ResourceManager::getInstance();
    // resourceManager.Loadshader("terrainLightShader", "assets/shaders/lighting.vs", "assets/shaders/lighting.fs");
    resourceManager.Loadshader("shadowShader", "assets/shaders/shadowmap.vs", "assets/shaders/shadowmap.fs");
    resourceManager.Loadshader("grassShader", "assets/shaders/lighting_instancing.vs", "assets/shaders/lighting.fs");
}
