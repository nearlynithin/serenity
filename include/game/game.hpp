#ifndef GAME_H
#define GAME_H

#include "raylib.h"

class Game {
public:
  Game();
  bool Initialize();
  void RunLoop();
  void Shutdown();

private:
  void ProcessInput();
  void UpdateGame();
  void GenerateOutput();

  const int screenWidth;
  const int screenHeight;
  bool mIsRunning;
};

#endif