////////////////////////////////////////////////////////////////////////////////////////////////////////
// tessellatedSphere.cpp
//
// Forward-compatible core GL 4.3 program drawing a tessellated sphere with
// tessellation levels automatically changing based on distance from eye.
//
// Interaction:
// Press the up/down arrow keys to move the sphere.
// Press space to toggle between filled and wireframe.
// Press 'm' to toggle between distance-based tessellation and maximum tessellation regardless distance.
//
// Sumanta Guha
////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "prepShader.h"
#include "light.h"
#include "material.h"

using namespace glm;

#define SPH_RADIUS 25.0 // Sphere radius.

static enum object {SPHERE};          

// Globals.
static int isWire = 0; // Is wireframe?
static int maxTess = 0; // Keep maximum tessellation always.

// Light properties.
static const Light light0 = 
{
	vec4(0.0, 0.0, 0.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 1.5, 3.0, 0.0)
};

// Global ambient.
static const vec4 globAmb = vec4(0.2, 0.2, 0.2, 1.0);

// Front material properties.
static const Material sphFront = 
{
	vec4(0.9, 0.0, 0.0, 1.0),
	vec4(0.9, 0.0, 0.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 0.0, 0.0, 1.0),
	50.0
};

// Sphere data.
static float sphRadius = SPH_RADIUS; 
static float zTrans = 0.0; // Displacement in the -z direction.

static mat4 modelViewMat, projMat;
static mat3 normalMat;

static unsigned int
   programId,
   vertexShaderId,
   tessEvaluationShaderId,
   fragmentShaderId,
   modelViewMatLoc,
   projMatLoc,
   normalMatLoc,
   sphRadiusLoc,
   vao[1];

// Initialization routine.
void setup(void) 
{
   glClearColor(1.0, 1.0, 1.0, 0.0); 
   glEnable(GL_DEPTH_TEST);

   // Create shader program executable.
   vertexShaderId = setShader("vertex", "Shaders/vertexShader.glsl");
   tessEvaluationShaderId = setShader("tessEvaluation", "Shaders/tessEvaluationShader.glsl");
   fragmentShaderId = setShader("fragment", "Shaders/fragmentShader.glsl");
   programId = glCreateProgram(); 
   glAttachShader(programId, vertexShaderId); 
   glAttachShader(programId, tessEvaluationShaderId);
   glAttachShader(programId, fragmentShaderId);
   glLinkProgram(programId); 
   glUseProgram(programId); 

   // Create VAO ... 
   glGenVertexArrays(1, vao);

   // ... bind to sphere.
   glBindVertexArray(vao[SPHERE]);

   // Obtain projection matrix uniform location and set value.
   projMatLoc = glGetUniformLocation(programId,"projMat"); 
   projMat = frustum(-5.0, 5.0, -5.0, 5.0, 5.0, 500.0); 
   glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

   // Obtain modelview matrix and normal matrix uniform locations.
   modelViewMatLoc = glGetUniformLocation(programId,"modelViewMat"); 
   normalMatLoc = glGetUniformLocation(programId,"normalMat"); 

   // Obtain light property uniform locations and set values.
   glUniform4fv(glGetUniformLocation(programId, "light0.ambCols"), 1, &light0.ambCols[0]);
   glUniform4fv(glGetUniformLocation(programId, "light0.difCols"), 1, &light0.difCols[0]);
   glUniform4fv(glGetUniformLocation(programId, "light0.specCols"), 1, &light0.specCols[0]);
   glUniform4fv(glGetUniformLocation(programId, "light0.coords"), 1, &light0.coords[0]);

   // Obtain global ambient uniform location and set value.
   glUniform4fv(glGetUniformLocation(programId, "globAmb"), 1, &globAmb[0]);
   
   // Obtain front material property uniform locations and set values.
   glUniform4fv(glGetUniformLocation(programId, "sphFront.ambRefl"), 1, &sphFront.ambRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "sphFront.difRefl"), 1, &sphFront.difRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "sphFront.specRefl"), 1, &sphFront.specRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "sphFront.emitCols"), 1, &sphFront.emitCols[0]);
   glUniform1f(glGetUniformLocation(programId, "sphFront.shininess"), sphFront.shininess);

   // Obtain sphere radius uniform location and set value.
   sphRadiusLoc = glGetUniformLocation(programId, "sphRadius");
   glUniform1f(sphRadiusLoc, sphRadius); 

   // Input patch size.
   glPatchParameteri(GL_PATCH_VERTICES, 1);
}

// Drawing routine.
void drawScene(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Set tessellation levels.
   float tessLevel;
   if (maxTess == 1) tessLevel = 50.0;
   else tessLevel = 50.0 - (floor(zTrans/50.0) * 10.0);
   tessLevel = (tessLevel > 10.0) ? tessLevel : 10.0;
   float tessLevelInner[2] = {tessLevel, tessLevel};
   float tessLevelOuter[4] = {tessLevel, tessLevel, tessLevel, tessLevel};
   glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, tessLevelOuter);
   glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, tessLevelInner);

   // Calculate and update modelview matrix.
   modelViewMat = mat4(1.0);
   modelViewMat = translate(modelViewMat, vec3(0.0, 0.0, -35.0 - zTrans));
   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));

   // Calculate and update normal matrix.
   normalMat = transpose(inverse(mat3(modelViewMat)));
   glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, value_ptr(normalMat));

   if (isWire) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   // Draw sphere.
   glBindVertexArray(vao[SPHERE]);
   glDrawArrays(GL_PATCHES, 0, 1);
 
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
         if (isWire == 0) isWire = 1;
         else isWire = 0;
         glutPostRedisplay();
         break;	 
      case 'm':
         if (maxTess == 0) maxTess = 1;
         else maxTess = 0;
         glutPostRedisplay();
         break;	
      default:
         break;
   }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
   if (key == GLUT_KEY_UP) if (zTrans < 450.0) ++zTrans;
   if (key == GLUT_KEY_DOWN) if (zTrans > 0.0) --zTrans;
   
   glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
   std::cout << "Interaction:" << std::endl;
   std::cout << "Press the up/down arrow keys to move the sphere." << std::endl
		<< "Press space to toggle between filled and wireframe." << std::endl
	    << "Press 'm' to toggle between distance-based tessellation and maximum tessellation regardless distance." ;
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
   glutCreateWindow("tessellatedSphere.cpp");
   glutDisplayFunc(drawScene); 
   glutReshapeFunc(resize);  
   glutKeyboardFunc(keyInput);
   glutSpecialFunc(specialKeyInput);

   glewExperimental = GL_TRUE;
   glewInit();

   setup(); 
   
   glutMainLoop(); 
}

