#version 430 core

layout(location=0) in vec4 squareCoords;
layout(location=1) in vec4 squareColors;

uniform mat4 modelViewMat;
uniform mat4 projMat;

out vec4 colorsExport;

void main(void)
{
   gl_Position = projMat * modelViewMat * squareCoords;
   colorsExport = squareColors;
}