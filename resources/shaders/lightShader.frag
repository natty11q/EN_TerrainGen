#version 330 core
out vec4 FragColor;


in vec4 colour; // take the input of the rgba colour given in the vertex shader

in vec2 TextCoord;


uniform sampler2D text0;

uniform vec4 lightColour;

void main()
{
    // FragColor = vec4(colour);
    FragColor = lightColour;
    // FragColor = texture(text0 , TextCoord );
}