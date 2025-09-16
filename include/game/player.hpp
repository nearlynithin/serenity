#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "renderContext.hpp"

#define VIEW_BOX_WIDTH 40
#define VIEW_BOX_HEIGHT 30
#define VIEW_BOX_LENGTH 75
#define VIEW_BOX_DISTANCE 50.0f

#define PLAYER_RUN_SPEED 18.0f
#define PLAYER_WALK_SPEED 8.0f

class Player
{
  private:
    Vector3 position;
    Vector3 targetPosition;
    Vector3 moveDir;
    Vector3 targetMoveDir;
    int moveLerpFactor;
    Vector3 moveVelocity;
    float modelYaw;
    Model model;
    float speed;
    float height;
    int animCurrentFrame;
    int animIndex;
    float animTime;
    float animFPS;

  public:
    static Camera3D camera;
    static Camera3D targetCamera;
    static float mouseSensitivity;
    static float yaw;
    static float pitch;
    static int cameraLerpFactor;
    static Player &getInstance()
    {
        static Player instance;
        return instance;
    }
    static void InitPlayer();
    static void DrawPlayer(SceneContext *sceneCtx);
    static Vector3 GetPlayerPosition();
    static void PlayerMoves();
    static void UpdateCamera();
    static void setPlayerPosition(Vector3 pos);
    static void UpdatePlayer();
};

#endif
