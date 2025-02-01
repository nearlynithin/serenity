#ifndef LIGHTS_H
#define LIGHTS_H

#define MAX_LIGHTS 10

#include "raylib.h"
#include <string>
#include <unordered_map>

typedef enum
{
    LIGHT_DIRECTIONAL = 0,
    LIGHT_POINT,
    LIGHT_SPOT
} LightType;

typedef struct
{
    int id;
    int type;
    int enabled;
    Vector3 position;
    Vector3 target;
    Color color;
    float attenuation;

    // Shader light parameter locations
    int typeLoc;
    int enabledLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;
    int attenuationLoc;
} Light;

// Creates a light and returns the id of the light
int CreateLight(const std::string &lightName, int type, Vector3 position, Vector3 target, Color color,
                float attenuation, Shader shader);
void UpdateLightValues(Shader shader, Light light);
void UpdateLights(Shader shader);
void UpdateLight(Shader shader, const std::string &lightName);

#endif