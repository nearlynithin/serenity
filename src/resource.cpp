#include "game/resource.hpp"
#include "raylib.h"

void ResourceLoader::LoadAllTextures()
{
    auto &resourceManager = ResourceManager::getInstance();
    resourceManager.LoadTex("terrainTexture", "assets/texture.png");

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
    resourceManager.Loadshader("terrainShader", "assets/shaders/lighting.vs", "assets/shaders/lighting.fs");
    resourceManager.Loadshader("shadowShader", "assets/shaders/shadowmap.vs", "assets/shaders/shadowmap.fs");
    resourceManager.Loadshader("grassShader", "assets/shaders/lighting_instancing.vs", "assets/shaders/lighting.fs");
    resourceManager.Loadshader("skyboxShader", "assets/shaders/skybox.vs", "assets/shaders/skybox.fs");
    resourceManager.Loadshader("cubemapShader", "assets/shaders/cubemap.vs", "assets/shaders/cubemap.fs");
    // resourceManager.Loadshader("light", "assets/shaders/lighting.vs", "assets/shaders/lighting.fs");
}

void ResourceLoader::LoadAllModels()
{
    auto &resourceManager = ResourceManager::getInstance();
    // resourceManager.Loadmodel("oiiaioooooiai", "assets/cat.glb");
    resourceManager.Loadmodel("grass", "assets/grass.obj");
    SetTextureFilter(resourceManager.getModel("grass").materials[0].maps[MATERIAL_MAP_DIFFUSE].texture,
                     TEXTURE_FILTER_TRILINEAR);
    resourceManager.LoadSkyboxModel("skybox", 1.0f, 1.0f, 1.0f);
}
