///////////////////////////////////////////////////////////////////////////////////////////////          
// normalMapping.cpp
//
// This program modifies bumpMappingPerPixelLight.cpp to store the calculated normals in normal
// map texture before applying to the plane.
//
// Interaction:
// Press space to toggle between applying the normal map and showing the normal map texture.
//
// Sumanta Guha
/////////////////////////////////////////////////////////////////////////////////////////////// 

#include <cmath>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "prepShader.h"
#include "plane.h"
#include "light.h"
#include "material.h"

using namespace glm;

static enum object {PLANE}; // VAO ids.
static enum buffer {PLANE_VERTICES, PLANE_INDICES}; // VBO ids.

// Globals.
// Light properties.
static const Light light0 = 
{
	vec4(0.0, 0.0, 0.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 1.0, 0.0, 0.0 )
};

// Global ambient.
static const vec4 globAmb = vec4(0.2, 0.2, 0.2, 1.0);

// Front and back material properties.
static const Material planeFandB = 
{
	vec4(0.0, 0.5, 0.5, 1.0),
	vec4(0.0, 0.5, 0.5, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 0.0, 0.0, 1.0),
	50.0
};

// Plane data.
static Vertex planeVertices[(PLANE_LONGS + 1) * (PLANE_LATS + 1)]; 
static unsigned int planeIndices[PLANE_LATS][2*(PLANE_LONGS+1)]; 
static int planeCounts[PLANE_LATS]; 
static void* planeOffsets[PLANE_LATS]; 

static mat4 modelViewMat, projMat;
static mat3 normalMat;

static unsigned int
   programId,
   vertexShaderId,
   fragmentShaderId,
   modelViewMatLoc,
   projMatLoc,
   normalMatLoc,
   normalMapTexLoc,
   showNormalMappedPlaneLoc,
   buffer[2], 
   vao[1],
   texture;

static uint showNormalMappedPlane = 1; // Show the normal mapped plane?
static float normalMap[2*PLANE_EXTENT][2*PLANE_EXTENT][4]; // Storage for normal map texture.

// Routine to make normal map texture.
void createNormalMap(void)
{
   int i, j, k;
   float u, v;

   k = 0;
   for (j = 0; j <= PLANE_LATS; j++)
      for (i = 0; i <= PLANE_LONGS; i++)
      {
		 u = (-1 + 2*(float)j/PLANE_LATS) * PLANE_EXTENT;
		 v = (-1 + 2*(float)i/PLANE_LONGS) * PLANE_EXTENT;
		 
		 normalMap[i][j][0] = (-cos(2.0*u) + 1.0)/2.0;
		 normalMap[i][j][1] = 0.75;
         normalMap[i][j][2] = 0.5;
         normalMap[i][j][3] = 1.0;
		 k++;
	  }
}

