//////////////////////////////////////////////////////////////////////////          
// particleSystem.cpp
//
// Forward-compatible core GL 4.3 program demonstrating a minimal particle
// system using transform feedback with ping pong buffering.
//
// Interaction:
// Press space to step through the animation.
//
// Sumanta Guha
////////////////////////////////////////////////////////////////////////// 

#include <cmath>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "prepShader.h"
#include "vertex.h"
#include "particle.h"

using namespace glm;

static enum object {PARTICLES_A, PARTICLES_B, SQUARE}; // VAO ids.
static enum bufferAndTransformFeedbackIds {TRANSFORM_FEEDBACK_A, TRANSFORM_FEEDBACK_B, SQUARE_VERTICES}; // VBO ids.
static enum programObjectIds {PARTICLE_PROG, SQUARE_PROG};

// Globals.
static mat4 modelViewMat, projMat;

static unsigned int
   programId[2],
   vertexShaderId,
   fragmentShaderId,
   geometryShaderId,
   modelViewMatLoc,
   projMatLoc,
   objectLoc,
   buffer[3], 
   vao[3],
   transformFeedback[2];

static int frameParity = 1;
static int firstFrame = 1;

// Bounding square vertices.
static Vertex squareVertices[] =  
{
   vec4(-95.0, -95.0, 0.0, 1.0),
   vec4(95.0, -95.0, 0.0, 1.0),
   vec4(95.0, 95.0, 0.0, 1.0),
   vec4(-95.0, 95.0, 0.0, 1.0)
};

// Seed particle.
static Particle initParticle = 
{ 
	vec4(0.0, 0.0, 0.0, 1.0), // Coordinates.
	vec3(0.0, 1.0, 0.0) // Velocity.
};

// Varyings.
static const char * varyings[] = {"updatedCoords", "updatedVel"};

// Initialization routine.
void setup(void) 
{
   glClearColor(1.0, 1.0, 1.0, 0.0); 
   glEnable(GL_DEPTH_TEST);

   // Create shaders.
   vertexShaderId = setShader("vertex", "Shaders/vertexShader.glsl");
   geometryShaderId = setShader("geometry", "Shaders/geometryShader.glsl");
   fragmentShaderId = setShader("fragment", "Shaders/fragmentShader.glsl");

   // Create shader program executable to draw the particles attaching vertex, geometry and fragment shaders.
   programId[PARTICLE_PROG] = glCreateProgram(); 
   glAttachShader(programId[PARTICLE_PROG], vertexShaderId); 
   glAttachShader(programId[PARTICLE_PROG], geometryShaderId);
   glAttachShader(programId[PARTICLE_PROG], fragmentShaderId);    
   glTransformFeedbackVaryings(programId[PARTICLE_PROG], 2, varyings, GL_INTERLEAVED_ATTRIBS);
   glLinkProgram(programId[PARTICLE_PROG]); 

   // Create shader program executable to draw the bounding square attaching only the vertex and fragment shaders.
   programId[SQUARE_PROG] = glCreateProgram(); 
   glAttachShader(programId[SQUARE_PROG], vertexShaderId); 
   glAttachShader(programId[SQUARE_PROG], fragmentShaderId);  
   glLinkProgram(programId[SQUARE_PROG]); 

   // Create VAOs, VBOs and transform feedback objects. 
   glGenVertexArrays(3, vao);
   glGenBuffers(3, buffer); 
   glGenTransformFeedbacks(2, transformFeedback);

   // PING PONG BUFFERING: Both the two buffers below are linked to the same logical vertex shader locations.

   // Attach buffer[TRANSFORM_FEEDBACK_A] to vao[PARTICLES_A] and initialize with seed particle data.
   glBindVertexArray(vao[PARTICLES_A]);
   glBindBuffer(GL_ARRAY_BUFFER, buffer[TRANSFORM_FEEDBACK_A]);
   glBufferData(GL_ARRAY_BUFFER, 128*sizeof(initParticle), &initParticle, GL_DYNAMIC_COPY);
   glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, vel));
   glEnableVertexAttribArray(1);

   // Attach buffer[TRANSFORM_FEEDBACK_B] to vao[PARTICLES_B] and initialize with null data.
   glBindVertexArray(vao[PARTICLES_B]);
   glBindBuffer(GL_ARRAY_BUFFER, buffer[TRANSFORM_FEEDBACK_B]);
   glBufferData(GL_ARRAY_BUFFER, 128*sizeof(initParticle), NULL, GL_DYNAMIC_COPY);
   glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, vel));
   glEnableVertexAttribArray(1);

   // Associate square data with vertex shader.
   glBindVertexArray(vao[SQUARE]);
   glBindBuffer(GL_ARRAY_BUFFER, buffer[SQUARE_VERTICES]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
   glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
   glEnableVertexAttribArray(2);

   // Point size.
   glPointSize(5.0); 

   // START specifying the particle-drawing program variables.
   glUseProgram(programId[PARTICLE_PROG]); 

   // Obtain projection matrix and modelview matrix uniform locations and set values.
   projMatLoc = glGetUniformLocation(programId[PARTICLE_PROG],"projMat"); 
   projMat = ortho(-100.0, 100.0, -100.0, 100.0, -1.0, 1.0); 
   glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));
   modelViewMatLoc = glGetUniformLocation(programId[PARTICLE_PROG],"modelViewMat"); 
   modelViewMat = mat4(1.0);
   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat)); 

   // Obtain object uniform location.
   objectLoc = glGetUniformLocation(programId[PARTICLE_PROG], "object");
   // END specifying the particle-drawing program variables.
   
   // START specifying the square-drawing program variables.   
   glUseProgram(programId[SQUARE_PROG]); 

   // Obtain projection matrix and modelview matrix uniform locations and set values.
   projMatLoc = glGetUniformLocation(programId[PARTICLE_PROG],"projMat"); 
   projMat = ortho(-100.0, 100.0, -100.0, 100.0, -1.0, 1.0); 
   glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));
   modelViewMatLoc = glGetUniformLocation(programId[PARTICLE_PROG],"modelViewMat"); 
   modelViewMat = mat4(1.0);
   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat)); 

   // Obtain object uniform location.
   objectLoc = glGetUniformLocation(programId[PARTICLE_PROG], "object");
   // END specifying the square-drawing program variables.
}

