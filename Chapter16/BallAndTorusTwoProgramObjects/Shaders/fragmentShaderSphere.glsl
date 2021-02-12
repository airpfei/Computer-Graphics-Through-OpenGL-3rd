#version 430 core

uniform vec4 sphColor;

out vec4 colorsOut;

void main(void)
{
   colorsOut = sphColor; 
}