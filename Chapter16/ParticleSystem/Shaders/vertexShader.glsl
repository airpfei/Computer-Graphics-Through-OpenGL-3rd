#version 430 core

#define PARTICLES_A 0
#define PARTICLES_B 1
#define SQUARE 2

layout(location=0) in vec4 particleCoords;
layout(location=1) in vec3 particleVel;
layout(location=2) in vec4 squareCoords;

uniform mat4 projMat;
uniform mat4 modelViewMat;
uniform uint object;

out vec4 coords;
out vec3 vel;

void main(void)
{
   if (object == PARTICLES_A || object == PARTICLES_B)
   {
      coords = particleCoords;
      vel = particleVel;
   }
   if (object == SQUARE) gl_Position = projMat * modelViewMat * squareCoords;
}
