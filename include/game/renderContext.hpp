#pragma once

#include "raylib.h"
#include "shader.hpp"

struct SceneContext
{
    Camera3D lightCam;
    Vector3 lightDir;
    Vector4 lightColor;
    float ambient[4];
    Matrix lightViewProj;
    int renderPass;
    gfx::Shader *grassShader;
    gfx::Shader *terrainShader;
    gfx::Shader *skyboxShader;
    gfx::Shader *shadowMapShader;
    gfx::Shader *cubemapShader;
};

struct shadowMapData
{
    RenderTexture2D *shadowMap;
    Matrix lightView;
    Matrix lightProj;
    Matrix lightViewProj;
    int shadowMapSlot;
    shadowMapData()
      : shadowMapSlot(1)
    {
    }
};
