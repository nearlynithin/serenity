#ifndef GAME_H
#define GAME_H

#include "game/scene.hpp"
#include "game/physics.hpp"
#include "raylib.h"

class Game
{
  public:
    Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();

    Scene scene;
    PhysicsSystem physics;
    ResourceManager resource;

  private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    float time;

    const int screenWidth;
    const int screenHeight;
    Color backg = ColorFromNormalized(Vector4{0.8, 1.0, 0.8, 1.0});
    bool mIsRunning;
};

#endif
