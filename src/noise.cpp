#include "noise.h"
#include "header_files/Utility.hpp"
#include <vector>
#include <future>



Noise::Noise(int MapWidth, int MapHeight,float scale_in ,float lacunarity_in , float persistance_in ,int oct_in ,float contrast_in )
{
    this->Width  = MapWidth;
    this->Height = MapHeight;
    if (scale_in == 0){scale_in = 0.0001f;}
    this->scale  = scale_in;
    this->lacunarity  = lacunarity_in;
    this->persistance = persistance_in;
    this->octaves = oct_in;
    this->contrast = contrast_in;

    std::future<void> futureNoisemap;

    futureNoisemap = std::async(std::launch::async, this->generateNoiseMap, &this->NoiseMap, &this->NoiseMap1D, this->Width, this->Height, this->scale, this->lacunarity, this->persistance,this->octaves, this->contrast);
}


// void Noise::generateNoiseMap()
// {   
//     int total = Height * Width;



//     for (int x = 0 ; x < Height + 1; ++x)
//     {
//         std::vector<float> currentY;
//         for (int y = 0 ; y < Width + 1; ++y)
//         {
//             // int index = (y * this->Width + x);

//             float val = 0;

//             float freq = 1;
//             float amp = 1;

//             for (int i  = 0 ; i < octaves; ++i)
//             {
//                 val  += smoothNoise2D((x * freq / scale), (y * freq / scale)) * amp;
//                 freq *= lacunarity;
//                 amp  *= persistance;
//             }

//             val *= contrast;

//             if (val > 1.0f) val = 1.0f;
//             else if (val < -1.0f) val = -1.0f;
//             currentY.push_back(val);
//             NoiseMap1D.push_back(val);
//         }
//         NoiseMap.push_back(currentY);
//     }
// }

void Noise::generateNoiseMap(std::vector<std::vector<float>>* NoiseMap, std::vector<float>* NoiseMap1D , int Width, int Height,float scale ,float lacunarity , float persistance ,int octaves ,float contrast )
{   
    int total = Height * Width;



    for (int x = 0 ; x < Height + 1; ++x)
    {
        std::vector<float> currentY;
        for (int y = 0 ; y < Width + 1; ++y)
        {
            // int index = (y * this->Width + x);

            float val = 0;

            float freq = 1;
            float amp = 1;

            for (int i  = 0 ; i < octaves; ++i)
            {
                val  += smoothNoise2D((x * freq / scale), (y * freq / scale)) * amp;
                freq *= lacunarity;
                amp  *= persistance;
            }

            val *= contrast;

            if (val > 1.0f) val = 1.0f;
            else if (val < -1.0f) val = -1.0f;
            currentY.push_back(val);
            NoiseMap1D->push_back(val);
        }
        NoiseMap->push_back(currentY);
    }
}