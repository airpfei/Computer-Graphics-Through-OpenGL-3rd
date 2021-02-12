#version 430 core

#define PI 3.14159265

layout(quads, equal_spacing, ccw) in;

uniform mat4 modelViewMat;
uniform mat4 projMat;
uniform mat3 normalMat;
uniform float sphRadius;

out vec3 normalExport;

float u, v;
vec4 sphCoords;
vec3 sphNormal;

void main( )
{
   u = gl_TessCoord.x;
   v = gl_TessCoord.y;
   
   sphCoords = vec4(sphRadius * cos(u * PI - PI/2.0) * cos(2.0 * v * PI),
                    sphRadius * sin(u * PI - PI/2.0),
					sphRadius * cos(u * PI - PI/2.0) * sin(2.0 * v * PI),
					1.0);
   sphNormal = vec3(cos(u * PI - PI/2.0) * cos(2.0 * v * PI),
                    sin(u * PI - PI/2.0),
					cos(u * PI - PI/2.0) * sin(2.0 * v * PI));

   normalExport = normalize(normalMat * sphNormal);

   gl_Position = projMat * modelViewMat *  sphCoords;
}
