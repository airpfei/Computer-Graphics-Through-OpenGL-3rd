#version 430 core

#define CYLINDER 0
#define DISC 1
#define RECTANGLE 2

in vec3 normalExport;
in vec2 texCoordsExport;
in vec3 eyeDirectionExport;

uniform sampler2D canLabelTex;
uniform sampler2D canTopTex;
uniform sampler2D canLabelSpecularMapTex;
uniform sampler2D specMapOnTex;
uniform sampler2D specMapOffTex;
uniform uint object;
uniform uint isSpecularMapped;

out vec4 colorsOut;

struct Light
{
   vec4 ambCols;
   vec4 difCols;
   vec4 specCols;
   vec4 coords;
};
uniform Light light0;

uniform vec4 globAmb;
  
struct Material
{
   vec4 ambRefl;
   vec4 difRefl;
   vec4 specRefl;
   vec4 emitCols;
   float shininess;
};
uniform Material canFandB;

vec3 normal, lightDirection, eyeDirection, halfway;
vec4 frontEmit, frontGlobAmb, frontAmb, frontDif, frontSpec, frontAmbDiff,
     backEmit, backGlobAmb, backAmb, backDif, backSpec, backAmbDiff;
vec4 texColor;

void main(void)
{  
   if (object == CYLINDER) texColor = texture(canLabelTex, texCoordsExport);
   if (object == DISC) texColor = texture(canTopTex, texCoordsExport);
   if (object == RECTANGLE)  
      if (isSpecularMapped == 1) texColor = texture(specMapOnTex, texCoordsExport); else texColor = texture(specMapOffTex, texCoordsExport);

   normal = normalize(normalExport);
   lightDirection = normalize(vec3(light0.coords));
   eyeDirection = eyeDirectionExport;
   halfway = (length(lightDirection + eyeDirection) == 0.0f) ? 
		        vec3(0.0) : (lightDirection + eyeDirection)/length(lightDirection + eyeDirection);
  
   // Implementing OpenGL's lighting equation.
   frontEmit = canFandB.emitCols;
   frontGlobAmb = globAmb * canFandB.ambRefl;
   frontAmb = light0.ambCols * canFandB.ambRefl;
   frontDif = max(dot(normal, lightDirection), 0.0f) * (light0.difCols * canFandB.difRefl);    
   frontSpec = pow(max(dot(normal, halfway), 0.0f), canFandB.shininess) * (light0.specCols * canFandB.specRefl);
   frontAmbDiff =  vec4(vec3(min(frontEmit + frontGlobAmb + frontAmb + frontDif, vec4(1.0))), 1.0);  
   frontSpec =  vec4(vec3(min(frontSpec, vec4(1.0))), 1.0);  
   
   // Implementing OpenGL's lighting equation, normals reversed.
   normal = -1.0f * normal;
   backEmit = canFandB.emitCols;
   backGlobAmb = globAmb * canFandB.ambRefl;    
   backAmb = light0.ambCols * canFandB.ambRefl;
   backDif = max(dot(normal, lightDirection), 0.0f) * (light0.difCols * canFandB.difRefl);    
   backSpec = pow(max(dot(normal, halfway), 0.0f), canFandB.shininess) * (light0.specCols * canFandB.specRefl);
   backAmbDiff =  vec4(vec3(min(backEmit + backGlobAmb + backAmb + backDif, vec4(1.0))), 1.0);  
   backSpec =  vec4(vec3(min(backSpec, vec4(1.0))), 1.0);

   // No highlights applied outside specular mapped region.
   if (isSpecularMapped == 1 && object == CYLINDER && texture(canLabelSpecularMapTex, texCoordsExport).r < 1.0)
      colorsOut = gl_FrontFacing? (frontAmbDiff * texColor) : (backAmbDiff * texColor);

   // Highlights applied to specular mapped region.
   else if (object != RECTANGLE)
      colorsOut = gl_FrontFacing? (frontAmbDiff * texColor + frontSpec) : (backAmbDiff * texColor + backSpec);
   
   else
	  colorsOut = texColor;
}