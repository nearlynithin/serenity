#include "game/resource.hpp"
#include "raylib.h"

void ResourceManager::LoadAllTextures()
{
    LoadTex("terrainTexture", "assets/texture.png");

    LoadShadowmapRenderTexture(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
    for (int param : {RL_TEXTURE_MIN_FILTER, RL_TEXTURE_MAG_FILTER, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_T})
    {
        rlTextureParameters(shadowMap.depth.id, param, RL_TEXTURE_FILTER_NEAREST);
    }
}

void ResourceManager::LoadAllShaders()
{
    Loadshader("terrainShader", "assets/shaders/lighting.vert", "assets/shaders/lighting.frag");
    Loadshader("shadowMapShader", "assets/shaders/shadowmap.vert", "assets/shaders/shadowmap.frag");
    Loadshader("grassShader", "assets/shaders/grass.vert", "assets/shaders/grass.frag");
    Loadshader("skyboxShader", "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
    Loadshader("cubemapShader", "assets/shaders/cubemap.vert", "assets/shaders/cubemap.frag");
}

void ResourceManager::LoadAllModels()
{
    // resourceManager.Loadmodel("oiiaioooooiai", "assets/cat.glb");
    Loadmodel("grass", "assets/grass.obj");
    SetTextureFilter(getModel("grass").materials[0].maps[MATERIAL_MAP_DIFFUSE].texture,
                     TEXTURE_FILTER_TRILINEAR);
    LoadSkyboxModel("skybox", 1.0f, 1.0f, 1.0f);
    Loadmodel("player", "assets/player.glb");
    Loadmodel("katana", "assets/katana.glb");

    // Loading model animations
    loadModelAnimation("player", "assets/player.glb");
}
