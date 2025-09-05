#include "game/player.hpp"
#include "game/resource.hpp"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <iostream>

Camera3D Player::camera;
float Player::mouseSensitivity;
float Player::yaw;
float Player::pitch;

void Player::InitPlayer()
{
    Player &player = Player::getInstance();
    player.position = Vector3{0, 0, 0};
    player.height = 4.0f;
    player.speed = 5.0f;
    player.model = ResourceManager::getInstance().getModel("player");
    Player::camera = {0};
    camera.position = {10.0f, 10.0f, 10.0f};
    camera.target = {0.0f, 0.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

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
    Vector3 forward = {sinf(Player::yaw), 0.0f, cosf(Player::yaw)};
    Vector3 right = {cosf(Player::yaw), 0.0f, -sinf(Player::yaw)};

    forward = Vector3Normalize(forward);
    right = Vector3Normalize(right);

    if (IsKeyDown(KEY_W))
        player.moveDir = Vector3Subtract(player.moveDir, forward);
    if (IsKeyDown(KEY_A))
        player.moveDir = Vector3Subtract(player.moveDir, right);
    if (IsKeyDown(KEY_S))
        player.moveDir = Vector3Add(player.moveDir, forward);
    if (IsKeyDown(KEY_D))
        player.moveDir = Vector3Add(player.moveDir, right);
    if (Vector3Length(player.moveDir) > 0.001f)
    {
        player.moveDir = Vector3Normalize(player.moveDir);
        player.position = Vector3Add(player.position, Vector3Scale(player.moveDir, player.speed * delta));
    }

    player.speed = (IsKeyDown(KEY_LEFT_SHIFT)) ? 15.0f : 5.0f;
}

void Player::UpdatePlayer()
{
    if (IsKeyDown(KEY_SPACE))
    {
        Player &player = Player::getInstance();
        auto rm = ResourceManager::getInstance();
        auto &anim = rm.getModelAnimation("player");
        player.animCurrentFrame = (player.animCurrentFrame + 1) % anim.frameCount;
        UpdateModelAnimation(player.model, anim, player.animCurrentFrame);
        std::cout << player.animCurrentFrame << "\n";
    }
}

void Player::UpdateCamera()
{
    Player player = Player::getInstance();
    Vector3 playerPos = player.GetPlayerPosition();
    Vector2 mouseDelta = GetMouseDelta();

    playerPos = Vector3Add(playerPos, Vector3{0.0f, player.height, 0.0f});

    yaw -= mouseDelta.x * mouseSensitivity;
    pitch += mouseDelta.y * mouseSensitivity;

    pitch = Clamp(pitch, -PI / 2 + 0.1f, PI / 2 - 0.1f);

    float distance = 5.0f;
    Vector3 offset = {cosf(pitch) * sinf(yaw) * distance, sinf(pitch) * distance + 2.0f,
                      cosf(pitch) * cosf(yaw) * distance};

    camera.target = playerPos;
    camera.position = Vector3Add(playerPos, offset);
    camera.up = {0.0f, 1.0f, 0.0f};

    Vector3 move = {0};
    Vector3 rot = {0.0f, 0.0f, 0.0f};
    UpdateCameraPro(&Player::camera, move, rot, 1.0f);
}

void Player::setPlayerPosition(Vector3 pos)
{
    Player &player = Player::getInstance();
    player.position = pos;
}

void Player::DrawPlayer()
{
    Player &player = Player::getInstance();
    if (Vector3Length(player.moveDir) > 0.001f)
    {
        player.modelYaw = atan2f(player.moveDir.x, player.moveDir.z) * RAD2DEG;
    }

    rlPushMatrix();
    rlTranslatef(player.position.x, player.position.y, player.position.z);
    rlRotatef(player.modelYaw, 0.0f, 1.0f, 0.0f);
    rlRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    rlScalef(0.02f, 0.02f, 0.02f);
    DrawModel(player.model, Vector3{0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
    rlPopMatrix();

    player.moveDir.x = 0;
    player.moveDir.y = 0;
    player.moveDir.z = 0;
}
