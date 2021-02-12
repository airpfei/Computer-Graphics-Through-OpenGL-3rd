#version 430 core

#define CYLINDER 0
#define DISC 1
#define RECTANGLE 2

layout(location=0) in vec4 cylCoords;
layout(location=1) in vec3 cylNormal;
layout(location=2) in vec2 cylTexCoords;
layout(location=3) in vec4 discCoords;
layout(location=4) in vec3 discNormal;
layout(location=5) in vec2 discTexCoords;
layout(location=6) in vec4 rectangleCoords;
layout(location=7) in vec3 rectangleNormal;
layout(location=8) in vec2 rectangleTexCoords;

uniform mat4 modelViewMat;
uniform mat4 projMat;
uniform mat3 normalMat;
uniform uint object;

out vec3 normalExport;
out vec2 texCoordsExport;
out vec3 eyeDirectionExport;

vec2 texCoords;
vec3 normal, lightDirection, eyeDirection, halfway;
vec4 frontEmit, frontGlobAmb, frontAmb, frontDif, frontSpec,
     backEmit, backGlobAmb, backAmb, backDif, backSpec;
vec4 coords;

void main(void)
{
   if (object == CYLINDER)
   {
      coords = cylCoords;
      normal = cylNormal;
	  texCoords = cylTexCoords;
   }
   if (object == DISC)
   {
      coords = discCoords;
      normal = discNormal;
	  texCoords = discTexCoords;
   }

   if (object == RECTANGLE)
   {
      coords = rectangleCoords;
      normal = rectangleNormal;
	  texCoords = rectangleTexCoords;
   }

   normalExport = normalMat * normal;
   texCoordsExport = texCoords;
   eyeDirectionExport = -1.0f * normalize(vec3(modelViewMat * coords));

   gl_Position = projMat * modelViewMat * coords;
}