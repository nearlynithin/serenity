#ifndef SCENE_H
#define SCENE_H

#include "player.hpp"
#include "renderContext.hpp"
#include "terrain.hpp"

class Scene
{
  private:
    SceneContext sceneCtx;
    shadowMapData shadowData;
    Player player;
    TerrainManager terrainManager;

  public:
    static Scene &getInstance()
    {
        static Scene instance;
        return instance;
    }

    void InitScene();
    void ProcessInput();
    void UpdateScene();
    void DrawScene();

    void SetModels();
    void SetLights();
    void SetShaders();
    void UpdateShaders();
    void InitShadowMapping();
    void RenderShadowMap();
    void UpdateLightSpaceMatrix();
    void SetTerrainShaderMatrices();
};

#endif
