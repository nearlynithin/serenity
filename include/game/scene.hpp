#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"

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

  public:
    static Scene &getInstance()
    {
        static Scene instance;
        return instance;
    }

    void SetModels();
    void SetLights();
    void SetShaders();
    void UpdateShaders(Camera3D *camera);
    void ScenePrep();
    void DrawScene();
};

#endif