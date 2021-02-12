///////////////////////////////////////////////////////////////////////          
// launchCameraBlurredcpp
//
// Camera blur of a texture implemented in the fragment shader.
//
// Interaction:
// Press space to toggle between blurred and not blurred.
//
// Sumanta Guha
//
// Texture Credits: See ExperimenterSource/Textures/TEXTURE_CREDITS.txt
/////////////////////////////////////////////////////////////////////// 

#include <cmath>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "prepShader.h"
#include "getBMP.h"
#include "vertex.h"

using namespace glm;

static enum object {LAUNCH}; // VAO ids.
static enum buffer {LAUNCH_VERTICES}; // VBO ids.

// Globals.
static Vertex launchVertices[4] =  
{
	{vec4(6.0, -6.0, -6.0, 1.0), vec2(1.0, 0.0)},
	{vec4(6.0, 6.0, -6.0, 1.0), vec2(1.0, 1.0)},
	{vec4(-6.0, -6.0, -6.0, 1.0), vec2(0.0, 0.0)},
	{vec4(-6.0, 6.0, -6.0, 1.0), vec2(0.0, 1.0)}
}; 

static mat4 modelViewMat, projMat;

static unsigned int
   programId,
   vertexShaderId,
   fragmentShaderId,
   modelViewMatLoc,
   projMatLoc,
   launchTexLoc, 
   isBlurredLoc,
   buffer[1], 
   vao[1],
   texture[1]; 

static imageFile *image[1]; // Local storage for bmp image data.
static uint isBlurred = 0; // Is the texture blurred?

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

   // Create VAOs and VBOs... 
   glGenVertexArrays(1, vao);
   glGenBuffers(1, buffer); 

   // ...and associate launch data with vertex shader.
   glBindVertexArray(vao[LAUNCH]);
   glBindBuffer(GL_ARRAY_BUFFER, buffer[LAUNCH_VERTICES]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(launchVertices), launchVertices, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
   glEnableVertexAttribArray(1);

   // Obtain projection matrix uniform location and set value.
   projMatLoc = glGetUniformLocation(programId,"projMat"); 
   projMat = frustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0); 
   glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));
   
   // Obtain modelview matrix uniform locations.
   modelViewMatLoc = glGetUniformLocation(programId,"modelViewMat");  

   // Obtain isBlurred uniform location and set value.
   isBlurredLoc = glGetUniformLocation(programId, "isBlurred");
   glUniform1uiv(isBlurredLoc, 1, &isBlurred);

   // Load the images.
   image[0] = getBMP("../../Textures/launch.bmp"); 

   // Create texture ids.
   glGenTextures(1, texture);

   // Bind launch image.
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture[0]);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0, 
	            GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   launchTexLoc = glGetUniformLocation(programId, "launchTex");
   glUniform1i(launchTexLoc, 0);
}

// Drawing routine.
void drawScene(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Calculate and update modelview matrix.
   modelViewMat = mat4(1.0);
   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat)); 

   // Draw launch.
   glBindVertexArray(vao[LAUNCH]);
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

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
		 isBlurred = isBlurred? 0 : 1;
		 glUniform1uiv(isBlurredLoc, 1, &isBlurred);
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
   std::cout << "Press space to toggle between blurred and not blurred." << std::endl; 
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
   glutCreateWindow("launchCameraBlurredcpp");
   glutDisplayFunc(drawScene); 
   glutReshapeFunc(resize);  
   glutKeyboardFunc(keyInput);

   glewExperimental = GL_TRUE;
   glewInit();

   setup(); 
   
   glutMainLoop(); 
}

