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
    Camera3D camera;
    Camera3D targetCamera;
    float mouseSensitivity;
    float yaw;
    float pitch;
    int cameraLerpFactor;
    void InitPlayer();
    void DrawPlayer(SceneContext *sceneCtx);
    Vector3 GetPlayerPosition();
    void PlayerMoves();
    void UpdateCamera();
    void setPlayerPosition(Vector3 pos);
    void UpdatePlayer();
    void DrawSkeleton(Model *model);
};

#endif
