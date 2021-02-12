#version 430 core

#define LAUNCH 0

in vec2 texCoordsExport;

uniform sampler2D launchTex;
uniform uint isBlurred;

out vec4 colorsOut;

vec4 launchTexColor;

void main(void)
{  
   // Not blurred.
   if (isBlurred == 0) launchTexColor = texture(launchTex, texCoordsExport);

   // Camera blur implemented with a weighted sum of 4 pixel color values starting with the original pixel and moving right.
   else launchTexColor = 0.4 * texture(launchTex, texCoordsExport) + 
	                     0.3 * texture(launchTex, texCoordsExport + vec2(0.005, 0.0)) +
                         0.2 * texture(launchTex, texCoordsExport + vec2(0.01, 0.0)) + 
						 0.1 * texture(launchTex, texCoordsExport + vec2(0.015, 0.0));
   
   colorsOut = launchTexColor;
}