// Drawing routine.
void draw(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Draw particles.
   glUseProgram(programId[PARTICLE_PROG]); 
   
   // Frame parity toggles between the two ping pong buffers.
   if (frameParity == 0)
   {
      // Read/draw particles from buffer[TRANSFORM_FEEDBACK_A] while applying transform feedback to update buffer[TRANSFORM_FEEDBACK_B].
	  // For the first frame draw the seed particle using glDrawArrays(); after that use glDrawTransformFeedback() with particle
	  // count obtained from transform feedback object TRANSFORM_FEEDBACK_B which updated buffer[TRANSFORM_FEEDBACK_A] the previous cycle.
	  glBindVertexArray(vao[PARTICLES_A]);
      glUniform1ui(objectLoc, PARTICLES_A); 
      glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[TRANSFORM_FEEDBACK_A]);
      glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffer[TRANSFORM_FEEDBACK_B]);
      glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer[TRANSFORM_FEEDBACK_B]); 
      glBeginTransformFeedback(GL_POINTS);
      if (firstFrame == 1) // First frame. 
      {
         glDrawArrays(GL_POINTS, 0, 1);
	     firstFrame = 0;
      }
      else // Not first frame.
         glDrawTransformFeedback(GL_POINTS, transformFeedback[TRANSFORM_FEEDBACK_B]);
      glEndTransformFeedback();
   }
   
   else // (frameParity == 1)
   {
      // Read/draw particles from buffer[TRANSFORM_FEEDBACK_B] while applying transform feedback to update buffer[TRANSFORM_FEEDBACK_A].
	  // Draw using glDrawTransformFeedback() with particle count obtained from transform feedback object TRANSFORM_FEEDBACK_A which 
	  // updated buffer[TRANSFORM_FEEDBACK_B] the previous cycle.
      glBindVertexArray(vao[PARTICLES_B]);
      glUniform1ui(objectLoc, PARTICLES_B); 
      glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[TRANSFORM_FEEDBACK_B]);
      glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffer[TRANSFORM_FEEDBACK_A]);
      glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer[TRANSFORM_FEEDBACK_A]);   
      glBeginTransformFeedback(GL_POINTS);
      glDrawTransformFeedback(GL_POINTS, transformFeedback[TRANSFORM_FEEDBACK_A]);
      glEndTransformFeedback();
   }

   // Draw the bounding square.
   glUseProgram(programId[SQUARE_PROG]); 
   glUniform1ui(objectLoc, SQUARE); 
   glBindVertexArray(vao[SQUARE]);
   glDrawArrays(GL_LINE_LOOP, 0, 4);

   glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
   glViewport(0, 0, w, h); 
}

// Animation step,
void step(void)
{
   if (frameParity == 0) frameParity = 1; else frameParity = 0; // Toggle parity.
   glutPostRedisplay();
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
		 step();
         break;
      default:
         break;
   }
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
   std::cout << "Interaction:" << std::endl;
   std::cout << "Press space to step through the animation." << std::endl;
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
   glutCreateWindow("particleSystem.cpp");
   glutDisplayFunc(draw); 
   glutReshapeFunc(resize);  
   glutKeyboardFunc(keyInput);

   glewExperimental = GL_TRUE;
   glewInit();

   setup(); 
   
   glutMainLoop(); 
}

