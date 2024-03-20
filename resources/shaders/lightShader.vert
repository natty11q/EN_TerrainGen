#version 330 core

layout (location = 0) in vec3 aPos;

layout (location = 1) in vec3 aNormal;
// telling the code that i have a vector containing colour values for each vertex
// in this case taking an rgba value
layout (location = 2) in vec3 aColour;

layout (location = 3) in vec2 aTextCoord;

// telling it to return that colour
out vec3 currentPos;

out vec3 normal;

out vec3 colour;

out vec2 TextCoord;


uniform mat4 model;
uniform mat4 camMatrix;



void main()
{

    currentPos = vec3(model * vec4(aPos , 1.0f));

    normal = aNormal;

    colour = aColour;
    
    TextCoord = aTextCoord;
    
    // outputs the position of all vetices

    // gl_Position = proj * view * model * vec4(aPos.x + aPos.x * scale, aPos.y + aPos.y * scale, aPos.z + aPos.z * scale, 1.0);
    gl_Position = camMatrix * model * vec4(currentPos , 1.0);
}