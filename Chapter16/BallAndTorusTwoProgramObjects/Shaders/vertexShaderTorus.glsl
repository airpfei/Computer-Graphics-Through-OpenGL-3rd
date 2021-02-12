#version 430 core

layout(location=1) in vec4 torCoords;

uniform mat4 projMat;
uniform mat4 modelViewMat;

vec4 coords;

void main(void)
{
   coords = torCoords;
   
   gl_Position = projMat * modelViewMat * coords;
}
