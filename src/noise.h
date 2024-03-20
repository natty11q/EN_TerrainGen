#pragma once

#include <vector>
#include <math.h>

#include "header_files/Utility.hpp"


class Noise
{
public:
    Noise(int MapWidth, int MapHeight, float scale_in = 400,float lacunarity_in = 2, float persistance_in = 0.5,int oct_in =12,float contrast_in = 1);
    // void generateNoiseMap();
    static void generateNoiseMap(std::vector<std::vector<float>>* NoiseMapint, std::vector<float>* NoiseMap1D ,int MapWidth, int MapHeight,float scale_in ,float lacunarity_in , float persistance_in ,int oct_in ,float contrast_in );

    std::vector<std::vector<float>> NoiseMap;
    std::vector<float> NoiseMap1D;

private:

    int Width;
    int Height;
    float scale;
    float lacunarity;
    float persistance;
    int octaves;
    float contrast;
};