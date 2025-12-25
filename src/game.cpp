#include "game/game.hpp"
#include "game/resource.hpp"
#include "game/scene.hpp"
#include "raylib.h"
#include "rlgl.h"

Game::Game()
  : screenWidth(1280),
    screenHeight(720),
    mIsRunning(false)
{}

bool Game::Initialize()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_WINDOW_UNDECORATED);
    InitWindow(screenWidth, screenHeight, "Serenity");
    time = GetTime();
    DisableCursor();

    resource.LoadAllTextures();
    resource.LoadAllShaders();
    resource.LoadAllModels();

    scene.InitScene(&resource);

    if (IsWindowReady())
    {
        mIsRunning = true;
    } else
    {
        mIsRunning = false;
    }
    return mIsRunning;
}

void Game::Shutdown()
{
    // Destroying resource manager
    resource.UnloadAll();

    if (mIsRunning)
    {
        mIsRunning = false;
        CloseWindow();
    }
}

void Game::RunLoop()
{
    while (mIsRunning && !WindowShouldClose())
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    scene.ProcessInput();
}

void Game::UpdateGame()
{
    physics.Update(&scene);
    scene.UpdateScene();
}

void Game::GenerateOutput()
{
    BeginDrawing();

    scene.DrawScene(&resource);

    DrawFPS(20, 20);

    EndDrawing();
}
