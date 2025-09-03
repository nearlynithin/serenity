#ifndef GAME_H
#define GAME_H

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
    Color backg = ColorFromNormalized(Vector4{0.8, 1.0, 0.8, 1.0});
    bool mIsRunning;
};

#endif
