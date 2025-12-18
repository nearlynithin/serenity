#include "game/player.hpp"
#include "game/resource.hpp"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

void Player::InitPlayer()
{
    auto rm = ResourceManager::getInstance();
    position = Vector3{0, 5, 0};
    height = 3.0f;
    speed = 5.0f;
    animFPS = 1.0f / 120.0f; // 70 FPS
    model = rm.getModel("player");
    anims = rm.getModelAnimation("player");
    camera = {0};
    camera.position = {10.0f, 10.0f, 10.0f};
    camera.target = {0.0f, 0.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    moveLerpFactor = 4;
    mouseSensitivity = 0.003f;
    cameraLerpFactor = 20.0f;
    yaw = 0.0f;
    pitch = 0.0f;
    modelScale = 2.0f;
    boneWorldPos = std::make_unique<Vector3[]>(model.boneCount);
    boneParents = std::make_unique<int[]>(model.boneCount);

    katana.InitKatana();
}

Vector3 Player::GetPlayerPosition()
{
    return position;
}

// Listen for inputs
void Player::PlayerMoves()
{
    float delta = GetFrameTime();
    speed = (IsKeyDown(KEY_LEFT_SHIFT)) ? PLAYER_RUN_SPEED : PLAYER_WALK_SPEED;

    // reset at the start of movement handling
    targetMoveDir = {0.0f, 0.0f, 0.0f};

    Vector3 forward = {sinf(yaw), 0.0f, cosf(yaw)};
    Vector3 right = {cosf(yaw), 0.0f, -sinf(yaw)};

    forward = Vector3Normalize(forward);
    right = Vector3Normalize(right);

    animIndex = 2;
    if (IsKeyDown(KEY_W))
    {
        targetMoveDir = Vector3Subtract(targetMoveDir, forward);
    }
    if (IsKeyDown(KEY_A))
    {
        targetMoveDir = Vector3Subtract(targetMoveDir, right);
    }
    if (IsKeyDown(KEY_S))
    {
        targetMoveDir = Vector3Add(targetMoveDir, forward);
    }
    if (IsKeyDown(KEY_D))
    {
        targetMoveDir = Vector3Add(targetMoveDir, right);
    }

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D))
    {
        animIndex = 0;
        if (IsKeyDown(KEY_LEFT_SHIFT))
            animIndex = 1;
    }

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
        animIndex = 3;
    }

    if (Vector3Length(targetMoveDir) > 0.001f)
    {
        targetMoveDir = Vector3Normalize(targetMoveDir);
    }
    Vector3 targetVel = Vector3Scale(targetMoveDir, speed);
    float t = moveLerpFactor * delta;

    moveVelocity.x += (targetVel.x - moveVelocity.x) * t;
    moveVelocity.y += (targetVel.y - moveVelocity.y) * t;
    moveVelocity.z += (targetVel.z - moveVelocity.z) * t;

    position = Vector3Add(position, Vector3Scale(moveVelocity, delta));
    if (Vector3Length(moveVelocity) > 0.001f)
    {
        modelYaw = atan2f(moveVelocity.x, moveVelocity.z) * RAD2DEG;
    }
}

void Player::UpdatePlayer()
{
    float dt = GetFrameTime();
    animTime += dt;

    while (animTime >= animFPS)
    {
        animTime -= animFPS;
        animCurrentFrame = (animCurrentFrame + 1) % anims.modelAnim[animIndex].frameCount;
    }

    updateBoneWordldPositions(animCurrentFrame);
    UpdateModelAnimation(model, anims.modelAnim[animIndex], animCurrentFrame);
}

void Player::UpdateCamera()
{
    Vector3 playerPos = GetPlayerPosition();
    Vector2 mouseDelta = GetMouseDelta();
    float delta = GetFrameTime();

    playerPos = Vector3Add(playerPos, Vector3{0.0f, height, 0.0f});

    yaw -= mouseDelta.x * mouseSensitivity;
    pitch += mouseDelta.y * mouseSensitivity;

    pitch = Clamp(pitch, -PI / 2 + 0.1f, PI / 2 - 0.8f);

    float distance = 5.0f;
    Vector3 offset = {cosf(pitch) * sinf(yaw) * distance, sinf(pitch) * distance + 2.0f,
                      cosf(pitch) * cosf(yaw) * distance};

    targetCamera.position = Vector3Add(playerPos, offset);
    targetCamera.target = playerPos;

    float t = cameraLerpFactor * delta;

    // Lerp position
    camera.position.x += (targetCamera.position.x - camera.position.x) * t;
    camera.position.y += (targetCamera.position.y - camera.position.y) * t;
    camera.position.z += (targetCamera.position.z - camera.position.z) * t;

    // Lerp target
    camera.target.x += (targetCamera.target.x - camera.target.x) * t;
    camera.target.y += (targetCamera.target.y - camera.target.y) * t;
    camera.target.z += (targetCamera.target.z - camera.target.z) * t;
}

void Player::setPlayerPosition(Vector3 pos)
{
    position = pos;
}

void Player::DrawPlayer(SceneContext *sceneCtx)
{
    for (int i = 0; i < model.materialCount; i++)
    {
        model.materials[i].shader = sceneCtx->terrainShader->getShader();
    }
    DrawModelEx(model, position, Vector3{0.0f, 1.0f, 0.0f}, modelYaw, Vector3{2.0f, 2.0f, 2.0f}, WHITE);
    katana.Draw(anims.modelAnim[animIndex].framePoses[animCurrentFrame][45], model.bindPose[45].rotation, modelMatrix, sceneCtx);
}

void Player::DrawSkeleton()
{
    for (int i = 0; i < model.boneCount; i++)
    {
        DrawCube(boneWorldPos[i], 0.05f, 0.05f, 0.05f, RED);

        int parent = boneParents[i];
        if (parent >= 0)
        {
            DrawLine3D(boneWorldPos[i], boneWorldPos[parent], RED);
        }
    }
}

void Player::updateBoneWordldPositions(int frame)
{
    Matrix mT = MatrixTranslate(position.x, position.y, position.z);
    Matrix mR = MatrixRotateY(modelYaw * DEG2RAD);
    Matrix mS = MatrixScale(modelScale, modelScale, modelScale);
    modelMatrix = MatrixMultiply(MatrixMultiply(mS, mR), mT);

    for (int i = 0; i < model.boneCount; i++)
    {
        Vector3 local = anims.modelAnim[animIndex].framePoses[frame][i].translation;
        boneWorldPos[i] = Vector3Transform(local, modelMatrix);
        boneParents[i] = anims.modelAnim[animIndex].bones[i].parent;
    }
}
