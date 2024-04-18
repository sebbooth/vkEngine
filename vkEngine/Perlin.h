#pragma once
#ifndef PERLIN_H
#define PERLIN_H

#include <math.h>

typedef struct {
    float x, y;
} vector2;

class Perlin
{
public:
    Perlin() = default;
    float perlin2D(int x, int y);

public:
    float init_freq = 1;
    float init_amp = 1;
    int grid_size = 400;
    int layers = 4;

private:
    float perlin2DPoint(int xi, int yi);
    vector2 randomGradient(int ix, int iy);
    float dotGridGradient(int ix, int iy, float x, float y);
    float interpolate(float a0, float a1, float w);

private:
    float freq = init_freq; 
    float amp = init_amp;
};

#endif