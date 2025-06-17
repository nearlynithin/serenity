#ifndef SCENE_H
#define SCENE_H

#include "lights.hpp"
#include "raylib.h"
#include <unordered_map>
#include <memory>

class Scene
{
  private:
    Camera3D lightCam;
    Vector3 lightDir;
    int lightDirLoc;
    int lightVPLoc;
    int shadowMapLoc;
    Matrix lightView;
    Matrix lightProj;
    Matrix lightViewProj;
    float fogDensity;
    float fogDensityLoc;
    int ambientLoc;
    float ambient[4] = {0.1f, 0.1f, 0.1f, 0.1f};

  public:
    static std::unordered_map<std::string, std::unique_ptr<Light>> lights;

    static Scene &getInstance()
    {
        static Scene instance;
        return instance;
    }

    void SetModels();
    void SetLights();
    void SetShaders();
    void UpdateShaders();
    void ScenePrep();
    void DrawScene();
};

#endif