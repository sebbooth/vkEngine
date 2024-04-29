#include "Perlin.h"

float Perlin::perlin2D(int xi, int yi)
{
    freq = init_freq;
    amp = init_amp;
   

    float perlinValue = 0;

    for (int i = 0; i < layers; i++) {
        perlinValue += perlin2DPoint(xi, yi);
        freq *= 2;
        amp /= 2;
    }

    perlinValue *= 1.2;
    /*
    // Clipping
    if (perlinValue > 1.0f)
        perlinValue = 1.0f;
    else if (perlinValue < -1.0f)
        perlinValue = -1.0f;
    */
    return (perlinValue + 1.0f) / 2.0f;
}

float Perlin::perlin2DPoint(int xi, int yi)
{
    float x = xi * freq / grid_size;
    float y = yi * freq / grid_size;

    // Determine grid cell corner coordinates
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Compute Interpolation weights
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // Compute and interpolate top two corners
    float n0 = dotGridGradient(x0, y0, x, y);
    float n1 = dotGridGradient(x1, y0, x, y);
    float ix0 = interpolate(n0, n1, sx);

    // Compute and interpolate bottom two corners
    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    float ix1 = interpolate(n0, n1, sx);

    // Final step: interpolate between the two previously interpolated values, now in y
    float value = interpolate(ix0, ix1, sy);

    return value * amp;
}

vector2 Perlin::randomGradient(int ix, int iy)
{
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = ix, b = iy;
    a *= 3284157443;

    b ^= a << s | a >> w - s;
    b *= 1911520717;

    a ^= b << s | b >> w - s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

    // Create the vector from the angle
    vector2 v;
    v.x = sin(random);
    v.y = cos(random);

    return v;
}

float Perlin::dotGridGradient(int ix, int iy, float x, float y)
{
    // Get gradient from integer coordinates
    vector2 gradient = randomGradient(ix, iy);

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return (dx * gradient.x + dy * gradient.y);
}

float Perlin::interpolate(float a0, float a1, float w)
{
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}



