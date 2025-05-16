#ifndef GAME_H
#define GAME_H

#include "game/lights.hpp"
#include "game/resource.hpp"
#include "game/scene.hpp"
#include "game/terrain.hpp"
#include "raylib.h"
#include <memory>
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

    float time;

    const int screenWidth;
    const int screenHeight;
    bool mIsRunning;

    float x = 0;
    float y = 0;
    float z = 0;
};

#endif