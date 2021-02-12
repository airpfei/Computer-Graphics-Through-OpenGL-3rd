////////////////////////////////////////////////////////////////          
// ballAndTorusShaderized.cpp
//
// Forward-compatible core GL 4.3 version of ballAndTorus.cpp.
//
// Interaction:
// Press space to toggle between animation on and off.
// Press the up/down arrow keys to speed up/slow down animation.
// Press the x, X, y, Y, z, Z keys to rotate the scene.
//
// Sumanta Guha
//////////////////////////////////////////////////////////////// 

#include <cmath>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "prepShader.h"
#include "sphere.h"
#include "torus.h"

using namespace glm;

static enum object {SPHERE, TORUS}; // VAO ids.
static enum buffer {SPH_VERTICES, SPH_INDICES, TOR_VERTICES, TOR_INDICES}; // VBO ids.

// Globals.
static float latAngle = 0.0; // Latitudinal angle.
static float longAngle = 0.0; // Longitudinal angle.
static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0; // Angles to rotate scene.
static int isAnimate = 0; // Animated?
static int animationPeriod = 100; // Time interval between frames.

// Sphere data.
static Vertex sphVertices[(SPH_LONGS + 1) * (SPH_LATS + 1)]; 
static unsigned int sphIndices[SPH_LATS][2*(SPH_LONGS+1)]; 
static int sphCounts[SPH_LATS];
static void* sphOffsets[SPH_LATS]; 
static vec4 sphColors = vec4(SPH_COLORS); 

// Torus data.
static Vertex torVertices[(TOR_LONGS + 1) * (TOR_LATS + 1)]; 
static unsigned int torIndices[TOR_LATS][2*(TOR_LONGS+1)]; 
static int torCounts[TOR_LATS];
static void* torOffsets[TOR_LATS]; 
static vec4 torColors = vec4(TOR_COLORS);

static mat4 modelViewMat, projMat;

static unsigned int
   programId,
   vertexShaderId,
   fragmentShaderId,
   modelViewMatLoc,
   projMatLoc,
   objectLoc,
   sphColorLoc,
   torColorLoc,
   buffer[4], 
   vao[2]; 

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

   // Initialize shpere and torus.
   fillSphere(sphVertices, sphIndices, sphCounts, sphOffsets);
   fillTorus(torVertices, torIndices, torCounts, torOffsets);

   // Create VAOs and VBOs... 
   glGenVertexArrays(2, vao);
   glGenBuffers(4, buffer); 

   // ...and associate sphere data with vertex shader.
   glBindVertexArray(vao[SPHERE]);  
   glBindBuffer(GL_ARRAY_BUFFER, buffer[SPH_VERTICES]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(sphVertices), sphVertices, GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[SPH_INDICES]);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphIndices), sphIndices, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
   glEnableVertexAttribArray(0);

   // ...and associate torus data with vertex shader.
   glBindVertexArray(vao[TORUS]);
   glBindBuffer(GL_ARRAY_BUFFER, buffer[TOR_VERTICES]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(torVertices), torVertices, GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[TOR_INDICES]);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(torIndices), torIndices, GL_STATIC_DRAW);
   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
   glEnableVertexAttribArray(1);

   // Obtain projection matrix uniform location and set value.
   projMatLoc = glGetUniformLocation(programId,"projMat"); 
   projMat = frustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0); 
   glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

   // Obtain color uniform locations and set values.
   sphColorLoc = glGetUniformLocation(programId, "sphColor");
   glUniform4fv(sphColorLoc, 1, &sphColors[0]);
   torColorLoc = glGetUniformLocation(programId, "torColor");
   glUniform4fv(torColorLoc, 1, &torColors[0]);

   // Obtain modelview matrix uniform and object uniform locations.
   modelViewMatLoc = glGetUniformLocation(programId,"modelViewMat"); 
   objectLoc = glGetUniformLocation(programId, "object");

   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

