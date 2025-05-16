#include "game/player.hpp"
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
    player.speed = 15.0f;
    Mesh mesh = GenMeshCylinder(1, player.height, 8);
    player.model = LoadModelFromMesh(mesh);
    Player::camera = {
        .position = {10.0f, 10.0f, 10.0f},
        .target = {0.0f, 0.0f, 0.0f},
        .up = {0.0f, 1.0f, 0.0f},
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE,
    };
    Player::mouseSensitivity = 0.003f;
    Player::yaw = 0.0f;
    Player::pitch = 0.0f;
}
void Player::DrawPlayer()
{
    Player &player = Player::getInstance();
    DrawModel(player.model, player.position, 0.4f, RED);
}

Vector3 Player::GetPlayerPosition()
{
    return Player::getInstance().position;
}

void Player::PlayerMoves()
{

    Player &player = Player::getInstance();

    Vector3 pos = player.GetPlayerPosition();

    float delta = GetFrameTime();
    Vector3 forward = {sinf(Player::yaw), 0.0f, cosf(Player::yaw)};

    Vector3 right = {cosf(Player::yaw), 0.0f, -sinf(Player::yaw)};

    forward = Vector3Normalize(forward);
    right = Vector3Normalize(right);

    if (IsKeyDown(KEY_W))
        player.position = Vector3Subtract(player.position, Vector3Scale(forward, player.speed * delta));
    if (IsKeyDown(KEY_A))
        player.position = Vector3Subtract(player.position, Vector3Scale(right, player.speed * delta));
    if (IsKeyDown(KEY_S))
        player.position = Vector3Add(player.position, Vector3Scale(forward, player.speed * delta));
    if (IsKeyDown(KEY_D))
        player.position = Vector3Add(player.position, Vector3Scale(right, player.speed * delta));
}

Vector2 Player::getPlayerCords()
{
    Vector3 pos = Player::getInstance().GetPlayerPosition();
    float gridX = static_cast<int>(floorf((pos.x + 250.0f) / 50.0f) - 5);
    float gridZ = static_cast<int>(floorf((pos.z + 250.0f) / 50.0f) - 5);
    return Vector2{gridX, gridZ};
}

void Player::UpdateCamera()
{
    Vector3 playerPos = Player::getInstance().GetPlayerPosition();
    Vector2 mouseDelta = GetMouseDelta();

    yaw -= mouseDelta.x * mouseSensitivity;
    pitch += mouseDelta.y * mouseSensitivity;

    pitch = Clamp(pitch, -PI / 2 + 0.1f, PI / 2 - 0.1f);

    float distance = 10.0f;
    Vector3 offset = {cosf(pitch) * sinf(yaw) * distance, sinf(pitch) * distance + 2.0f,
                      cosf(pitch) * cosf(yaw) * distance};

    camera.target = Vector3Add(playerPos, Vector3{0.0f, 0.75f, 0.0f});
    camera.position = Vector3Add(playerPos, offset);
    camera.up = {0.0f, 1.0f, 0.0f};

    Vector3 move = {0};
    Vector3 rot = {0.0f, 0.0f, 0.0f};
    UpdateCameraPro(&Player::camera, move, rot, -10.0f);
}