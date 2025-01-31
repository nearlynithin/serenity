#include "perlin.hpp"

int height = 100;
int width = 100;

Vector2 randomGradient(int ix, int iy)
{
    // Use consistent hashing for reproducibility
    unsigned int hash = ix * 1619 + iy * 31337;
    hash = hash * hash * hash * 60493;
    hash = (hash >> 13) ^ hash;

    float angle = (hash & 0xFFFFFFF) / static_cast<float>(0xFFFFFFF) * 2.0f * PI;
    return Vector2{cosf(angle), sinf(angle)};
}

float noise(float x, float y)
{
    // Scale input coordinates to match the desired tile size
    x = fmodf(x, static_cast<float>(width));
    y = fmodf(y, static_cast<float>(height));
    if (x < 0)
        x += width;
    if (y < 0)
        y += height;

    int x0 = static_cast<int>(floorf(x));
    int y0 = static_cast<int>(floorf(y));
    int x1 = (x0 + 1) % width;
    int y1 = (y0 + 1) % height;

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

// Computes the dot product of the distance and gradient vectors
float dotGridGradient(int ix, int iy, float x, float y)
{
    // Wrap coordinates
    ix = ix % width;
    iy = iy % height;
    if (ix < 0)
        ix += width;
    if (iy < 0)
        iy += height;

    Vector2 gradient = randomGradient(ix, iy);
    float dx = x - static_cast<float>(ix);
    float dy = y - static_cast<float>(iy);

    return (dx * gradient.x + dy * gradient.y);
}

// Improved smoothstep function for better interpolation
float smoothstep(float t)
{
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float interpolate(float a0, float a1, float w)
{
    return a0 + smoothstep(w) * (a1 - a0);
}

float perlin(float worldX, float worldZ, float scale)
{
    // Convert world coordinates to noise coordinates
    float noiseX = worldX * scale;
    float noiseY = worldZ * scale;
    return noise(noiseX, noiseY);
}