#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <cerrno>
#include <filesystem>
#include <ostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <random>
#include <chrono>

#include <glad/glad.h>

// sometimes in life you need to know what path youre on 
std::string get_working_path();
std::string get_file_contents(const char* filename);
/* Beloved Smoothstep, like sine but faster and is purty > :))  */
float smoothstep(float x, float edge0 = 0.0f, float edge1 = 1.0f ); 
float clamp(float x, float lowerlimit = 0.0f, float upperlimit = 1.0f );

float lerp(float t, float a, float b ); // linear interp

float simpleNoise1D( int x );
float simpleNoise2D( int x , int y);

float lerpNoise1D( float x );
float lerpNoise2D( float x , float y);

float smoothNoise1D( float x );
float smoothNoise2D( float x , float y);


int NoElementsArrInt(int* arr);

float NoElementsArrFloar(float* arr);

template <typename T>
T findLargest(const T* values, size_t size);


std::vector<std::string> GetFolderContents(const std::string& path);
int GetRandomIntieger(int R1 , int R2);
float GetRandomfloat(float R1 , float R2);

// int getMaxArrInt(int* arr);
// int getMaxArrGLuint(GLuint* arr);

// int getMinArrInt(int* arr);

// float getMaxArrFloat(int* arr);

// float getMinArrFloat(int* arr);

#endif