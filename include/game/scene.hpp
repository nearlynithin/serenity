#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"
#include "renderContext.hpp"
#include "shader.hpp"
#include <memory>
#include <unordered_map>

class Scene
{
  private:
    SceneContext sceneCtx;
    shadowMapData shadowData;

  public:
    static Scene &getInstance()
    {
        static Scene instance;
        return instance;
    }

    void InitScene();
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
