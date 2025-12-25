#ifndef SCENE_H
#define SCENE_H

#include "player.hpp"
#include "renderContext.hpp"
#include "terrain.hpp"

class Scene
{
  private:
    SceneContext sceneCtx{};
    shadowMapData shadowData;
    Player player;
    TerrainManager terrainManager;

  public:
    void InitScene(ResourceManager *rm);
    void ProcessInput();
    void UpdateScene();
    void DrawScene(ResourceManager *rm);

    void SetModels();
    void SetLights();
    void SetShaders(ResourceManager *rm);
    void UpdateShaders();
    void InitShadowMapping(ResourceManager *rm);
    void RenderShadowMap();
    void UpdateLightSpaceMatrix();
    void SetTerrainShaderMatrices();
};

#endif
