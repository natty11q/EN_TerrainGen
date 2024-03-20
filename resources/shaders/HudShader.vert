#version 330 core

// only need xy for this as the hud is fixed to the screen
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;




out vec2 CurrentPosition;
out vec2 TextureCoordinate;
out float AR;

void main()
{
    CurrentPosition = aPos;
    TextureCoordinate = aTexCoord;
    gl_Position = vec4(CurrentPosition ,0.0f, 1.0f);
}