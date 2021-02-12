#version 430 core

in vec2 texCoordsExport;

uniform sampler2D sugaryTex;

out vec4 colorsOut;

void main(void)
{
   colorsOut = texture(sugaryTex, texCoordsExport);
}