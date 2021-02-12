#version 430 core

#define CYLINDER 0
#define DISC 1

in vec4 frontAmbDiffExport, frontSpecExport, backAmbDiffExport, backSpecExport;
in vec2 texCoordsExport;

uniform sampler2D canTopTex;
uniform uint object;
uniform uint renderHoles;

out vec4 colorsOut;

vec4 texColor;
vec4 redColor = vec4(1.0, 0.0, 0.0, 0.0);
vec4 greenColor = vec4(0.0, 1.0, 0.0, 0.0);

void main(void)
{  
   if (object == CYLINDER) 
   {   
      // Define the procedural texture.
   	  texColor = abs(fract(10.0 * texCoordsExport.s) - 0.5) * greenColor + 
		         abs(fract(10.0 * texCoordsExport.t) - 0.5)  * redColor; 

	  // Make the rectangular holes.
      if (renderHoles == 1) if ( ( int(floor(10.0 * texCoordsExport.s)) + 
		                           int(floor(10.0 * texCoordsExport.t)) ) % 2 == 1  ) discard; 
   }
   
   if (object == DISC) texColor = texture(canTopTex, texCoordsExport);

   colorsOut = gl_FrontFacing? (frontAmbDiffExport * texColor + frontSpecExport) : 
                               (backAmbDiffExport * texColor + backSpecExport);
}