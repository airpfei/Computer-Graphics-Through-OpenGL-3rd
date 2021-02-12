#version 430 core

#define PARTICLES_A 0
#define PARTICLES_B 1
#define SQUARE 2

uniform uint object;

out vec4 colorsOut;

void main(void)
{
   if (object == PARTICLES_A || object == PARTICLES_B) colorsOut = vec4(0.0, 0.0, 1.0, 1.0);
   if (object == SQUARE) colorsOut = vec4(1.0, 0.0, 0.0, 1.0);
}