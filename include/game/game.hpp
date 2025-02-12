#ifndef GAME_H
#define GAME_H

#include "game/lights.hpp"
#include "game/resource.hpp"
#include "game/terrain.hpp"
#include "raylib.h"
#include <memory>
#include <unordered_map>
#include <vector>

class Game
{
  public:
    Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();

  private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    void LoadAllTextures();
    void LoadAllShaders();

    void SetLights();
    void SetShaders();
    void UpdateShaders();

    float time;

    const int screenWidth;
    const int screenHeight;
    bool mIsRunning;

    float x = 0;
    float y = 0;
    float z = 0;

    Camera camera;
    Camera3D lightCam;
    Vector3 lightDir;
    int lightDirLoc;
    int lightVPLoc;
    int shadowMapLoc;
    Matrix lightView;
    Matrix lightProj;
    Matrix lightViewProj;
    RenderTexture2D shadowMap;
    ResourceManager resourceManager;
    std::vector<std::unique_ptr<Terrain>> terrains;
};

#endif