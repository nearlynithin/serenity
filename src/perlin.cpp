#include "perlin.hpp"

#define UNITS 1
#define WIDTH 200
#define HEIGHT 200

Vector2 randomGradient(int ix, int iy)
{
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = ix, b = iy;

    a *= 3284157443;
    b ^= a << s | a >> (w - s);
    b *= 1911520717;
    a ^= b << s | b >> (w - s);
    a *= 2048419325;

    // Convert to angle in [0, 2*PI]
    float angle = a * (PI / ~(~0u >> 1));
    return Vector2{cosf(angle), sinf(angle)};
}

float dotGridGradient(int ix, int iy, float x, float y)
{
    Vector2 gradient = randomGradient(ix, iy);

    float dx = x - static_cast<float>(ix);
    float dy = y - static_cast<float>(iy);

    return (dx * gradient.x + dy * gradient.y);
}

float smoothStep(float t)
{
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float interpolate(float a0, float a1, float w)
{
    float weight = smoothStep(w);

    return a0 + weight * (a1 - a0);
}

float singlePerlin(float x, float y)
{
    int x0 = static_cast<int>(floorf(x));
    int y0 = static_cast<int>(floorf(y));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float sx = x - static_cast<float>(x0);
    float sy = y - static_cast<float>(y0);

    float n0 = dotGridGradient(x0, y0, x, y);
    float n1 = dotGridGradient(x1, y0, x, y);
    float n2 = dotGridGradient(x0, y1, x, y);
    float n3 = dotGridGradient(x1, y1, x, y);

    float ix0 = interpolate(n0, n1, sx);
    float ix1 = interpolate(n2, n3, sx);

    return interpolate(ix0, ix1, sy);
}

float perlin(float x, float y, float scale, int octaves, float persistence, float lacunarity)
{
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; i++)
    {
        float nx = x * frequency / scale;
        float ny = y * frequency / scale;

        float noiseValue = singlePerlin(nx, ny);

        // Post-process noise value for smoother mountains
        // Apply subtle curve to create more rounded terrain features
        if (i < 2)
        { // Only apply to lower octaves (larger features)
            // This smooths the large features while preserving some detail
            noiseValue = (noiseValue > 0) ? powf(noiseValue, 1.2f) : -powf(-noiseValue, 1.2f);
        }

        total += noiseValue * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= lacunarity;
    }

    float result = total / maxValue;

    result = (result > 0) ? powf(result, 1.1f) : -powf(-result, 1.1f);

    return result;
}

float terrainNoise(float x, float y, float scale)
{
    float mountains = perlin(x, y, scale * 2.0f, 2, 0.5f, 2.0f);

    mountains = (mountains > 0) ? powf(mountains, 1.4f) : -powf(-mountains, 1.4f);
    float hills = perlin(x + 100.0f, y + 100.0f, scale, 3, 0.4f, 2.0f) * 0.25f;

    float details = perlin(x - 200.0f, y - 200.0f, scale * 0.5f, 2, 0.3f, 2.0f) * 0.1f;
    return mountains + hills + details;
}