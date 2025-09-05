#include "game/game.hpp"
#include "game/player.hpp"
#include "game/resource.hpp"
#include "game/scene.hpp"
#include "game/terrain.hpp"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <iostream>

Game::Game()
  : screenWidth(1280),
    screenHeight(720),
    mIsRunning(false),
    time(GetTime())
{
}

bool Game::Initialize()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_WINDOW_UNDECORATED);
    InitWindow(screenWidth, screenHeight, "Serenity");
    DisableCursor();

    ResourceLoader::LoadAllTextures();
    ResourceLoader::LoadAllShaders();
    ResourceLoader::LoadAllModels();

    Scene::getInstance().InitScene();

    if (IsWindowReady())
    {
        mIsRunning = true;
    }
    else
    {
        mIsRunning = false;
    }
    return mIsRunning;
}

void Game::Shutdown()
{
    // Destroying resource manager
    ResourceManager::getInstance().UnloadAll();

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
    // will be scene inputs later on
    Player::PlayerMoves();
}

void Game::UpdateGame()
{
    Scene::getInstance().UpdateScene();
}

void Game::GenerateOutput()
{
    Scene::getInstance().DrawScene();

    DrawFPS(20, 20);

    EndDrawing();
}
