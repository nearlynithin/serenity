#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

#define VIEW_BOX_WIDTH 40
#define VIEW_BOX_HEIGHT 30
#define VIEW_BOX_LENGTH 75
#define VIEW_BOX_DISTANCE 50.0f

class Player
{
  private:
    Vector3 position;
    Vector3 moveDir;
    float modelYaw;
    Model model;
    float speed;
    float height;
    int animCurrentFrame;

  public:
    static Camera3D camera;
    static float mouseSensitivity;
    static float yaw;
    static float pitch;
    static Player &getInstance()
    {
        static Player instance;
        return instance;
    }
    static void InitPlayer();
    static void DrawPlayer();
    static Vector3 GetPlayerPosition();
    static void PlayerMoves();
    static void UpdateCamera();
    static void setPlayerPosition(Vector3 pos);
    static void UpdatePlayer();
};

#endif
