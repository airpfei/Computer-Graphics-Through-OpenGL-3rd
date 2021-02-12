#version 430 core

#define SPHERE 0
#define TORUS 1

uniform uint object;
uniform vec4 sphColor, torColor;

out vec4 colorsOut;

void main(void)
{
   if (object == SPHERE) colorsOut = sphColor; 
   if (object == TORUS) colorsOut = torColor;
}