#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

class Player
{
  private:
    Vector3 position;
    Model model;
    float speed;
    float height;

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
    static Vector2 getPlayerCords();
    static void PlayerMoves();
    static void UpdateCamera();
};

#endif