// Drawing routine.
void drawScene(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Calculate modelview matrix.
   modelViewMat = mat4(1.0);
   modelViewMat = translate(modelViewMat, vec3(0.0, 0.0, -25.0));
   modelViewMat = rotate(modelViewMat, radians(Zangle), vec3(0.0, 0.0, 1.0));
   modelViewMat = rotate(modelViewMat, radians(Yangle), vec3(0.0, 1.0, 0.0));
   modelViewMat = rotate(modelViewMat, radians(Xangle), vec3(1.0, 0.0, 0.0));

   // Draw torus.
   glUniform1ui(objectLoc, TORUS); // Update object name.
   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat)); // Update modelview matrix.
   glBindVertexArray(vao[TORUS]);
   glMultiDrawElements(GL_TRIANGLE_STRIP, torCounts, GL_UNSIGNED_INT, (const void **)torOffsets, TOR_LATS);

   // Calculate modelview matrix.
   modelViewMat = rotate(modelViewMat, radians(longAngle), vec3(0.0, 0.0, 1.0));
   modelViewMat = translate(modelViewMat, vec3(12.0, 0.0, 0.0));
   modelViewMat = rotate(modelViewMat, radians(latAngle), vec3(0.0, 1.0, 0.0));
   modelViewMat = translate(modelViewMat, vec3(-12.0, 0.0, 0.0));
   modelViewMat = translate(modelViewMat, vec3(20.0, 0.0, 0.0));
   
   // Draw ball.
   glUniform1ui(objectLoc, SPHERE); // Update object name.
   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat)); // Update modelview matrix.
   glBindVertexArray(vao[SPHERE]);
   glMultiDrawElements(GL_TRIANGLE_STRIP, sphCounts, GL_UNSIGNED_INT, (const void **)sphOffsets, SPH_LATS);

   glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
   glViewport(0, 0, w, h); 
}

// Timer function.
void animate(int value)
{
   if (isAnimate) 
   {
      latAngle += 5.0;
	  if (latAngle > 360.0) latAngle -= 360.0;
      longAngle += 1.0;
	  if (longAngle > 360.0) longAngle -= 360.0;

	  glutPostRedisplay();
      glutTimerFunc(animationPeriod, animate, 1);
   }
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
         if (isAnimate) isAnimate = 0;
		 else 
		 {
	        isAnimate = 1; 
			animate(1);
		 }
		 break;
      case 'x':
         Xangle += 5.0;
		 if (Xangle > 360.0) Xangle -= 360.0;
         glutPostRedisplay();
         break;
      case 'X':
         Xangle -= 5.0;
		 if (Xangle < 0.0) Xangle += 360.0;
         glutPostRedisplay();
         break;
      case 'y':
         Yangle += 5.0;
		 if (Yangle > 360.0) Yangle -= 360.0;
         glutPostRedisplay();
         break;
      case 'Y':
         Yangle -= 5.0;
		 if (Yangle < 0.0) Yangle += 360.0;
         glutPostRedisplay();
         break;
      case 'z':
         Zangle += 5.0;
		 if (Zangle > 360.0) Zangle -= 360.0;
         glutPostRedisplay();
         break;
      case 'Z':
         Zangle -= 5.0;
		 if (Zangle < 0.0) Zangle += 360.0;
         glutPostRedisplay();
         break;
      default:
         break;
   }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
   if (key == GLUT_KEY_DOWN) animationPeriod += 5;
   if( key == GLUT_KEY_UP) if (animationPeriod > 5) animationPeriod -= 5;
   glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
   std::cout << "Interaction:" << std::endl;
   std::cout << "Press space to toggle between animation on and off." << std::endl
	    << "Press the up/down arrow keys to speed up/slow down animation." << std::endl
        << "Press the x, X, y, Y, z, Z keys to rotate the scene." << std::endl;
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
   glutCreateWindow("ballAndTorusShaderized.cpp");
   glutDisplayFunc(drawScene); 
   glutReshapeFunc(resize);  
   glutKeyboardFunc(keyInput);
   glutSpecialFunc(specialKeyInput);

   glewExperimental = GL_TRUE;
   glewInit();

   setup(); 
   
   glutMainLoop(); 
}

