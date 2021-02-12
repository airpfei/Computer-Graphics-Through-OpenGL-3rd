#version 430 core

layout(location=0) in vec4 sphCoords;

uniform mat4 projMat;
uniform mat4 modelViewMat;

vec4 coords;

void main(void)
{
   coords = sphCoords;
   
   gl_Position = projMat * modelViewMat * coords;
}
