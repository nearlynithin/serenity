#include "game/game.hpp"
#include "game/grass.hpp"
#include "game/lights.hpp"
#include "game/player.hpp"
#include "game/resource.hpp"
#include "game/scene.hpp"
#include "game/terrain.hpp"
#include "raymath.h"
#include "rlgl.h"
#include <iostream>
#include <raylib.h>

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
    TerrainManager::LoadTerrains();

    Player::InitPlayer();

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
    Player::PlayerMoves();
}

void Game::UpdateGame()
{
    Player::UpdateCamera();
    TerrainManager::UpdateTerrains();
}

void Game::GenerateOutput()
{
    time += GetFrameTime();

    BeginDrawing();
    Color backg = ColorFromNormalized(Vector4{0.8, 1.0, 0.8, 1.0});
    ClearBackground(backg);

    BeginMode3D(Player::camera);
    Scene::getInstance().DrawScene();
    EndMode3D();

    DrawFPS(20, 20);

    EndDrawing();
}