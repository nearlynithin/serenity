#include "game/game.hpp"
#include <iostream>

Game::Game()
  : screenWidth(1280),
    screenHeight(720),
    mIsRunning(false)
{
}

bool Game::Initialize()
{
    InitWindow(screenWidth, screenHeight, "Cpp setup");
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
    std::cout << "Process input\n";
}

void Game::UpdateGame()
{
    std::cout << "Updating game elements\n";
}

void Game::GenerateOutput()
{
    BeginDrawing();
    ClearBackground(WHITE);

    DrawText("Demo output", 20, 20, 40, BLACK);

    EndDrawing();
}