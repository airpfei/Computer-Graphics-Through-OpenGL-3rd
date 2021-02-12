#version 430 core

layout(location=0) in vec4 planeCoords;
layout(location=1) in vec2 planeTexCoords;

uniform mat4 modelViewMat;
uniform mat4 projMat;

out vec2 texCoordsExport;

void main(void)
{   
   texCoordsExport = planeTexCoords;

   gl_Position = projMat * modelViewMat * planeCoords;
}
