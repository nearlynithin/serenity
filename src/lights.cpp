#include "lights.hpp"
#include "raylib.h"
#include <iostream>

int Light::lightCounter = 0;

Light::Light(int type, Vector3 position, Vector3 target, Color color, Shader &lightShader)
{
    if (lightCounter >= MAX_LIGHTS)
    {
        TraceLog(LOG_ERROR, "Max number of lights exceeded");
    }

    id = lightCounter++;
    shader = &lightShader;
    enabled = 1;
    this->type = type;
    this->position = position;
    this->target = target;
    this->color = color;
    // attenuation = attenuation;

    enabledLoc = GetShaderLocation(*shader, TextFormat("lights[%i].enabled", id));
    typeLoc = GetShaderLocation(*shader, TextFormat("lights[%i].type", id));
    positionLoc = GetShaderLocation(*shader, TextFormat("lights[%i].position", id));
    targetLoc = GetShaderLocation(*shader, TextFormat("lights[%i].target", id));
    colorLoc = GetShaderLocation(*shader, TextFormat("lights[%i].color", id));
    // attenuationLoc = GetShaderLocation(*shader, TextFormat("lights[%i].attenuation", id));

    UpdateLightShader();
}

void Light::UpdateLightShader()
{
    // Send to shader light enabled state and type
    SetShaderValue(*shader, enabledLoc, &enabled, SHADER_UNIFORM_INT);
    SetShaderValue(*shader, typeLoc, &type, SHADER_UNIFORM_INT);

    // Send to shader light position values
    float lightPos[3] = {position.x, position.y, position.z};
    SetShaderValue(*shader, positionLoc, &lightPos, SHADER_UNIFORM_VEC3);

    // Send to shader light target position values
    float lightTarget[3] = {target.x, target.y, target.z};
    SetShaderValue(*shader, targetLoc, lightTarget, SHADER_UNIFORM_VEC3);

    // Send to shader light color values
    float lightColor[4] = {(float)color.r / (float)255, (float)color.g / (float)255, (float)color.b / (float)255,
                           (float)color.a / (float)255};
    SetShaderValue(*shader, colorLoc, lightColor, SHADER_UNIFORM_VEC4);

    // Send to shader Light attenuation value
    // SetShaderValue(*shader, attenuationLoc, &attenuation, SHADER_UNIFORM_FLOAT);
}

bool Light::isEnabled()
{
    return (enabled == 1) ? true : false;
}

Vector3 Light::getPosition()
{
    return position;
}

Color Light::getColor()
{
    return color;
}

void Light::toggle()
{
    enabled = !enabled;
}