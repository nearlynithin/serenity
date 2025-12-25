#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "renderContext.hpp"
#include "resource.hpp"
#include "weapon.hpp"
#include <vector>

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

    ResourceManager *rm;
    Model model;
    Weapon katana;
    Matrix modelMatrix;
    float modelScale;
    std::unique_ptr<Vector3[]> boneWorldPos;
    std::unique_ptr<int[]> boneParents;
    float modelYaw;
    ModelAnim *anims;
    float animFPS;
    int animCurrentFrame;
    int animIndex;
    float animTime;

    float speed;
    float height;
    float mouseSensitivity;
    float yaw;
    float pitch;
    int cameraLerpFactor;
    Camera3D targetCamera;

  public:
    Camera3D camera;
    void InitPlayer(ResourceManager *rm);
    void DrawPlayer(SceneContext *sceneCtx);
    Vector3 GetPlayerPosition();
    void PlayerMoves();
    void UpdateCamera();
    void setPlayerPosition(Vector3 pos);
    void UpdatePlayer();
    void DrawSkeleton();
    void updateBoneWordldPositions(int frame);
};

#endif
