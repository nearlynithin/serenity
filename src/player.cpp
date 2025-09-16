#include "game/player.hpp"
#include "game/resource.hpp"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

Camera3D Player::camera;
float Player::mouseSensitivity;
float Player::yaw;
float Player::pitch;
Camera3D Player::targetCamera;
int Player::cameraLerpFactor = 15;

void Player::InitPlayer()
{
    Player &player = Player::getInstance();
    player.position = Vector3{0, 0, 0};
    player.height = 3.0f;
    player.speed = 5.0f;
    player.animFPS = 1.0f / 120.0f; // 70 FPS
    player.model = ResourceManager::getInstance().getModel("player");
    Player::camera = {0};
    camera.position = {10.0f, 10.0f, 10.0f};
    camera.target = {0.0f, 0.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    player.moveLerpFactor = 4;
    Player::mouseSensitivity = 0.003f;
    Player::yaw = 0.0f;
    Player::pitch = 0.0f;
}

Vector3 Player::GetPlayerPosition()
{
    return Player::getInstance().position;
}

// Listen for inputs
void Player::PlayerMoves()
{
    Player &player = Player::getInstance();
    float delta = GetFrameTime();
    player.speed = (IsKeyDown(KEY_LEFT_SHIFT)) ? PLAYER_RUN_SPEED : PLAYER_WALK_SPEED;

    // reset at the start of movement handling
    player.targetMoveDir = {0.0f, 0.0f, 0.0f};

    Vector3 forward = {sinf(Player::yaw), 0.0f, cosf(Player::yaw)};
    Vector3 right = {cosf(Player::yaw), 0.0f, -sinf(Player::yaw)};

    forward = Vector3Normalize(forward);
    right = Vector3Normalize(right);

    player.animIndex = 0;
    if (IsKeyDown(KEY_W))
    {
        player.targetMoveDir = Vector3Subtract(player.targetMoveDir, forward);
        player.animIndex = 3;
    }
    if (IsKeyDown(KEY_A))
    {
        player.targetMoveDir = Vector3Subtract(player.targetMoveDir, right);
        player.animIndex = 3;
    }
    if (IsKeyDown(KEY_S))
    {
        player.targetMoveDir = Vector3Add(player.targetMoveDir, forward);
        player.animIndex = 3;
    }
    if (IsKeyDown(KEY_D))
    {
        player.targetMoveDir = Vector3Add(player.targetMoveDir, right);
        player.animIndex = 3;
    }

    if (IsKeyDown(KEY_LEFT_SHIFT))
        player.animIndex = 1;
    if (IsKeyDown(KEY_T))
        player.animIndex = 2;

    if (Vector3Length(player.targetMoveDir) > 0.001f)
    {
        player.targetMoveDir = Vector3Normalize(player.targetMoveDir);
    }
    Vector3 targetVel = Vector3Scale(player.targetMoveDir, player.speed);
    float t = player.moveLerpFactor * delta;

    player.moveVelocity.x += (targetVel.x - player.moveVelocity.x) * t;
    player.moveVelocity.y += (targetVel.y - player.moveVelocity.y) * t;
    player.moveVelocity.z += (targetVel.z - player.moveVelocity.z) * t;

    player.position = Vector3Add(player.position, Vector3Scale(player.moveVelocity, delta));
    if (Vector3Length(player.moveVelocity) > 0.001f)
    {
        player.modelYaw = atan2f(player.moveVelocity.x, player.moveVelocity.z) * RAD2DEG;
    }
}

void Player::UpdatePlayer()
{
    Player &player = Player::getInstance();
    float dt = GetFrameTime();
    auto rm = ResourceManager::getInstance();
    ModelAnim &anim = rm.getModelAnimation("player");

    player.animTime += dt;

    while (player.animTime >= player.animFPS)
    {
        player.animTime -= player.animFPS;
        player.animCurrentFrame = (player.animCurrentFrame + 1) % anim.modelAnim->frameCount;
    }

    UpdateModelAnimation(player.model, anim.modelAnim[player.animIndex], player.animCurrentFrame);
}

void Player::UpdateCamera()
{
    Player player = Player::getInstance();
    Vector3 playerPos = player.GetPlayerPosition();
    Vector2 mouseDelta = GetMouseDelta();
    float delta = GetFrameTime();

    playerPos = Vector3Add(playerPos, Vector3{0.0f, player.height, 0.0f});

    yaw -= mouseDelta.x * mouseSensitivity;
    pitch += mouseDelta.y * mouseSensitivity;

    pitch = Clamp(pitch, -PI / 2 + 0.1f, PI / 2 - 0.8f);

    float distance = 5.0f;
    Vector3 offset = {cosf(pitch) * sinf(yaw) * distance, sinf(pitch) * distance + 2.0f,
                      cosf(pitch) * cosf(yaw) * distance};

    Player::targetCamera.position = Vector3Add(playerPos, offset);
    Player::targetCamera.target = playerPos;

    float t = Player::cameraLerpFactor * delta;

    // Lerp position
    Player::camera.position.x += (Player::targetCamera.position.x - Player::camera.position.x) * t;
    Player::camera.position.y += (Player::targetCamera.position.y - Player::camera.position.y) * t;
    Player::camera.position.z += (Player::targetCamera.position.z - Player::camera.position.z) * t;

    // Lerp target
    Player::camera.target.x += (Player::targetCamera.target.x - Player::camera.target.x) * t;
    Player::camera.target.y += (Player::targetCamera.target.y - Player::camera.target.y) * t;
    Player::camera.target.z += (Player::targetCamera.target.z - Player::camera.target.z) * t;
}

void Player::setPlayerPosition(Vector3 pos)
{
    Player &player = Player::getInstance();
    player.position = pos;
}

void Player::DrawPlayer(SceneContext *sceneCtx)
{
    Player &player = Player::getInstance();

    rlPushMatrix();
    rlTranslatef(player.position.x, player.position.y, player.position.z);
    rlRotatef(player.modelYaw, 0.0f, 1.0f, 0.0f);
    rlRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    rlScalef(0.02f, 0.02f, 0.02f);
    for (int i = 0; i < player.model.materialCount; i++)
    {
        player.model.materials[i].shader = sceneCtx->terrainShader->getShader();
    }
    DrawModel(player.model, Vector3{0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
    rlPopMatrix();
}
