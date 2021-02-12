#version 430 core

#define LAUNCH 0

layout(location=0) in vec4 launchCoords;
layout(location=1) in vec2 launchTexCoords;

uniform mat4 modelViewMat;
uniform mat4 projMat;

out vec2 texCoordsExport;

vec4 coords;

void main(void)
{   
   coords = launchCoords;
   texCoordsExport = launchTexCoords;

   gl_Position = projMat * modelViewMat * coords;
}