// Initialization routine.
void setup(void) 
{
   glClearColor(1.0, 1.0, 1.0, 0.0); 
   glEnable(GL_DEPTH_TEST);

   // Create shader program executable.
   vertexShaderId = setShader("vertex", "Shaders/vertexShader.glsl");
   fragmentShaderId = setShader("fragment", "Shaders/fragmentShader.glsl");
   programId = glCreateProgram(); 
   glAttachShader(programId, vertexShaderId); 
   glAttachShader(programId, fragmentShaderId); 
   glLinkProgram(programId); 
   glUseProgram(programId);  

   // Initialize plane.
   fillPlane(planeVertices, planeIndices, planeCounts, planeOffsets);

   // Create VAOs and VBOs...
   glGenVertexArrays(1, vao);
   glGenBuffers(2, buffer); 

   // ...and associate plane data with vertex shader.
   glBindVertexArray(vao[PLANE]);
   glBindBuffer(GL_ARRAY_BUFFER, buffer[PLANE_VERTICES]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[PLANE_INDICES]);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
   glEnableVertexAttribArray(1);

   // Obtain projection matrix uniform location and set value.
   projMatLoc = glGetUniformLocation(programId,"projMat"); 
   projMat = frustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0); 
   glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

   // Obtain modelview matrix and normal matrix uniform locations.
   modelViewMatLoc = glGetUniformLocation(programId,"modelViewMat"); 
   normalMatLoc = glGetUniformLocation(programId,"normalMat"); 

   // Obtain isBumpMapped uniform location and set value.
   showNormalMappedPlaneLoc = glGetUniformLocation(programId, "showNormalMappedPlane");
   glUniform1uiv(showNormalMappedPlaneLoc, 1, &showNormalMappedPlane);

   // Obtain light property uniform locations and set values.
   glUniform4fv(glGetUniformLocation(programId, "light0.ambCols"), 1, &light0.ambCols[0]);
   glUniform4fv(glGetUniformLocation(programId, "light0.difCols"), 1, &light0.difCols[0]);
   glUniform4fv(glGetUniformLocation(programId, "light0.specCols"), 1, &light0.specCols[0]);
   glUniform4fv(glGetUniformLocation(programId, "light0.coords"), 1, &light0.coords[0]);

   // Obtain global ambient uniform location and set value.
   glUniform4fv(glGetUniformLocation(programId, "globAmb"), 1, &globAmb[0]);

   // Obtain material property uniform locations and set values.
   glUniform4fv(glGetUniformLocation(programId, "planeFandB.ambRefl"), 1, &planeFandB.ambRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "planeFandB.difRefl"), 1, &planeFandB.difRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "planeFandB.specRefl"), 1, &planeFandB.specRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "planeFandB.emitCols"), 1, &planeFandB.emitCols[0]);
   glUniform1f(glGetUniformLocation(programId, "planeFandB.shininess"), planeFandB.shininess);

   // Create texture ids.
   glGenTextures(1, &texture);
   
   // Generate normal texture.
   createNormalMap();

   // Bind normal map.
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2*PLANE_EXTENT, 2*PLANE_EXTENT, 0, GL_RGBA, GL_FLOAT, normalMap);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   normalMapTexLoc = glGetUniformLocation(programId, "normalMapTex");
   glUniform1i(normalMapTexLoc, 0);
}

// Drawing routine.
void drawScene(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Calculate and update modelview matrix.
   modelViewMat = mat4(1.0);
   modelViewMat = lookAt(vec3(0.0, 5.0, 30.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));

   // Calculate and update normal matrix.
   normalMat = transpose(inverse(mat3(modelViewMat)));
   glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, value_ptr(normalMat));
   
   // Draw plane.
   glMultiDrawElements(GL_TRIANGLE_STRIP, planeCounts, GL_UNSIGNED_INT, (const void **)planeOffsets, PLANE_LATS);
  
   glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
   glViewport(0, 0, w, h); 
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
   switch(key) 
   {
      case 27:
         exit(0);
         break;
      case ' ':
		 showNormalMappedPlane = showNormalMappedPlane? 0 : 1;
		 glUniform1uiv(showNormalMappedPlaneLoc, 1, &showNormalMappedPlane);
         glutPostRedisplay();
		 break;
      default:
         break;
   }
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
   std::cout << "Interaction:" << std::endl;
   std::cout << "Press space to toggle between applying the normal map and showing the normal map texture." << std::endl;
}

// Main routine.
int main(int argc, char **argv) 
{
   printInteraction();
   glutInit(&argc, argv);

   glutInitContextVersion(4, 3);
   glutInitContextProfile(GLUT_CORE_PROFILE);
   glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); 
   glutInitWindowSize(500, 500);
   glutInitWindowPosition(100, 100); 
   glutCreateWindow("normalMapping.cpp");
   glutDisplayFunc(drawScene); 
   glutReshapeFunc(resize);  
   glutKeyboardFunc(keyInput);

   glewExperimental = GL_TRUE;
   glewInit();

   setup(); 
   
   glutMainLoop(); 
}